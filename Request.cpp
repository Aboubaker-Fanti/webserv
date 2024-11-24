#include "Request.hpp"

ChildProcess::ChildProcess(): id(-2), rtn_wt(0), bgn_tm(0), end(0) { }

ChildProcess::ChildProcess(pid_t id): id(id) { }

ChildProcess::ChildProcess(const ChildProcess& o) {
	(*this) = o;
}

ChildProcess ChildProcess::operator=(const ChildProcess& o) {
	this->id = o.id;
	this->bgn_tm = o.bgn_tm;
	return (*this);
}

string Request::pathTOFile() {
	size_t pos = path.find_last_of('/');
	if (pos == location["root"].size() - 1)
		return (path);
	return (path.substr(0, pos));
}

Request::Request():
header_size(0), content_length(-1),
sz_bdy(0), fin_rd(0), fin_rq(0), port(0), rdfirst_rq(0),
cnl(ContentLengthRead(&this->body, &this->fin_rd, &this->sz_bdy)),
chk(&this->body, &this->fin_rd, &this->max_size), kid_proc(new ChildProcess()){
}

Request::Request(int socket):
socket(socket), header_size(0), content_length(-1),
sz_bdy(0), fin_rd(0), fin_rq(0), port(0), rdfirst_rq(0), smart(0), fail_rd(0),
cnl(ContentLengthRead(&this->body, &this->fin_rd, &this->sz_bdy)),
chk(&this->body, &this->fin_rd, &this->max_size), kid_proc(new ChildProcess()){
}

Request::Request(int socket, int port):
socket(socket),  header_size(0), content_length(-1),
sz_bdy(0), fin_rd(0), fin_rq(0), port(port), rdfirst_rq(0), smart(0), fail_rd(0),
cnl(ContentLengthRead(&this->body, &this->fin_rd, &this->sz_bdy)),
chk(&this->body, &this->fin_rd, &this->max_size), kid_proc(new ChildProcess()){
}

Request::Request(int socket, int port, Webserve& paramserv):
socket(socket), vepoll(0), header_size(0), content_length(-1),
sz_bdy(0), fin_rd(0), fin_rq(0), port(port), rdfirst_rq(0), smart(0), fail_rd(0),
cnl(ContentLengthRead(&this->body, &this->fin_rd, &this->sz_bdy)),
chk(&this->body, &this->fin_rd, &this->max_size), kid_proc(new ChildProcess()){
	int nb = paramserv.find_server_using_port(port);
	Server serv = paramserv.getServers()[nb];
	this->error_page = serv.error_page;
	c_type = &paramserv.c_type;
	n_file = &paramserv.n_file;
}


Request::Request(const Request& o) {
	this->socket = o.socket;
}

Request& Request::operator=(const Request& o) {
	this->socket = o.socket;
	this->port = o.port;
	return (*this);
}

int Request::getFinRd() {
	return (this->fin_rd);
}

int Request::getFinRq() {
	return (this->fin_rq);
}

int Request::getVEpoll() {
	return (vepoll);
}

void Request::setFinRd(int b) {
	fin_rd = b;
}

void Request::setVEpoll(bool v) {
	vepoll = v;
}

void Request::setResStat(string rtn_status) {
	res_stat =rtn_status;
	fin_rd = 1;
}

void Request::unchunkBody(char* request, size_t pos, unsigned int valread) {
	try {
		this->chk = ChunkReader(request, valread, pos);
		this->chk.start ? this->chk.unchunkBodyStart() : this->chk.unchunkBody();		
	} catch (std::string &e) { throw e; }
}

void Request::contentLengthRequest(char* request, unsigned int valread) {
	if (max_size && (size_t)content_length > max_size)
		throw string("request body size very large \'"
			+ int_to_string(content_length) + " > " + int_to_string(max_size) + "\"");
	if (content_length == 0) {fin_rd = 1; return ;}
	this->cnl = ContentLengthRead(request, valread, content_length);
	cnl.execute();
}

void Request::readFirstRequest(Webserve& paramserv) {
	char buffer[BUFFER_SIZE ] = {0};
	int valread = 0;
	valread = read(socket, buffer, BUFFER_SIZE);
	if (failReadOrWrite(READ , &valread))
		return ;
	if (!headerIsFull(buffer, valread) || rdfirst_rq == 1)
		return ;
	if ((!full_header(first_rq, paramserv)
		|| parseRequest() || !openFile())) 
		return ;
	try {
		if (method == "POST")
			(content_length >= 0) ?
			contentLengthRequest((char *)(first_rq.c_str() + this->header_size), (rdfirst_rq * BUFFER_SIZE + valread) - this->header_size) :
			unchunkBody((char *)first_rq.c_str(), header_size, (rdfirst_rq * BUFFER_SIZE + valread));
		if (method != "POST")
			fin_rd = 1;
		executeFirstRequest();
	} catch (string &e) {
		unlink(post_file.c_str());
		setResStat("413");
	}
}

void Request::cgiRequest() {
	if (method != "POST" || fin_rd)
		(kid_proc->name_out.empty())?
		firstCgiExecute() : executeCgi();
}

void Request::executeFirstRequest() {
	if (location.count("cgi rq"))
		(method == "POST") ?
		firstPostRequest() : cgiRequest();
	else if (this->method == "POST")
		this->firstPostRequest();
	else if (this->method == "DELETE")
		this->deleteRequest();
}

void Request::executeRequest(Webserve& paramserv) {
	if (!res_stat.empty()) {
		if (res_stat[0] == '4' || res_stat[0] == '5')
			responseError(res_stat);
		else {
			Response(socket, res_stat, "").writeResponse(*this);
			fin_rq = 1;
		}

	}
	if (version.empty()) {
		Request::readFirstRequest(paramserv);
		return ;
	}
	if (location.count("cgi rq"))
		(method == "POST" && !fin_rd) ?
		postRequest() : cgiRequest();
	else if (this->method == "POST" || !fin_rd)
		this->postRequest();
	else if (this->method == "GET")
		this->getRequest();
	else if (this->method == "DELETE")
		this->deleteRequest();
}

Request::~Request() {
	delete (kid_proc);
	if (file.is_open())
		file.close();
	if (file2.is_open())
		file2.close();
}

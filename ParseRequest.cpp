#include "Request.hpp"
#include <fcntl.h>

bool uriContainCharNotAllowed(std::string &path)
{
	size_t i = 0;
	std::string allowed_char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-._~:/?#[]@!$&'()*+,;=%";

	while (i < path.size())
		if (allowed_char.find(path[i++]) == std::string::npos)
			return (1);
	return (0);
}

void Request::responseError(const string error_code)
{
	res_stat.clear();
	if (error_page.count(error_code) && method != "HEAD")
	{
		if (location.count("cgi rq"))
			location.erase("cgi rq");
		smart = 10;
		method = "GET";
		path = error_page["root"] + "/" + error_page[error_code];
		file2.open(path.c_str(), std::ifstream::binary);
		Response(socket, error_code, "", 1).writeResponse(*this);
	}
	else
	{
		Response(socket, error_code, "").writeResponse(*this);
		fin_rq = 1;
	}
}

bool Request::full_header(const std::string &request, Webserve &paramserv)
{
	int separator = 0;
	std::vector<std::string> header;
	std::vector<std::string> lines;
	std::vector<std::string> header_body;
	if (!request.size() && request.empty())
		return (version = "error", setResStat("400"), 0);
	if (first_rq.find("\r\n") < first_rq.find("\n"))
	{
		header_body = splitString(request, "\r\n\r\n");
		if (header_body.empty())
			return (version = "error", setResStat("400"), 0);
		lines = splitString(header_body[0], "\r\n");
	}
	else
	{
		header_body = splitString(request, "\n\n");
		if (header_body.empty())
			return (version = "error", setResStat("400"), 0);
		lines = splitString(header_body[0], "\n");
	}
	if (lines.empty())
		return (version = "error", setResStat("400"), 0);
	if (first_rq.find("\r\n\r\n") != string::npos)
		separator = 4;
	else if (first_rq.find("\n\n") != string::npos)
		separator = 2;
	std::vector<std::string> first_line = splitString(lines[0], " ");
	if (first_line.size() != 3 || lines.size() < 3)
		return (version = "error", setResStat("400"), 0);
	if (first_line[1].size() > 2048)
		return (version = "error", setResStat("414"), 0);
	this->method = first_line[0];
	this->path = first_line[1];
	this->version = first_line[2];
	unsigned int i = 1;
	int xln = (request.find("\r\n\r\n") || request.find("\n\n")) ? 0 : 1;
	while (i < lines.size() - xln)
	{
		header = splitString(lines[i++], ": ");
		if (header.size() != 2)
			return (setResStat("400"), 0);
		this->headers[header[0]] = header[1];
	}
	this->header_size = header_body[0].size() + separator;
	if (this->headers.count("Content-Length"))
		this->content_length = strToLong(this->headers["Content-Length"]);
	if (!confParams(paramserv)["return"].empty())
		return (Response(socket, "301", location["return"]).writeResponse(*this), fin_rq = 1, 0);
	return (1);
}
std::map<std::string, std::string> Request::confParams(Webserve &paramserv)
{
	int nb;
	(headers.count("Host")) ? location = paramserv.map_location(path, port, headers["Host"]) : location = paramserv.map_location(path, port);

	(headers.count("Host")) ? nb = paramserv.find_server_using_port_host(port, headers["Host"]) : nb = paramserv.find_server_using_port(port);

	Server serv = paramserv.getServers()[nb];
	error_page = serv.error_page;

	if (location.count("Error") || !location["return"].empty())
		return (location);

	if (path.find_first_of('?') != std::string::npos)
	{
		query_str = path.substr(path.find_first_of('?') + 1, ((path.size() - path.find_first_of('?'))));
		path = path.substr(0, path.find_first_of('?'));
	}
	if (uriContainCharNotAllowed(path))
		return (location["Uri contain character not allowded"], location);
	replaceEncodingCharacter(path, 0);
	old_path = path;
	if (location.count("rtn index"))
		path += "/" + location["index"];
	path = path.substr(location["path"].size() - 1);
	path = location["root"].empty() ? serv.get_root() + path : location["root"] + path;
	location["index"] = location["index"].empty() ? serv.get_index_home() : location["index"];

	if (location.count("cgi") && location["cgi"] == "on" && method != "DELETE")
	{
		if (path.find(".php", path.size() - 5) != string::npos && location["cgi-ext"].find(".php") != string::npos)
			location["cgi rq"] = "/usr/bin/php-cgi";
		else if (path.find(".py", path.size() - 4) != string::npos && location["cgi-ext"].find(".py") != string::npos)
			location["cgi rq"] = "/usr/bin/python3";
	}
	if (location["max size"].empty())
		location["max size"] = serv.get_client_max_size();
	max_size = strToLong(location["max size"]);
	return location;
}

bool Request::openFile()
{
	if (!pathExists())
		return (0);
	if (location.count("cgi rq"))
	{
		kid_proc->name_in = generateRandomInputName(socket);
		file.open(kid_proc->name_in.c_str(), ios_base::out);
	}
	else if (method == "POST")
	{
		generatePostFileName();
		file.open(post_file.c_str(), ios_base::trunc | ios_base::out | std::ifstream::binary);
	}
	else if (method == "GET")
		file2.open(path.c_str(), std::ifstream::binary);
	if (method != "DELETE" && !file.is_open() && !file2.is_open())
		return (setResStat("404"), 0);
	return (1);
}

bool Request::parseRequest()
{
	if (this->version != "HTTP/1.1")
		return (setResStat("505"), 1);
	if (method != "POST" && first_rq.size() > this->header_size)
		return (setResStat("400"), 1);
	else if (this->headers.count("Transfer-Encoding") && this->headers["Transfer-Encoding"] != "chunked")
		return (setResStat("501"), 1);
	else if ((this->headers.count("Transfer-Encoding") && this->headers.count("Content-Length")) || (!this->headers.count("Transfer-Encoding") && !this->headers.count("Content-Length") && this->method == "POST") ||
			 (location.count("Uri contain character not allowded")))
		return (setResStat("400"), 1);
	else if (path.size() > 2048)
		return (setResStat("414"), 1);
	else if (location.count("Error"))
		return (setResStat("404"), 1);
	else if (method != "GET" && method != "POST" && method != "DELETE")
		return (setResStat("501"), 1);
	else if (location["allow_methods"].find(method) == std::string::npos)
		return (setResStat("405"), 1);
	return (0);
}

bool Request::failReadOrWrite(int rdwt, int *rtn_vl)
{
	(*rtn_vl == 0 || *rtn_vl == -1) ? fail_rd++ : fail_rd = 0;
	if ((rdwt == READ) && (fail_rd == 1))
	{
		(location.count("cgi rq")) ? (unlink(kid_proc->name_in.c_str())) : (unlink(post_file.c_str()));
		setResStat("400");
		fin_rq = 1;
		return (1);
	}
	else if ((rdwt == WRITE) && (fail_rd == 1))
	{
		setResStat("400");
		fin_rq = 1;
		return (1);
	}
	if (*rtn_vl == -1)
	{
		*rtn_vl = 0;
		return (1);
	}
	return (0);
}

bool Request::failReadOrWrite(int rdwt, bool is_fail)
{
	(is_fail) ? fail_rd++ : fail_rd = 0;
	if ((rdwt == READ) && (fail_rd == 1))
	{
		(location.count("cgi rq")) ? (unlink(kid_proc->name_in.c_str())) : (unlink(post_file.c_str()));
		setResStat("400");
		fin_rq = 1;
		return (1);
	}
	return (0);
}

bool Request::headerIsFull(char *buffer, int valread)
{
	first_rq.append(buffer, valread);
	if (first_rq.find("\r\n\r\n") != std::string::npos || first_rq.find("\n\n") != std::string::npos || valread < BUFFER_SIZE)
		return (1);
	rdfirst_rq++;
	return (0);
}

bool Request::pathExists()
{
	struct stat s;
	if (method == "POST" && (location.count("cgi rq") || location["upload_pass_args"] == "on"))
		return (1);
	else if (method == "POST" && location["upload_pass_args"] != "on")
		return (setResStat("403"), 0);
	if (!realpath(path.c_str(), NULL) || string(realpath(path.c_str(), NULL)).find(location["root"]) != 0)
		return (setResStat("404"), 0);
	if (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFREG))
		return (1);
	if (method == "DELETE" && stat(path.c_str(), &s) == 0 && s.st_mode & S_IFDIR)
		return (1);
	if (stat(path.c_str(), &s) == 0 && (s.st_mode & S_IFDIR))
	{
		path = path + "/";
		string first_index = hasIndexFile(path);
		if (stat((path + first_index).c_str(), &s) == 0 && (s.st_mode & S_IFREG))
		{
			path += first_index;
			return (1);
		}
		if (method == "GET" && location["autoindex"] == "on")
			return 1;
		if (method == "GET" && location["autoindex"] == "off")
			return (setResStat("403"), 0);
	}
	return (setResStat("404"), 0);
}

#include "Response.hpp"
#include <sstream>

Response::Response() {
}

std::string fn_status_message(const std::string& status_code) {
	if (status_code == "200")
		return ("OK");
	else if (status_code == "201")
		return ("Created");
	else if (status_code == "202")
		return ("Accepted");
	else if (status_code == "204")
		return ("No Content");
	else if (status_code == "301")
		return ("Moved Permanently");
	else if (status_code == "302")
		return ("Found");
	else if (status_code == "304")
		return ("Not Modified");
	else if (status_code == "400")
		return ("Bad Request");
	else if (status_code == "401")
		return ("Unauthorized");
	else if (status_code == "403")
		return ("Forbidden");
	else if (status_code == "404")
		return ("Not Found");
	else if (status_code == "405")
		return ("Method Not Allowed");
	else if (status_code == "413")
		return ("Payload Too Large");
	else if (status_code == "414")
		return ("URI Too Long");
	else if (status_code == "500")
		return ("Internal Server Error");
	else if (status_code == "504")
		return ("Gateway Timeout");
	else if (status_code == "501")
		return ("Not Implemented");	
	else if (status_code == "505")
		return ("HTTP Version Not Supported");
	return ("");
}

std::map<std::string, std::string> responseHeaders(const std::string& status_code, const std::string& body, int nb) {
	std::map<std::string, std::string> headers;
	if (nb) {
		headers["Content-Type"] = "text/html";
		headers["Transfer-Encoding"] = "chunked";
	}
	else if (status_code[0] == '3')
		headers["Location"] = body;
	else {
		headers["Content-Type"] = "text/plain";
		headers["Content-Length"] = int_to_string(body.size());
	}
	return (headers);
}

Response::Response(int socket, const std::string& status_code, const std::map<std::string, std::string>& headers, const std::string& body):
socket(socket), status_code(status_code), status_message(fn_status_message(status_code)), body(body) {
	this->headers = headers;
}

Response::Response(int socket, const std::string& status_code, const std::string& body):
socket(socket), status_code(status_code), status_message(fn_status_message(status_code)), body(body) {
	this->headers = responseHeaders(status_code, body, 0);
	if (status_code[0] == '3')
		this->body.clear();
}

Response::Response(int socket, const std::string& status_code, const std::string& body, int nb):
socket(socket), status_code(status_code), status_message(fn_status_message(status_code)), body(body) {
	this->headers = responseHeaders(status_code, body, nb);
}

Response::~Response() {
}

std::string Response::getResponse() {
	std::string response;
	response = "HTTP/1.1 " + this->status_code + " " + this->status_message + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++) {
		response += it->first + ": " + it->second + "\r\n";
	}
	response += "\r\n";
	response += this->body;
	return (response);
}

void Response::writeResponse(Request& rq) {
	std::string rp = getResponse();
	int vl = write(socket, rp.c_str(), rp.size());
	if (rq.failReadOrWrite(WRITE, &vl))
		return;
}

void Response::writeResponse() {
	std::string rp = getResponse();
	write(socket, rp.c_str(), rp.size());
}

std::string Response::getResponse(int valread) {
	std::string response;
	response = "HTTP/1.1 " + this->status_code + " " + this->status_message + "\r\n";
	for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response += "\r\n";
	if (valread > 0)
		response.append(this->body.c_str(), valread);
	return (response);
}

void Response::writeResponse(Request& rq, int valread) {
	std::string rp = getResponse(valread);
	int vl = write(socket, rp.c_str(), rp.size());
	if (rq.failReadOrWrite(WRITE, &vl))
		return;
}
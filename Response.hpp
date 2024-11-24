#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <unistd.h>
# include <iostream>
# include <map>
# include "function_utils.hpp"
# include "Request.hpp"

class Request;

class Response {
private:
	int socket;
	std::string status_code;
	std::string status_message;
	std::map<std::string, std::string> headers;
	std::string body;
public:
	Response();
	Response(int socket, const std::string& status_code, const std::map<std::string, std::string>& headers, const std::string& body);
	Response(int socket, const std::string& status_code, const std::string& body);
	Response(int socket, const std::string& status_code, const std::string& body, int nb);
	~Response();
	std::string getResponse();
	void writeResponse(Request& rq);
	void writeResponse();
	std::string getResponse(int valread);
	void writeResponse(Request& rq, int valread);
};

#endif
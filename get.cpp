#include "Request.hpp"
#include "Response.hpp"
#include "Server.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <dirent.h>
#include <strings.h>
#include <sys/stat.h>
#include <cstring>
#include <iomanip>
#include <sys/wait.h>
#include <string>

static std::streampos getFileSize(const char* filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    if (!file)
        return -1; 
    return file.tellg();
}

int check_if_file(std::string path){
    const char* dir = path.c_str();
    struct stat sb;
    if (stat(dir, &sb) == 0 && !(sb.st_mode & S_IFDIR))
        return (1);
    else if (stat(dir, &sb) == 0 && (sb.st_mode & S_IFDIR))
        return (0);
	else
		return (-1);
}

bool find_string_inside_string(std::string str, std::string substr){
	size_t pos = str.find(substr);
	if (pos == std::string::npos)
		return (false);
	return (true);
}

void Request::getRequest() {
	if (smart == 0){
		if (!getFileSize(path.c_str())) {
			Response(socket, "200", "").writeResponse(*this);
			fin_rq = 1;
				return;
		}
	}
	if (fin_rq == 1){
		return;
	}
	int vl = 0;
	std::stringstream chunk_size_hex;
	std::string response;
	std::string last_path;
	std::string extantion = "";

	if (path[path.size() - 1] != '/' && check_if_file(path) == 1){
		if (path.find_last_of('/') != std::string::npos){
			last_path = path.substr(path.find_last_of('/'));
		}
		else
			last_path = path;
		if (last_path.find_last_of('.') != std::string::npos)
			extantion = (*c_type)[last_path.substr(last_path.find_last_of('.'))];
		if (extantion.empty() || extantion == "application/x-httpd-php" || extantion == "text/x-python")
			extantion = (*c_type)[".html"];
		if (smart == 0){
			response += "HTTP/1.1 200 OK\r\n";
			response += "Content-Type: ";
			response += extantion;
			response += "\r\n";
			response += "Transfer-Encoding: chunked\r\n\r\n";
			this->smart = 10;
		}
		char chunk[2048];
		file2.read(chunk, sizeof(chunk));
		chunk_size_hex << std::hex << file2.gcount() << "\r\n";
		chunk_size_hex.write(chunk, file2.gcount());
		chunk_size_hex << "\r\n" ;
		response.append(chunk_size_hex.str());
		if (!file2.eof())
		{
			vl = write(socket, response.c_str(), response.length());
			if (failReadOrWrite(WRITE, &vl))
				return;
		}
		std::memset(chunk, 0, sizeof(chunk));
		if (file2.eof()){
			response += "0\r\n\r\n";
			vl = write(socket, response.c_str(), response.length());
			if (failReadOrWrite(WRITE, &vl))
				return;
			if (vl == -1 || vl == 0) {
				fin_rq = 1;
				return ;
			}
			fin_rq = 1;
			smart = 0;
			file2.close();
		}
		response.clear();
		response = "";
	}
	if (check_if_file(path) == 0){
		std::string autoind = location["autoindex"];
		if (path[path.size() - 1] != '/' )
			path += "/";
		if (autoind == "off"){
			responseError("403");
		}
		else if (autoind == "on"){
			DIR* dir = opendir(path.c_str());
			if (dir) {
				response = "HTTP/1.1 200 OK\r\n";
				response += "Content-Type: text/html\r\n"; 
				response += "Transfer-Encoding: chunked\r\n\r\n";

				std::stringstream chunk_size_hex;
				chunk_size_hex.write(response.c_str(), response.length());
				struct dirent* entry;
				std::string chunk;
				while ((entry = readdir(dir)) != NULL) {
					std::string filename = entry->d_name;
					if (filename != "." && filename != ".."){
						if (old_path == "/")
							chunk = "<li><a href=\"" + filename  + "\">" + filename + "</a></li>\n";
						else
							chunk = "<li><a href=\"" + this->old_path + "/"+ filename + "\">" + filename + "</a></li>\n";
						chunk_size_hex << std::hex << chunk.length() << "\r\n";
						chunk_size_hex.write(chunk.c_str(), chunk.length());
						chunk_size_hex << "\r\n";
					}
				}
				closedir(dir);
				chunk_size_hex << "0\r\n\r\n";
				vl = write(socket, chunk_size_hex.str().c_str(), chunk_size_hex.str().length());
				if (vl == -1 || vl == 0) {
					fin_rq = 1;
					return ;
				}
			}
			fin_rq = 1;
		}
	}
	return ;
}
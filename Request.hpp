#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <map>
# include <vector>
# include <iostream>
# include <fstream>
# include <sstream>
# include "ChunkReader.hpp"
# include "ContentLengthRd.hpp"
# include "Webserve.hpp"
# include "function_utils.hpp"
# include <unistd.h>
# include <limits.h>
# include <cmath>
# include "Response.hpp"
# include <string>
# include <sys/stat.h>
# include <cstdlib>
# include <stdlib.h>
# include <fcntl.h>


class Webserve;
using namespace std;
#define BUFFER_SIZE 2048

enum Fdcase {
	READ,
	WRITE,
};

class ChildProcess {
public:
	pid_t 	id;
	int cp_in;
	int cp_out;
	pid_t	rtn_wt;
	double	bgn_tm;
	int		fd;
	int		input_fd;
	string	name_in;
	string	name_out;
	int		valread;
	bool	end;
	ChildProcess();
	ChildProcess(pid_t id);
	ChildProcess(const ChildProcess& o);
	ChildProcess operator=(const ChildProcess& o);
};

class Request {
private:
	int 			socket;
	bool			vepoll;
	unsigned int	header_size;
	long long		content_length;
	long long		sz_bdy;
	bool 			fin_rd;
	bool			fin_rq;
	std::string		method;
	std::string		path;
	std::string		old_path;
	std::string		version;
	std::string		ip_address;
	std::string 	query_str;
	std::string 	post_file;
	std::string		res_stat;
	int 			port;
	std::fstream 	file;
	std::ifstream	file2;
	std::string 	body;
	size_t 			max_size;
	std::string 	first_rq;
	int 			rdfirst_rq;
	int				smart;
	int				fail_rd;
	std::map<std::string, std::string> headers;
	std::map<std::string, std::string> location;
	ContentLengthRead	cnl;
	ChunkReader 		chk;
	ChildProcess *kid_proc;
	std::map<std::string, std::string> *c_type;
	std::map<std::string, std::string> *n_file;
	std::map<std::string, std::string> error_page;

public:
	Request();
	Request(int socket);
	Request(int socket, int port);
	Request(int socket, int port, Webserve& paramserv);
	~Request();
	Request(const Request& o);
	Request& operator=(const Request& o);
	bool full_header(const std::string& request, Webserve& paramserv);
	void readFirstRequest(Webserve& paramserv);
	void unchunkBody(char* request, size_t pos, unsigned int valread);
	void contentLengthRequest(char* request, unsigned int valread);
	void getRequest();
	bool failReadOrWrite(int rdwt, int* rtn_vl);
	bool failReadOrWrite(int rdwt, bool is_fail);
	bool pathExists();
	string pathTOFile();
	void postRequest();
	void generatePostFileName();
	void firstPostRequest();
	bool parseRequest();
	bool openFile();
	void writeInFile();
	void executeRequest(Webserve& paramserv);
	void executeFirstRequest();
	int getFinRd();
	int getFinRq();
	int getVEpoll();
	void setFinRd(int b);
	void setVEpoll(bool v);
	void setResStat(string rtn_status);
	bool headerIsFull(char* buffer, int valread);
	std::map<std::string, std::string> confParams(Webserve& paramserv);
	void deleteRequest();
	void remove_directory(const char *path);
	int check_path(const char *path);
	void responseError(const string error_code);
	char** cgiEnv();
	void cgiRequest();
	void firstCgiExecute();
	void executeCgi();
	bool firstCgiResponse(char* buffer);
	bool cgiExecuteTime();
	void closeFiles();
	bool pathInRoot();
};

#endif
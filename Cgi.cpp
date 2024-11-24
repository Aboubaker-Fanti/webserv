#include "Request.hpp"
#include "Response.hpp"
#include <cstring>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>

char *variable(string s)
{
	char *v = new char[s.size() + 1];
	strcpy(v, s.c_str());
	v[s.size()] = '\0';
	return (v);
}

char **Request::cgiEnv()
{
	char **cgi_env = new char *[7];
	int i = 0;
	cgi_env[i++] = variable(string("SCRIPT_NAME=") + location["cgi rq"]);
	cgi_env[i++] = variable("REQUEST_METHOD=" + method);
	cgi_env[i++] = variable("REDIRECT_STATUS=CGI");
	cgi_env[i++] = variable("PATH_TRANSLATED=" + path);
	if (headers.count("Cookie"))
		cgi_env[i++] = variable("HTTP_COOKIE=" + headers["Cookie"]);
	if (!query_str.empty())
		cgi_env[i++] = variable("QUERY_STRING=" + query_str);
	cgi_env[i] = NULL;
	return (cgi_env);
}

void Request::firstCgiExecute()
{
	kid_proc->bgn_tm = clock();

	kid_proc->cp_out = dup(1);
	kid_proc->cp_in = dup(0);
	kid_proc->name_out = generateRandomName(socket);
	file.close();
	if ((kid_proc->id = fork()) == -1)
		return;
	if (kid_proc->id == 0)
	{
		kid_proc->fd = open(kid_proc->name_out.c_str(), O_CREAT | O_RDWR, 0644);
		kid_proc->input_fd = open(kid_proc->name_in.c_str(), O_CREAT | O_RDWR, 0644);
		(dup2(kid_proc->fd, 1), dup2(kid_proc->input_fd, 0));
		(close(kid_proc->fd), close(kid_proc->input_fd));
		char *cmd[] = {(char *)location["cgi rq"].c_str(),
					   (char *)this->path.c_str(), NULL};
		chdir("/usr/bin/");
		execve(cmd[0], cmd, cgiEnv());
		exit(127);
	}
	else
	{
		executeCgi();
	}
}

bool Request::cgiExecuteTime()
{
	clock_t currenttime = clock();
	if (kid_proc->id != -2 && ((double)currenttime - kid_proc->bgn_tm) / CLOCKS_PER_SEC > 8)
	{
		closeFiles();
		unlink(kid_proc->name_in.c_str());
		unlink(kid_proc->name_out.c_str());
		kill(kid_proc->id, SIGKILL);
		waitpid(kid_proc->id, NULL, 0);
		setResStat("504");
		kid_proc->id = -2;
		return (1);
	}
	return (0);
}

static std::streampos getFileSize(const char *filename)
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	if (!file)
		return (-1);
	return file.tellg();
}

map<string, string> cgiHeader(const std::string &str_header)
{
	map<string, string> map_header;
	vector<string> header;
	vector<string> lines = splitString(str_header, "\r\n");
	unsigned int i = 0;
	if (lines.empty() || lines.size() == 0)
		return (map_header[""] = "", map_header);
	while (i < lines.size())
	{
		header = splitString(lines[i++], ": ");
		if (header.empty())
			return (map_header[""] = "", map_header);
		map_header[header[0]] = header[1];
	}
	return (map_header);
}

static std::map<std::string, std::string> responseHeaders(const std::string &cgi_response, long long sz_body, bool c = 1)
{
	std::map<std::string, std::string> headers;
	if (c == 1)
		headers = cgiHeader(cgi_response);
	if (!headers.count("Content-Type") && !headers.count("Content-type"))
		headers["Content-Type"] = "text/html";
	if (!headers.count("Content-Length"))
		headers["Content-Length"] = int_to_string(sz_body);
	return (headers);
}

bool Request::firstCgiResponse(char *buffer)
{
	std::string str;
	str.append(buffer, kid_proc->valread);
	long long fileSize = ((long long)getFileSize(kid_proc->name_out.c_str()));
	if (fileSize == 0)
	{
		Response(socket, "200", "").writeResponse(*this);
		return (1);
	}
	else if (kid_proc->valread < 0)
		Response(socket, "500", "").writeResponse(*this);
	if (location["cgi rq"].find("python") != string::npos)
	{
		Response(socket, "200", responseHeaders("", fileSize, 0),
				 str)
			.writeResponse(*this, kid_proc->valread);
		return (1);
	}
	vector<string> header_body = splitString(str, "\r\n\r\n");
	fileSize = (fileSize - ((int)header_body[0].size() + 4));
	str.clear();
	str.append(buffer + header_body[0].size() + 4, kid_proc->valread - header_body[0].size() - 4);
	if ((unsigned long)kid_proc->valread > header_body[0].size() + 4)
		Response(socket, "200",
				 responseHeaders(header_body[0], fileSize),
				 str)
			.writeResponse(*this, kid_proc->valread - header_body[0].size() - 4);
	else
		Response(socket, "200",
				 responseHeaders(header_body[0], fileSize),
				 "")
			.writeResponse(*this, 0);
	return (1);
}

void Request::closeFiles()
{
	(dup2(kid_proc->cp_out, 1), dup2(kid_proc->cp_in, 0));
	(close(kid_proc->cp_out), close(kid_proc->cp_in));
	(close(kid_proc->fd));
}

void Request::executeCgi()
{
	int status;

	if (!kid_proc->end)
	{
		kid_proc->rtn_wt = waitpid(kid_proc->id, &status, WNOHANG);
		if (kid_proc->rtn_wt == kid_proc->id)
		{
			kid_proc->fd = open(kid_proc->name_out.c_str(), O_CREAT | O_RDWR, 0644);
			kid_proc->end = 1;
		}
	}
	if (kid_proc->end)
	{
		char buffer[BUFFER_SIZE] = {0};
		kid_proc->valread = read(kid_proc->fd, buffer, BUFFER_SIZE);
		if (kid_proc->rtn_wt == kid_proc->id)
		{
			kid_proc->rtn_wt = 0;
			if (WEXITSTATUS(status))
			{
				setResStat("500");
				closeFiles();
				unlink(kid_proc->name_in.c_str());
				unlink(kid_proc->name_out.c_str());
				return;
			}
			firstCgiResponse(buffer);
		}
		else if (kid_proc->valread > 0)
		{
			int rv = write(socket, buffer, kid_proc->valread);
			if (rv <= 0)
			{
				Response(socket, "500", "").writeResponse(*this);
				kid_proc->valread = -1;
			}
		}
		if (kid_proc->valread < BUFFER_SIZE)
		{
			closeFiles();
			unlink(kid_proc->name_in.c_str());
			unlink(kid_proc->name_out.c_str());
			fin_rq = 1;
		}
	}
}

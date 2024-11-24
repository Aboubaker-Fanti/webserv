#ifndef CONTENTLENGTHREQUEST_HPP
# define CONTENTLENGTHREQUEST_HPP

# include <string>
# include <iostream>
using namespace std;
class ContentLengthRead {
public:
	bool		start;
	char		*req;
	int			valread;
	long long 		content_length;
	long long 		*sz_bdy;
	std::string *body;
	bool		*fin_rd;
public:
	ContentLengthRead();
	ContentLengthRead(char *req, int valread, long long content_length);
	ContentLengthRead(std::string *body, bool *fin_rd);
	ContentLengthRead(std::string *body, bool *fin_rd, long long *sz_bdy);
	ContentLengthRead(const ContentLengthRead& o);
	ContentLengthRead& operator=(const ContentLengthRead& src);
	~ContentLengthRead();
	void execute();
};

#endif
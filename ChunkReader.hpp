#ifndef CHUNKREADER_HPP
# define CHUNKREADER_HPP

# include <string>
# include <iostream>
# include "function_utils.hpp"
using namespace std;
class ChunkReader {
public:
	bool		start;
	char		*req;
	size_t		valread;
	size_t		pos;
	size_t		size;
	size_t		nxt_pos;
	long long 	rest;
	int 		rest_nl;
	bool		*fin_rd;
	std::string *body;
	size_t 		*max_size;
	std::string xp_size;
	size_t 		sum_sz;
public:
	ChunkReader();
	ChunkReader(char *req, size_t valread, size_t pos);
	ChunkReader(std::string *body, bool *fin_rd);
	ChunkReader(std::string *body, bool *fin_rd, size_t *max_size);
	ChunkReader& operator=(const ChunkReader& src);
	~ChunkReader();
	void unchunkBodyStart();
	void unchunkBody();
	long long getRestChunk();
	void getSize();
	size_t getChunkSize();
	size_t getNextPos();
};

#endif
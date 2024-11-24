#include "ChunkReader.hpp"
#include <cstdlib>

ChunkReader::ChunkReader(): start(1), req(NULL), valread(0), pos(0), size(0), nxt_pos(0), rest(0), rest_nl(0), sum_sz(0){
}

ChunkReader::ChunkReader(char *req, size_t valread, size_t pos):
start(1),
req(req), valread(valread), pos(pos),
size(0), nxt_pos(0), rest(0), rest_nl(0), sum_sz(0){
}

ChunkReader::ChunkReader(std::string *body, bool *fin_rd):
start(1), req(NULL), valread(0), pos(0), size(0), nxt_pos(0), rest(0), rest_nl(0), sum_sz(0) {
	this->fin_rd = fin_rd;
	this->body = body;
}

ChunkReader::ChunkReader(std::string *body, bool *fin_rd, size_t *max_size):
start(1), req(NULL), valread(0), pos(0), size(0), nxt_pos(0), rest(0), rest_nl(0), sum_sz(0) {
	this->fin_rd = fin_rd;
	this->body = body;
	this->max_size = max_size;
}

void ChunkReader::getSize() {
	pos = 0;
	std::string str;
	str.append(req, valread);
	if (!rest_nl)
		xp_size.append(req, str.find("\n"));
	rest = strtol(xp_size.c_str(), NULL, 16);
	sum_sz += rest;
	xp_size = "";
	nxt_pos = str.find("\n") + 1;
}

ChunkReader& ChunkReader::operator=(const ChunkReader& o) {
	if (this == &o)
		return (*this);
	this->nxt_pos = 0;
	this->pos = this->rest_nl;
	this->valread = o.valread;
	if (!this->xp_size.empty()) 
		ChunkReader::getSize();
	this->rest_nl = 0;
	if (this->start)
		this->pos = o.pos;
	this->req = o.req;
	return (*this);
}

size_t ChunkReader::getNextPos() {
	if (pos == valread)
		return (std::string::npos);
	std::string str;
	str.append(req + pos, valread - pos);
	if (str.find("\r") == std::string::npos) {
		xp_size.append(req + pos, valread - pos);
		return (std::string::npos);
	}
	if (str.find("\n") == std::string::npos) {
		rest_nl = 1;
		xp_size.append(req + pos, valread - pos);
		return (std::string::npos);
	}
	size_t nxt_pos = str.find("\n");
	return ((nxt_pos == std::string::npos) ? nxt_pos : nxt_pos + pos + 1);
}

size_t ChunkReader::getChunkSize() {
	size_t sz;
	sz = strtol(req + pos, NULL, 16);
	sum_sz += sz;
	if (*max_size && (sz > *max_size || sum_sz > *max_size))
		throw std::string("request pass max body size: \"" + int_to_string(*max_size) + "\"." );
	return (sz);
}

long long ChunkReader::getRestChunk() {
	if (nxt_pos + size + 1 == valread) {
		rest_nl = 1;
		return (0);
	}
	else if (nxt_pos + size == valread)
		rest_nl = 2;
	return (size - (valread - nxt_pos));
}

void ChunkReader::unchunkBodyStart() {
	try {
		start = 0;
		nxt_pos = getNextPos();
		if (nxt_pos == std::string::npos)
			return ;
		size = getChunkSize();
		while (size != 0) {
			if (nxt_pos + size + 2 > valread) {
				if (nxt_pos < valread)
					(*body).append(req + nxt_pos, ((nxt_pos + size > valread) ? valread - nxt_pos : size));
				rest = ChunkReader::getRestChunk();
				return ;
			}
			if (nxt_pos < valread)
				(*body).append(req + nxt_pos, size);
			pos = nxt_pos + size + 2;
			nxt_pos = getNextPos();
			if (nxt_pos == std::string::npos)
				return ;
			size = getChunkSize();
		}
		*fin_rd = 1;
	}
	catch (std::string &e) {
		throw e;
	}
}

void ChunkReader::unchunkBody() {
	try {
		if (*max_size && (sum_sz > *max_size))
			throw std::string("request pass max body size: \"" +
				int_to_string(*max_size) + "\"." );
		if (rest != 0) {
			if (nxt_pos + rest > valread) {
				if (nxt_pos < valread)
					(*body).append(req + nxt_pos, valread - nxt_pos);
				rest -= valread - nxt_pos;
				return ;
			}
			if (nxt_pos < valread)
				(*body).append(req + nxt_pos, rest);
			size = rest;
			rest = 0;
			pos = nxt_pos + size + 2;
			if (pos > valread) {
				ChunkReader::getRestChunk();
				return;
			}
		}
		ChunkReader::unchunkBodyStart();
	} catch (string &e) {
		throw e;
	}
}

ChunkReader::~ChunkReader() {
}
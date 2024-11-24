#include "ContentLengthRd.hpp"

ContentLengthRead::ContentLengthRead(){};

ContentLengthRead::ContentLengthRead(char *req, int valread, long long content_length):
req(req), valread(valread), content_length(content_length){
}

ContentLengthRead::ContentLengthRead(std::string *body, bool *fin_rd):
sz_bdy(0), fin_rd(0) {
    this->body = body;
    this->fin_rd = fin_rd;
}

ContentLengthRead::ContentLengthRead(std::string *body, bool *fin_rd, long long *sz_bdy){
    this->body = body;
    this->fin_rd = fin_rd;
    this->sz_bdy = sz_bdy;
}

ContentLengthRead::ContentLengthRead(const ContentLengthRead& o) {
    this->req = o.req;
    this->valread = o.valread;
    this->content_length = o.content_length;
}

ContentLengthRead& ContentLengthRead::operator=(const ContentLengthRead& o) {
    this->req = o.req;
    this->valread = o.valread;
    this->content_length = o.content_length;
    return (*this);
}

ContentLengthRead::~ContentLengthRead(){}

void ContentLengthRead::execute() {
    if ((*sz_bdy) + valread < content_length)
        (*body).append(req, valread);
    else {
        (*body).append(req, content_length - (*sz_bdy));
        *fin_rd = 1;
    }
    (*sz_bdy) += valread;
}
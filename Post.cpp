#include "Request.hpp"

std::string getFileName(std::string bgn, std::string extantion) {
    struct stat s;
    int i = 0;
    std::string name = bgn + int_to_string(i) + extantion;
    while (stat(name.c_str(), &s) == 0)
        name = bgn + "(" + int_to_string(++i) + ")"  + extantion;
    return (name);
}

void Request::generatePostFileName() {
    std::string extantion;
    if (headers.count("Content-Type") && (*n_file).count(headers["Content-Type"]))
        post_file = (*n_file)[headers["Content-Type"]];
    else
        post_file = (*n_file)[""];
    extantion = "." + post_file.substr(post_file.find_last_of('_') + 1);
    post_file = location["root"] + "/" + location["upload_pass"] + "/" + post_file;    
    post_file = getFileName(post_file, extantion);
}

void Request::writeInFile() {
	if (method == "POST") {
		file << body;
		if (failReadOrWrite(WRITE , file.fail()))
			return ;
	}
	body.clear();
	if (fin_rd && !location.count("cgi rq") && method == "POST")
		setResStat("201");
}

void Request::firstPostRequest() {
	writeInFile();		
}

void Request::postRequest() {
	if (fin_rd && !location.count("cgi rq") && method == "POST") {
		setResStat("201");
		return;
	}
	try {
		char buffer[BUFFER_SIZE] = {0};
		int valread = 0;
		valread = read(socket, buffer, BUFFER_SIZE);
		if (failReadOrWrite(READ, &valread))
			return ;
		(content_length > 0) ?
		contentLengthRequest(buffer, valread) :
		unchunkBody(buffer, 0, valread);
		writeInFile();
	}
	catch (std::string &e) {
		unlink(post_file.c_str());
		setResStat("413");
		fin_rd = 1;
	}
}

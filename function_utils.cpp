#include "function_utils.hpp"

std::vector<std::string> splitString(const std::string& str, const std::string delim) {
	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	std::string token;

	if (str.empty()) {
		tokens.push_back("");
		return (tokens);
	}
	while (pos < str.length() && prev < str.length()) {
		if ((pos = str.find(delim, prev)) == std::string::npos)
			pos = str.length();
		token = str.substr(prev, pos - prev);
		if (!token.empty())
			tokens.push_back(token);
		prev = pos + delim.length();
	}
	return (tokens);
}

std::string int_to_string(int num) {
	std::string str;
	std::stringstream ss;
	ss << num;
	ss >> str;
	return (str);
}

std::string int_to_string(long long num) {
	std::string str;
	std::stringstream ss;
	ss << num;
	ss >> str;
	return (str);
}

std::string int_to_string(size_t num) {
	std::string str;
	std::stringstream ss;
	ss << num;
	ss >> str;
	return (str);
}

long long strToLong(std::string s) {
	long long nb;
	std::stringstream ss;
	ss << s;
	ss >> nb;
	return (nb);
}

size_t findEncodingChar(std::string& s, size_t pos) {
    size_t new_pos = s.find('%', pos);
    if (new_pos == string::npos || new_pos + 2 >= s.length())
        return (string::npos);
    if (isdigit(s[new_pos + 1]) && isdigit(s[new_pos + 2]))
        return (new_pos);
    return (findEncodingChar(s, new_pos + 1));
}

string encodingChar(std::string &s, size_t pos) {
    char c = strtol(s.c_str() + pos + 1, NULL, 16);
    string ns(&c, 1);
    return (ns);
}

void replaceEncodingCharacter(std::string& s, size_t pos) {
    size_t new_pos = findEncodingChar(s, pos);
    if (new_pos == string::npos)
        return ;
    replaceEncodingCharacter(s.replace(new_pos, 3, encodingChar(s, new_pos)), new_pos + 1);
}

string generateRandomName(int socket) {
	int t = time(NULL);
	string s = int_to_string(t);
    s = "." + int_to_string(socket) + s;
	size_t i = 1;
	while (i < s.size()) {
		if (s[i] % 2 != 1)
			s[i] = s[i] + 18;
        i++;
    }
	s = "/tmp/" + s;
	return (s);
}

string generateRandomInputName(int socket) {
	int t = time(NULL);
	string s = int_to_string(t);
    s = "." + int_to_string(socket) + s;
	size_t i = 1;
	while (i < s.size()) {
		if (s[i] % 2 != 1)
			s[i] = s[i] + 52;
        i++;
    }
	s = "/tmp/" + s;
	return (s);
}


string hasIndexFile(const std::string& directoryPath) {
    DIR* dir = opendir(directoryPath.c_str());
    if (dir == NULL)
        return ("");

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
		string file_name = string(entry->d_name);
		size_t pos = (file_name.size() < 6) ? file_name.size() : 6;
		if (file_name.substr(0, pos) == "index.")
		{
            closedir(dir);
            return (file_name);
        }
    }
    closedir(dir);
    return ("");
}

#ifndef FUNCTION_UTILS_HPP
# define FUNCTION_UTILS_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <sstream>
# include <cstdlib>
# include <dirent.h>

using namespace std;

std::vector<std::string> splitString(const std::string& str, const std::string delim);
std::string int_to_string(int num);
std::string int_to_string(size_t num);
std::string int_to_string(long long num);
void replaceEncodingCharacter(std::string& s, size_t pos);
string generateRandomInputName(int socket);
string generateRandomName(int socket);
string hasIndexFile(const std::string& directoryPath);
long long strToLong(std::string s);

#endif
#include "Location.hpp"

/*      *************** Costructor && Destructor ***************        */

Location::Location() {
    this->_autoindex = "off";
    this->_cgi = "off";
    this->_upload_pass_args = "off";
    this->_upload_pass = "/upload_dir";
    auto_deplicate = 0;
    cgi_deplicate = 0;
    upload_deplicate = 0;
    upload_dep = 0;
}

Location::~Location() {
}

/*      *************** Atripute setter ***************     */

void Location::set_path(std::string path, int i) {
    if (i != 6 || !_path.empty())
        throw std::range_error("config file not valide please follow rules");

    if (path[path.size() - 1] != '/')
        path += "/";
    if (path[0] != '/')
        _path = "/";
    _path += path;
}

void Location::set_root(std::string root, int i) {
    if (i != 8 || !_root.empty())
        throw std::range_error("fconfig file not valide please follow rules");
    if (!fileExists(root) || access(root.c_str(), W_OK | R_OK ) != 0)
        throw std::out_of_range("file root not exist check it");

    this->_root = root;
}

void Location::set_autoindex(std::string autoindex, int i) {
    if (i != 8 || auto_deplicate != 0)
        throw std::range_error("config file not valide please follow rules");

    this->_autoindex = autoindex;
    auto_deplicate++;
}

void Location::set_allow_methods(std::string allow_methods, int i) {
    std::string input;

    if (i != 8 || !_allow_methods.empty())
        throw std::range_error("config file not valide please follow rules");
    std::istringstream ss(allow_methods.c_str());
    std::string name;
    while (getline(ss, name, ',')) {
        i = skip_space_tap(name);
        int f = skip_space_end(name);
        input = name.substr(i, (f + 1) - i);
        if (input != "GET" && input != "DELETE" && input != "POST")
            throw std::range_error("config file not valide please follow rules 'METHODS'.");
    }
    this->_allow_methods = allow_methods;
}

void Location::set_index_home(std::string index_home, int i) {
    if (i != 8 || !_index_home.empty())
        throw std::range_error("config file not valide please follow rules");
    this->_index_home = index_home;
}

void Location::set_return_index(std::string return_index, int i) {
    if (i != 8 || !_return_index.empty())
        throw std::range_error("config file not valide please follow rules");
    this->_return_index = return_index;
}

void Location::set_cgi_ext(std::string cgi, int i) {
    if (i != 8 || (cgi != "off" && cgi != "on") || cgi_deplicate != 0)
        throw std::range_error("config file not valide please follow rules");

    this->_cgi = cgi;
    cgi_deplicate++;
}

void Location::set_cgi(std::string cgi_ext, int i)
{
    if (i != 8 || !_cgi_ext.empty())
        throw std::range_error("config file not valide please follow rules");
    i = 0;
    std::string input;
    std::string last;
    std::istringstream ss(cgi_ext.c_str());
    std::string name;
    while (getline(ss, name, ',')) {
        std::istringstream s(name.c_str());
        while (getline(s, name, ' ')){
            int t = skip_space_tap(name);

            int f = skip_space_end(name);
            input = name.substr(t, (f + 1) - t);
            if (name.empty())
                throw std::range_error("sorry we handel just .php && .py");

            if (i == 0) {
                last = input;
                if (input != ".py" && input != ".php") {
                    throw std::range_error("sorry we handel just .php && .py");
                }
            }
            else if (i == 1) {
                if (last == ".py") {
                    if (input != "/usr/bin/python3")
                        throw std::range_error("yout executable path not valid");
                }
                else if (last == ".php")
                    if (input != "/usr/bin/php-cgi")
                        throw std::range_error("yout executable path not valid");
                if (!isExecutableValid(input.c_str()))
                    throw std::range_error("yout executable path not valid");
            }
            if (i > 1)
                throw std::range_error("error in yout cgi variable check it");
            i++;
        }
        i = 0;
    }
    this->_cgi_ext = cgi_ext;
}

void Location::set_upload_pass(std::string upload_pass, int i) {
    if (i != 8 || upload_dep != 0)
       throw std::range_error("error in your upload pass check it?");
    _upload_pass = upload_pass;
    upload_dep++;
}

void Location::set_upload_pass_args(std::string upload_pass_args, int i) {

    if (i != 8 || (upload_pass_args != "off" && upload_pass_args != "on") || upload_deplicate != 0)
        throw std::range_error("error in your upload pass args check it?");
    _upload_pass_args = upload_pass_args;
    upload_deplicate++;
}

/*          *************** Atripute getter ***************           */

std::string Location::get_path() const {
    return (_path);
}

std::string Location::get_root() const {
    return (_root);
}

std::string Location::get_autoindex() const {
    return (_autoindex);
}

std::string Location::get_allow_methods() const {
    return (_allow_methods);
}

std::string Location::get_index_home() const {
    return (_index_home);
}

std::string Location::get_return_index() const {
    return (_return_index);
}

std::string Location::get_cgi() const {
    return (_cgi);
}

std::string Location::get_set_cgi() const {
    return (_cgi_ext);
}

std::string Location::upload_on_off() const {
    return (_upload_pass_args);
}

std::string Location::upload_pas() const {
    return (_upload_pass);
}

/*          *************** some mumber function ***************           */

bool Location::fileExists(const std::string &filename) {
    const char *dir = filename.c_str();
    struct stat sb;
    if (stat(dir, &sb) == 0 && (sb.st_mode & S_IFDIR))
        return (true);
    return (false);
}

int Location::skip_space_tap(std::string str) {
    for (int i = 0; str[i]; i++) {
        if (str[i] != ' ')
            return (i);
        if (str[i] == '\t') {
            throw std::range_error("there is invalid server name?");
        }
    }
    return (0);
}

int Location::skip_space_end(std::string str) {
    for (int i = str.size() - 1; str[i]; i--) {
        if (str[i] != ' ' && str[i] != '\t')
            return (i);
    }
    return (0);
}

bool Location::isExecutableValid(const char *path) {
    if (access(path, X_OK) != 0){
        return false;
    }
    return true;
}

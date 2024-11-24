#include "Server.hpp"

/**************** Costructor && Destructor ****************/

Server::Server(){
        this->error_404_page = "/error/404.html";
        this->error_403_page = "/error/403.html";
        this->error_400_page = "/error/400.html";
        this->error_402_page = "/error/402.html";
        this->error_401_page = "/error/401.html";
        this->error_413_page = "/error/413.html";
        this->error_501_page = "/error/501.html";
        this->error_505_page = "/error/505.html";
        this->error_204_page = "/error/204.html";
        this->error_412_page = "/error/412.html";
        this->error_405_page = "/error/405.html";
        this->error_500_page = "/error/500.html";
        this->error_502_page = "/error/502.html";
}

Server::~Server(){
}

/**************** Atripute setter ****************/

void Server::set_location_atripute(int index ,int nbr, std::string input, int space){
    
    switch (nbr){
        case 1:
            _locations[index - 1].set_root(input, space);
            break;
        case 2:
            _locations[index - 1].set_autoindex(input, space);
            break;
        case 3:
           _locations[index - 1].set_allow_methods(input, space);
            break;
        case 4:
            _locations[index - 1].set_index_home(input, space);
            break;
        case 5:
            _locations[index - 1].set_return_index(input, space);
            break;
        case 6:
            _locations[index - 1].set_cgi(input, space);
            break;
        case 7:
            _locations[index - 1].set_cgi_ext(input, space);
            break;
        case 8:
           _locations[index - 1].set_path(input, space);
            break;
        case 9:
           _locations[index - 1].set_upload_pass(input, space);
            break;
        case 10:
           _locations[index - 1].set_upload_pass_args(input, space);
            break;

    }
}

void Server::set_server_name(std::string server_name, int space){
    if (space != 4 || !_server_name.empty())
        throw std::range_error("handel space in your config file or deplicate;");
    this->_server_name = server_name;
}

void Server::set_port(std::string port, int space){
    if (space != 4 || !_port.empty())
        throw std::range_error("handel space in your config file or deplicate;");
    for(int i = 0; port[i]; i++){
        if (!isdigit(port[i]))
            throw std::range_error("error in your port number");
    }
    std::istringstream ss(port);
    int nbr;
    ss >> nbr;
    if (nbr < 0 || nbr > 65535){
        throw std::range_error("error in your port number");
    }
    this->_port = port;
}

void Server::set_host(std::string host, int space){
    if (space != 4 || !_host.empty())
        throw std::range_error("handel space in your config file or deplicate;");
    int d = 0;
    if (host[0] >= 48 && host[0] <= 57){
        for(int i = 0; host[i]; i++){
            if (host[i] == '.')
                d++;
            
            if ((!isdigit(host[i]) && host[i] != '.') || (host[i] == '.' && !host[i + 1])){
                throw std::range_error("you should use program name with config file");
            }
        }
        std::string  check;
        std::istringstream input(host);
        while (getline(input, check, '.')){
            if (atoi(check.c_str()) < 0 || atoi(check.c_str()) > 255 || d != 3)
                 throw std::range_error("you should use program name with config file");
        }
        this->_host = host;
    }
    else if (host[0] >= 97 && host[0] <= 122){
        if (host != "localhost"){
            throw std::range_error("you should use program name with config file");
        }
        this->_host = host;
    }
    else
        throw std::range_error("you should use program name with config file44.");
}

void Server::set_client_max_size(std::string client_max_size, int space){
    if (space != 4 || !_client_max_size.empty())
        throw std::range_error("handel space in your config file or deplicate;");
    std::stringstream ss(client_max_size);
    long long nbr;
    ss >> nbr;
    if (nbr <= 0 || nbr > 922337203685477580)
        throw std::range_error("error check your client max body size;");
    this->_client_max_size = client_max_size;
}

void Server::set_root(std::string root, int space){
    if (space != 4 || !_root.empty())
        throw std::range_error("handel space in your config file or deplicate;");
     if (!fileExists(root) || (access(root.c_str(), W_OK | R_OK ) != 0))
        throw std::out_of_range("file root not exist check it");
    this->_root = root;
}

void Server::set_index_home(std::string index_home, int space){
    if (space != 4 || !_index_home.empty())
        throw std::range_error("handel space in your config file or there is deplicate;");
    this->_index_home = index_home;
}

void    Server::set_location(size_t nbr){
    for (size_t i = 0; i < nbr; i++){
        _locations.push_back(Location());
    }
}

void    Server::set_location_nbr(size_t t){
    nbr_of_location = t;
}

/*  *************** Atripute getter error page ***************  */



std::string     Server::get_404_page(){
    return(error_404_page);
}

std::string     Server::get_402_page(){
    return(error_402_page);
}

std::string     Server::get_403_page(){
    return(error_403_page);
}

std::string     Server::get_400_page(){
    return(error_400_page);
}

std::string     Server::get_401_page(){
    return(error_401_page);
}

std::string     Server::get_413_page(){
    return(error_413_page);
}

std::string     Server::get_501_page(){
    return(error_501_page);
}

std::string     Server::get_505_page(){
    return(error_505_page);
}

std::string     Server::get_204_page(){
    return(error_204_page);
}

std::string     Server::get_412_page(){
    return(error_412_page);
}

std::string     Server::get_405_page(){
    return(error_405_page);
}
std::string     Server::get_500_page(){
    return(error_500_page);
}
std::string     Server::get_502_page(){
    return(error_502_page);
}

/*  *************** Atripute getter ***************  */

std::string Server::get_server_name() const{
    return (_server_name);
}

std::string Server::get_port() const{
    return (_port);
}

std::string Server::get_host() const{
    return (_host);
}

std::string Server::get_client_max_size() const{
    return (_client_max_size);
}

std::string Server::get_root() const{
    return (_root);
}

std::string Server::get_index_home() const{
    return (_index_home);
}

std::string Server::get_from_location_class(std::string input, int index){
    if (input == "path"){
        std::string name = _locations[index].get_path();
        size_t found = name.find_last_of('.');
        if (found != std::string::npos)
            return (name);
        if (name[name.size() - 1] != '/')
            name += "/";
        return(name);
    }
    if (input == "root")
        return(_locations[index].get_root());
    if (input == "autoindex")
        return(_locations[index].get_autoindex());
    if (input == "allow_methods")
        return(_locations[index].get_allow_methods());
    if (input == "index_home")
        return(_locations[index].get_index_home());
    if (input == "return_index")
        return(_locations[index].get_return_index());
    if (input == "cgi")
        return(_locations[index].get_cgi());
    if (input == "cgi_ext")
        return(_locations[index].get_set_cgi());
    if (input == "upload_pass")
        return(_locations[index].upload_pas());
    if (input == "upload_pass_args")
        return(_locations[index].upload_on_off());
    return (NULL);
}

size_t    Server::get_location_nbr(){
    return (nbr_of_location);
}

/*  *************** Atripute setter error page ***************  */

void    Server::set_404_page(std::string error, int space){
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    error_404_page = error;
}

void    Server::set_403_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");

    error_403_page = error;
}

void    Server::set_402_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_402_page = error;
}

void    Server::set_400_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_400_page = error;
}

void    Server::set_401_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_401_page = error;
}

void    Server::set_413_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_413_page = error;
}

void    Server::set_501_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_501_page = error;
}

void    Server::set_502_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_502_page = error;
}
void    Server::set_500_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_500_page = error;
}

void    Server::set_505_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_505_page = error;
}

void    Server::set_204_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_204_page = error;
}

void    Server::set_412_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_412_page = error;
}

void    Server::set_405_page(std::string error, int space){
    if (space != 4)
        throw std::out_of_range("error follow the ruls.");
    if (error.substr(error.find_last_of('.'))!= ".html")
        throw std::out_of_range("error follow the ruls .html in error page.");
    error_405_page = error;
}

/*  *************** some mumber function ***************  */

bool Server::fileExists(const std::string& filename) {
    const char *dir = filename.c_str();
    struct stat sb;
    if (stat(dir, &sb) == 0 && (sb.st_mode & S_IFDIR))
        return (true);
    return (false);
}

int check_file_exis(std::string path){
    const char* dir = path.c_str();
    struct stat sb;
    if (stat(dir, &sb) == 0 && (sb.st_mode & S_IFREG)){
        if (access(path.c_str(), W_OK | R_OK ) != 0)
            return (-1);
        return (1);
    }
    else if (stat(dir, &sb) == 0  && (sb.st_mode & S_IFDIR)){
        if (access(path.c_str(), W_OK | R_OK ) != 0)
            return (-1);
        return (2);
    }
    return (-1);
}

void Server::check_index_valid(){
    for (size_t i = 0; i < nbr_of_location; i++){
        if (!_locations[i].get_index_home().empty())
            if (check_file_exis(_locations[i].get_root() + "/" + _locations[i].get_index_home()) != 1)
                throw std::range_error("config file not valide index or root not valid");
        if (!_locations[i].upload_pas().empty())
            if (check_file_exis(_locations[i].get_root() + "/" + _locations[i].upload_pas()) != 2)
                throw std::range_error("config file not valide your upload path not valid");
        if (!_locations[i].upload_on_off().empty())
            if (_locations[i].upload_on_off() != "off" && _locations[i].upload_on_off() != "on")
                throw std::range_error("config file not valide");
        if (!_locations[i].get_cgi().empty())
            if (_locations[i].get_cgi() != "off" && _locations[i].get_cgi() != "on")
                throw std::range_error("config file not valide");
        if (!_locations[i].get_autoindex().empty())
            if (_locations[i].get_autoindex() != "off" && _locations[i].get_autoindex() != "on")
                throw std::range_error("config file not valide");
    }
}

void    Server::set_error_in_map(){

    error_page["404"] = get_404_page();
    error_page["402"] = get_402_page();
    error_page["403"] = get_403_page();
    error_page["400"] = get_400_page();
    error_page["401"] = get_401_page();
    error_page["413"] = get_413_page();
    error_page["501"] = get_501_page();
    error_page["505"] = get_505_page();
    error_page["204"] = get_204_page();
    error_page["412"] = get_412_page();
    error_page["405"] = get_405_page();
    error_page["500"] = get_500_page();
    error_page["502"] = get_502_page();
    error_page["root"] = get_root();
}

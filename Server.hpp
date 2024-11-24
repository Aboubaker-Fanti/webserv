#ifndef SERVER_HPP
# define SERVER_HPP

#include <iostream>
#include <deque>
#include "Location.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>
#include <map>
#include <stdlib.h>
#include <sys/stat.h>

class Location;

class Server {
    private:
        size_t                  nbr_of_location;
        std::string             _server_name;
        std::string             _port;
        std::string             _host;
        std::string             error_404_page;
        std::string             error_403_page;
        std::string             error_400_page;
        std::string             error_402_page;
        std::string             error_401_page;
        std::string             error_413_page;
        std::string             error_501_page;
        std::string             error_505_page;
        std::string             error_204_page;
        std::string             error_412_page;
        std::string             error_405_page;
        std::string             error_500_page;
        std::string             error_502_page;
        std::string             _client_max_size;
        std::string             _root;
        std::string             _index_home;

    public:
        Server();
        ~Server();
        std::vector<Location>   _locations;

        std::map<std::string, std::string> error_page;

    //setter for atripute
        void    set_404_page(std::string error, int space);
        void    set_403_page(std::string error, int space);
        void    set_402_page(std::string error, int space);
        void    set_400_page(std::string error, int space);
        void    set_401_page(std::string error, int space);
        void    set_413_page(std::string error, int space);
        void    set_501_page(std::string error, int space);
        void    set_505_page(std::string error, int space);
        void    set_204_page(std::string error, int space);
        void    set_412_page(std::string error, int space);
        void    set_405_page(std::string error, int space);
        void    set_500_page(std::string error, int space);
        void    set_502_page(std::string error, int space);
        void    set_location_nbr(size_t t);
        void    set_location(size_t nbr);
        void    set_server_name(std::string server_name, int space);
        void    set_port(std::string port, int space);
        void    set_host(std::string host, int space);
        void    set_client_max_size(std::string client_max_size, int space);
        void    set_root(std::string root, int space);
        void    set_index_home(std::string index_home, int space);
        void    set_time_out(std::string time_out, int space);
        void    set_location_atripute(int index ,int nbr, std::string input, int space);
        void    set_error_in_map();
    //getter for atripute
        std::string     get_404_page();
        std::string     get_402_page();
        std::string     get_403_page();
        std::string     get_400_page();
        std::string     get_401_page();
        std::string     get_413_page();
        std::string     get_501_page();
        std::string     get_505_page();
        std::string     get_204_page();
        std::string     get_412_page();
        std::string     get_405_page();
        std::string     get_500_page();
        std::string     get_502_page();
        std::string     get_server_name() const;
        std::string     get_port() const;
        std::string     get_host() const;
        std::string     get_error_page() const;
        std::string     get_client_max_size() const;
        std::string     get_root() const;
        std::string     get_index_home() const;
        std::string     get_time_out() const;
        std::string     get_from_location_class(std::string input, int index);
        bool            fileExists(const std::string& filename);
        void            check_index_valid();
        size_t          get_location_nbr();

};

#endif
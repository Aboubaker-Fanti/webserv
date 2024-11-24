#ifndef WEBSERVE_HPP
# define WEBSERVE_HPP

#include <iostream>
#include <map>
#include <algorithm>
#include <deque>
#include "Server.hpp"
#include "Location.hpp"
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include "Request.hpp"


class Webserve{
    private:
        size_t                      nbr_of_serv;
        size_t                      server_nbr;
        size_t                      location_nbr;
        std::string                 buffur;
        std::vector<Server>         servers;
        std::vector<std::string>    server_n_osama;

        Webserve(const Webserve& other);
        Webserve& operator=(const Webserve& other);

    public:
        Webserve();
        Webserve(std::ifstream& input);
        ~Webserve();

        std::map<std::string, std::string>  n_file;
        std::map<std::string, std::string>  c_type;
        std::vector<std::string>            server_host_osama;
        std::vector<std::string>            server_id_osama;
        std::vector<Server>                 getServers();
        char                                **env_serv;
        int                                 fanti;
        int                                 skip_space_tap(std::string str);
        int                                 skip_space_end(std::string str);
        void                                check_configFile_error(std::string str);
        void                                check_file_input(std::string str);
        void                                fill_atripute(std::ifstream& input);
        void                                fill_content_type();
        void                                names_file();
        int                                 find_server_using_port_host(int port, std::string host);
        size_t                              get_servers_nbr();
        void                                fill_vector_name_id();
        void                                check_otheres_error();
        int                                 find_server_using_port(int port);
        void                                check_error_pages();
        void                                check_return_isvalid();
        int                                 check_if_full(std::string input);
        std::string                         from_location(std::string path, int port_nbr, std::string u_need);
        std::map<std::string, std::string>  map_location(std::string , int);
        std::map<std::string, std::string>  map_location(std::string path, int port, std::string host);
        std::map<std::string, std::string>  return_location(std::string path, int server_n, int port_nbr);
void check_root_with_error();
}; 

#endif

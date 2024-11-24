#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
class Location {
    private:
        std::string     _path;
        std::string     _root;
        std::string     _autoindex;
        std::string     _allow_methods;
        std::string     _index_home;
        std::string     _return_index;
        std::string     _upload_pass;
        std::string     _upload_pass_args;
        std::string     _cgi;
        std::string     _cgi_ext;
        int             auto_deplicate;
        int             cgi_deplicate;
        int             upload_deplicate;
        int             upload_dep;
    public:
        Location();
        ~Location();

    //setter for atripute
        void        set_path(std::string path, int i);
        void        set_root(std::string root, int i);
        void        set_autoindex(std::string autoindex, int i);
        void        set_allow_methods(std::string allow_methods, int i);
        void        set_index_home(std::string index_home, int i);
        void        set_return_index(std::string return_index, int i);
        void        set_cgi_ext(std::string cgi_ext, int i);
        void        set_cgi(std::string cgi, int i);
        void        set_upload_pass(std::string upload_pass, int i);
        void        set_upload_pass_args(std::string upload_pass_args, int i);

    //getter for atripute
        std::string get_path() const;
        std::string get_root() const;
        std::string get_autoindex() const;
        std::string get_allow_methods() const;
        std::string get_index_home() const;
        std::string get_return_index() const;
        std::string get_cgi() const;
        std::string get_set_cgi() const;
        std::string upload_pas() const;
        std::string upload_on_off() const;

        int         skip_space_end(std::string str);
        int         skip_space_tap(std::string str);
        bool        fileExists(const std::string& filename);
        bool        isExecutableValid(const char *path);
};

#endif
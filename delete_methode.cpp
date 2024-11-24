#include <iostream>
#include <sys/stat.h>
#include <iostream>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include "Request.hpp"
#include "Response.hpp"


int Request::check_path(const char *path)
{
    struct stat s;
    if (stat(path, &s) == 0)
    {
        if (s.st_mode & S_IFDIR) {
            if (access(path, W_OK) == -1)
                return(setResStat("403"), 0);
            else
                return 1;
        }
        else if (s.st_mode & S_IFREG)
        {
            if (access(path, X_OK) == 0)
                return(setResStat("403"), 0);
            else if (access(path, W_OK) == -1)
                return(setResStat("403"), 0);
            else
                return 2;
        }
        else
            return(setResStat("403"), 0);
    }
    else
        return (setResStat("404"), 0);
}

void Request::remove_directory(const char *path)
{
    DIR *dirp = opendir(path);
    if (!dirp) {
        setResStat("400");
        return ;
    }

    struct dirent *dent;
    while ((dent = readdir(dirp)) != NULL)
    {
        if (strcmp(dent->d_name, ".") != 0 && strcmp(dent->d_name, "..") != 0)
        {
            std::string full_path = std::string(path) + "/" + dent->d_name;
            if (dent->d_type == DT_DIR)
                remove_directory(full_path.c_str());
            else
            {
                if (unlink(full_path.c_str()) == -1) {
                    setResStat("400");
                    return ;
                }
            }
        }
    }
    closedir(dirp);
    if (remove(path) == -1) {
        setResStat("400");
        return ;
    }
}


void Request::deleteRequest() 
{
    int i = check_path(path.c_str());

    if (i == 2)
    {
        if (unlink(path.c_str()) == -1)
            setResStat("400");
        Response(socket, "204", "No Content").writeResponse(*this);
    }
    else if (i == 1)
    {
        remove_directory(path.c_str());
        Response(socket, "204", "No Content").writeResponse(*this);
    }
    return ;
}
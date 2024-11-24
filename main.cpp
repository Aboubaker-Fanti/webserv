
#include <iostream>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <map>
#include "Webserve.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include <signal.h>
#include <deque>
#include <arpa/inet.h>
#define MAX_EVENTS 100
#define TIME_OUT 1000
#define MAX_CLIENTS 100

int check_is_client(int fd,std::deque<int> serverFd)
{
    size_t i = 0;
    while (i < serverFd.size())
    {
        if (fd == serverFd[i])
        {
            return 0;
        }
        i++;
    }
    return 1;
}

bool timeOut(double client_bgn) {
    double current_time = clock();
    return (((current_time - client_bgn) / CLOCKS_PER_SEC) > 10);
}



void run_server(Webserve& paramserv)
{
    std::map<int, int> server_map;
    std::map<int, int> client_port;
    std::map<int, double> client_bgn;
    std::map<int, Request *> requests;
    std::deque<int> serverFd;
    int epollFd;
    struct sockaddr_in serverAddress;
    struct epoll_event event, events[MAX_EVENTS];
    size_t i = 0;

    while (i < paramserv.server_id_osama.size())
    {
        int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1)
            return ;
        serverFd.push_back(fd);
        server_map.insert(std::make_pair(fd,atoi(paramserv.server_id_osama[i].c_str()))); // maps server_id and port
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(paramserv.server_host_osama[i].c_str());
        serverAddress.sin_port = htons(atoi(paramserv.server_id_osama[i].c_str()));
        int opt = 1;
        if (setsockopt(serverFd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
        {
            perror("Setsockopt failed");
            exit(EXIT_FAILURE);
        }
        if (bind(serverFd[i], (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1)
        {
            close(serverFd[i]);
            return ;
        }
        if (listen(serverFd[i], MAX_CLIENTS) == -1)
        {
            close(serverFd[i]);
            return ;
        }
        i++;
    }
    epollFd = epoll_create(1);
    if (epollFd == -1)
    {
        for (size_t j = 0; j < i; j++)
        {
            close(serverFd[j]);
        }
        return ;
    }
    i = 0;
    while (i < paramserv.server_id_osama.size())
    {
        event.events = EPOLLIN | EPOLLOUT;
        event.data.fd = serverFd[i];
        if (epoll_ctl(epollFd, EPOLL_CTL_ADD, serverFd[i], &event) == -1)
        {
            for (size_t j = 0; j < paramserv.server_id_osama.size(); j++)
                close(serverFd[j]);
            close(epollFd);
            return ;
        }
        i++;
    }
    std::map<int,int> client_map;
    std::cout << "===> the server is start <=== \n";
    while (1)
    {
        int numEvents = epoll_wait(epollFd, events, MAX_EVENTS, TIME_OUT);
        signal(SIGPIPE, SIG_IGN);
        if (numEvents == -1)
            break;
        else if (numEvents == 0)
            continue;
        for (int i = 0; i < numEvents; ++i)
        {
            if (find(serverFd.begin(),serverFd.end(),events[i].data.fd) != serverFd.end())
            {
                struct sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientFd = accept(events[i].data.fd, (struct sockaddr*)&clientAddress, &clientAddressLength);
                if (clientFd == -1)
                    continue;
                event.data.fd = clientFd;
                event.events = EPOLLIN | EPOLLOUT;
                if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1)
                {
                    close(clientFd);
                    continue;
                }
                client_port[clientFd] = server_map[events[i].data.fd];
                client_bgn[clientFd] = clock();
            }
            else if ((events[i].events & EPOLLIN) && check_is_client(events[i].data.fd,serverFd))
            {
                int clientFd = events[i].data.fd;
                client_bgn[clientFd] = clock();
                if (!requests.count(clientFd) && client_port[clientFd])
                {
                    requests[clientFd] = new Request(clientFd, client_port[clientFd], paramserv);
                    requests[clientFd]->readFirstRequest(paramserv);
                }
                else if (requests.count(clientFd) && !requests[clientFd]->getFinRd())
                    requests[clientFd]->executeRequest(paramserv);
                if (requests.count(clientFd) && requests[clientFd]->getFinRd()) {
                    events[i].events = EPOLLOUT;
                    if (epoll_ctl(epollFd, EPOLL_CTL_MOD, clientFd, &events[i]) == -1)
                        close(clientFd);
                }
            }
            else if (check_is_client(events[i].data.fd,serverFd) && (events[i].events & EPOLLOUT))
            {                    
                int clientFd = events[i].data.fd;
                if (requests.count(clientFd) && requests[clientFd]->getFinRd())
                    requests[clientFd]->executeRequest(paramserv);
                if (requests.count(clientFd))
                    requests[clientFd]->cgiExecuteTime();
                bool time_out = timeOut(client_bgn[clientFd]);
                if (requests.count(clientFd) && ((requests[clientFd]->getFinRq()) || (!requests[clientFd]->getFinRd() && time_out))) 
                {
                    if (time_out) Response(clientFd, "500", "").writeResponse();
                    delete (requests[clientFd]);
                    requests.erase(clientFd);
                    client_port.erase(clientFd);
                    client_bgn.erase(clientFd);
                    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &event) == -1) 
                        perror("epoll_ctl");
                    close(clientFd);
                }
                else if (!requests.count(clientFd) && time_out)
                {
                    Response(clientFd, "500", "").writeResponse();
                    client_port.erase(clientFd);
                    client_bgn.erase(clientFd);
                    if (epoll_ctl(epollFd, EPOLL_CTL_DEL, clientFd, &event) == -1) 
                        perror("epoll_ctl");
                    close(clientFd);
                }
            }
        }
    }
    for (size_t i = 0;i < serverFd.size();i++)
        close(serverFd[i]);
    close(epollFd);
    return ;
}

int main(int ac, char** av){

    try
    {
        if (ac != 2)
            throw std::range_error("you should use program name with config file1.");
        std::string name(av[1]);
        if ((name.find_last_of('.')) == std::string::npos || name.substr(name.find_last_of('.')) != ".yaml")
            throw std::range_error("you should use program name with config file1.");
        std::ifstream inputfile(av[1]);
        if (!inputfile.is_open())
            throw std::range_error("your input file does't exist.");
        Webserve objs(inputfile);
        run_server(objs);
    }catch(std::exception &e){
        std::cout << e.what() << std::endl;
    }

    return (0);
}


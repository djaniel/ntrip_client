#ifndef NTRIP_CLIENT_H
#define NTRIP_CLIENT_H

#include <iostream>
#include <chrono>
#include <thread>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <thread>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <cstddef>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

#include <base64.h>

#define BUFFER_LENGTH 1024

class NtripClient{

    std::string url_, mount_point_, agent_, user_, gpgga_;
    int port_ {}, sock_ {};
    bool operating_ {};
    boost::function<void(unsigned char*, ssize_t)> callback_;
    struct sockaddr_in serv_addr_;

    std::thread *t_queryServer_ {};
public:
    NtripClient(std::string agent_name,
                std::string url,
                int port,
                std::string mount_point,
                std::string user,
                std::string password,
                boost::function<void(unsigned char*, ssize_t)> const &callback );

    NtripClient(std::string agent_name,
                std::string url,
                int port,
                std::string mount_point,
                boost::function<void(unsigned char*, ssize_t)> const &callback );

    ~NtripClient();

    enum event{
        CONNECTED = 0
    };

    std::string getMountPointString();
    void setPosition(std::string gpgga);
    void start();
    void t_queryServer();
};

#endif // NTRIP_CLIENT_H

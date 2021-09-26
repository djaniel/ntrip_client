#ifndef NTRIP_CLIENT_H
#define NTRIP_CLIENT_H

#include <iostream>
#include <thread>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <sstream>
#include <thread>
//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Options.hpp>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
//#include <infix_iterator.h>

#include <base64.h>

using namespace std;

class NtripClient{

    string url_, mount_point_, agent_, user_;
    int port_ {}, sock_ {};
    bool operating_ {};
    boost::function<void(string)> callback_;
    struct sockaddr_in serv_addr_;

    std::thread *t_queryServer_ {};
public:
    NtripClient(string agent_name,
                string url,
                int port,
                string mount_point,
                string user,
                string password,
                boost::function<void(string)> const &callback );

    NtripClient(string agent_name,
                string url,
                int port,
                string mount_point,
                boost::function<void(string)> const &callback );

    ~NtripClient();

    enum event{
        CONNECTED = 0
    };

    string getMountPointString();
    void start();
    void t_queryServer();
};

#endif // NTRIP_CLIENT_H

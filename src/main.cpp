//============================================================================
// Name        : ntrip_client.cpp
// Author      : Daniel Soto
// Version     :
// Copyright   : Use wisely
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ntrip_client/ntrip_client.h>
#include <csignal>
#include <string>

namespace
{
    volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler(int signal)
{
    gSignalStatus = signal;
}

void callback(unsigned char *buffer, ssize_t valread){
    ssize_t type_msg;
    for (ssize_t ind=0; ind < valread; ind++ ){
        if ( buffer[ind] == 0xD3 && (buffer[ind+1] & 0xFC) == 0x00 )
            type_msg = (buffer[ind+3] << 4 )+ ((buffer[ind+4] & 0xF0 )>>4);
            std::stringstream ss;
            ss << "Callback Found message, ind: "<< ind << " type: " << type_msg;
            std::cout << ss.str() << std::endl;
    }
}

int main() {
    boost::function<void(unsigned char*, ssize_t)> ntrip_cb=boost::bind(&callback, _1, _2);
    std::string name ("ntripclient_cpp/0.1");
    std::string usr ("centipede");
    std::string psd ("centipede");
    std::string caster ("caster.centipede.fr");
    std::string mountingpoint ("ENSIL");

    NtripClient client(name, caster, 2101, mountingpoint, usr, psd, ntrip_cb);
    try{
        client.start();
    }
    catch (const std::runtime_error &e){
        std::cout << "Runtime error: " << e.what() << std::endl;
        return -1;
    }
    std::cout << "Connected to NTRIP server!" << std::endl;

    std::signal(SIGINT, signal_handler);
    sleep(1);
    while (gSignalStatus != 2){
        //waiting for SIGINT signal Ctrl+C,
    }

    std::cout << "Exiting, joining thread" << std::endl;
    return 0;
}

//============================================================================
// Name        : ntrip_client.cpp
// Author      : Daniel Soto
// Version     :
// Copyright   : Use wisely
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <ntrip_client.h>
#include <csignal>
#include <string>

using namespace std;


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
    std::string usrpsd ("centipede");
    std::string caster ("caster.centipede.fr");
    NtripClient client(name, caster, 2101, "ENSIL", usrpsd, usrpsd, ntrip_cb);
    //try{
        client.start();
    //}
    //catch

    std::signal(SIGINT, signal_handler);
    sleep(1);
    while (gSignalStatus != 2){
        //waiting for SIGINT signal Ctrl+C
    }


    cout << "Exiting, joining thread" << endl; // prints !!!Hello World!!!
    return 0;
}

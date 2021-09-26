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

void callback(std::string msg){

}

int main() {
    boost::function<void(std::string)> ntrip_cb=boost::bind(&callback, _1);
    std::string name ("ntripclient_cpp/0.1");
    std::string caster ("caster.centipede.fr");
    NtripClient client(name, caster, 2101, "ENSIL", ntrip_cb);
    //try{
        client.start();
    //}
    //catch

    std::signal(SIGINT, signal_handler);
    sleep(1);
    while (gSignalStatus != 2){
        //waiting for SIGINT signal Ctrl+C
    }


    cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
    return 0;
}

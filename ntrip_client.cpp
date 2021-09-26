
#include "ntrip_client.h"


NtripClient::NtripClient(
        string agent_name,
        string url,
        int port,
        string mount_point,
        string user,
        string password,
        boost::function<void(string)> const &callback
        ):
    agent_ ("NTRIP " + agent_name),
    url_(url),
    port_(port),
    mount_point_(mount_point),
    operating_(false),
    callback_(callback),
    sock_(-1)
{
    string usrpswd(user + ":" + password);
    user_= base64_encode(user);
}


NtripClient::NtripClient(
        string agent_name,
        string url,
        int port,
        string mount_point,
        boost::function<void(string)> const &callback )
    :	NtripClient(agent_name, url, port, mount_point, "USR", "PSD", callback)
{}

NtripClient::~NtripClient(){
    if (operating_){
        operating_= false;
        t_queryServer_->join();
    }
}

string
NtripClient::getMountPointString()
{
    stringstream ss;
    string mountPointString;

    ss <<  "GET /" << mount_point_ << " HTTP/1.1\r\n" <<
           "User-Agent: "  << agent_ << "\r\n" <<
           "Authorization: Basic " <<  user_ << "\r\n" <<
           "Host: " << url_ << ":" << port_ << "\r\n" <<
           "Ntrip-Version: Ntrip/2.0\r\n" << "\r\n";

    mountPointString =  ss.str();
    return mountPointString;
}

void
NtripClient::start(){

    struct hostent * host = gethostbyname(url_.c_str());

    if ( (host == NULL) || (host->h_addr == NULL) ) {
        cout << "Error retrieving DNS information " << url_.c_str() << endl;
        return;
    }
    std::string ip = inet_ntoa(*((struct in_addr *)host->h_addr));

    if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        cout << "Socket creation error "<< endl;
        return;
    }

    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(port_);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr_.sin_addr)<=0)
    {
        cout <<  "Invalid address/ Address not supported: " << url_.c_str() << endl;
        return;
    }

    if(connect(sock_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
    {
        cout <<"Connection Failed"<< endl;
        return;
    }

    int  valread;
    char buffer[4096] = {0};
    string request = getMountPointString();
    send(sock_ , request.c_str() , strlen(request.c_str()) , 0 );
    valread = read( sock_ , buffer, 4096);

    char *output = NULL;
    output = strstr(buffer, "HTTP/1.1 200 OK");

    if(output) {
        cout << "Connected to " << url_ << endl;
        operating_ = true;
        t_queryServer_ = new std::thread(&NtripClient::t_queryServer, this);
    }
}

void
NtripClient::t_queryServer(){
    //std::list<std::string> header;
    while(operating_){
        sleep(1);
    }
}

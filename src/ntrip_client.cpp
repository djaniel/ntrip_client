
#include <ntrip_client/ntrip_client.h>



extern uint32_t getbitu(const uint8_t *buff, int pos, int len)
{
    uint32_t bits=0;
    int i;
    for (i=pos;i<pos+len;i++) bits=(bits<<1)+((buff[i/8]>>(7-i%8))&1u);
    return bits;
}

NtripClient::NtripClient(
        std::string agent_name,
        std::string url,
        int port,
        std::string mount_point,
        std::string user,
        std::string password,
        boost::function<void(unsigned char*, ssize_t)> const &callback
        ):
    agent_ ("NTRIP " + agent_name),
    url_(url),
    port_(port),
    mount_point_(mount_point),
    operating_(false),
    gpgga_("$GPGGA,134658.00,5106.9792,N,11402.3003,W,2,09,1.0,1048.47,M,-16.27,M,08,AAAA*60"),
    sock_(-1),
    callback_(callback)
{
    std::string usrpswd(user + ":" + password);
    user_= base64_encode(user);
}


NtripClient::NtripClient(
        std::string agent_name,
        std::string url,
        int port,
        std::string mount_point,
        boost::function<void(unsigned char*, ssize_t)> const &callback )
    :	NtripClient(agent_name, url, port, mount_point, "USR", "PSD", callback)
{}

NtripClient::~NtripClient(){
    if (operating_){
        operating_= false;
        t_queryServer_->join();
    }
}

std::string
NtripClient::getMountPointString()
{
    std::stringstream ss;

    ss <<  "GET /" << mount_point_ << " HTTP/1.1\r\n" <<
           "User-Agent: "  << agent_ << "\r\n" <<
           "Authorization: Basic " <<  user_ << "\r\n" <<
           "Host: " << url_ << ":" << port_ << "\r\n" <<
           "Ntrip-Version: Ntrip/2.0\r\n" << "\r\n";

    return  ss.str();
}


void
NtripClient::start(){
    struct hostent * host {nullptr};
    host = gethostbyname(url_.c_str());

    if ( (host == nullptr) || (host->h_addr == nullptr) )
        throw std::runtime_error("Error retrieving ip address from domain. DNS ok?" );

    std::string ip = inet_ntoa(*((struct in_addr *)host->h_addr));

    if ((sock_ = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        throw std::runtime_error( "Socket creation error" );

    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(static_cast<unsigned short> (port_));

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr_.sin_addr)<=0)
        throw std::runtime_error( "Invalid address/ Address not supported, DNS ok?" );


    if(connect(sock_, (struct sockaddr *)&serv_addr_, sizeof(serv_addr_)) < 0)
        throw std::runtime_error( "Connection to socket failed" );

    int  valread;
    char buffer[BUFFER_LENGTH] = {0};
    std::string request = getMountPointString();
    send(sock_ , request.c_str() , strlen(request.c_str()) , 0 );
    valread = read( sock_, buffer, BUFFER_LENGTH);

    char *output {nullptr};
    output = strstr(buffer, "HTTP/1.1 200 OK");

    if(output) {
        operating_ = true;
        t_queryServer_ = new std::thread(&NtripClient::t_queryServer, this);
    }
}

void
NtripClient::t_queryServer(){
    unsigned int sent_gppga = 60;
    unsigned char buffer[BUFFER_LENGTH] = {0};

    ssize_t valread;

    send(sock_ , gpgga_.c_str() , strlen(gpgga_.c_str()) , 0 );
    sleep(1);
    while(operating_){
        --sent_gppga;
        if ( sent_gppga == 0 ){
            sent_gppga = 60;
            send(sock_, gpgga_.c_str(), strlen(gpgga_.c_str()), 0 );
        }
        /*
        According to:
        https://www.use-snip.com/kb/knowledge-base/question-what-ntrip-client-should-i-use/
        "... your code will connect to the Caster (picking which stream),
        and then pipe that stream over a serial port to your rover GNSS device.
        You never need to understand or decode RTCM."
        */
        valread = read( sock_ , buffer, BUFFER_LENGTH);
        callback_(buffer, valread);

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

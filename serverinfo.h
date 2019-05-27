#ifndef SERVERINFO_H
#define SERVERINFO_H
#include<WinSock2.h>
#include<stdint.h>
#include<string>

class ServerInfo
{
public:
    ServerInfo();
    void setPort();
    void setName();
private:
    std::string Name;
    uint32_t Adress;
    unsigned Port;
    struct addrinfo addr_info;
    struct sockaddr_in serv_addr;
};

#endif // SERVERINFO_H

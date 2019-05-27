/**
  *************************************************************************************************************************
  * @file    mysocket.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   socket wrapper
  **************************************************************************************************************************
*/
#include "mysocket.h"
#include"exceptions.h"
#include<Ws2tcpip.h>
#include<cstdint>
#include<iostream>

/*constructor*/
mySocket::mySocket():
    blocking(0)
{
    socketId = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    /*throw exception if socket isn't created*/
    try {
        WSA_CHECK ((socketId == INVALID_SOCKET),"Failed to initialize socket");
    } catch (wsa_exception& ex) {
        std::cerr<<ex.what();
    }
    unsigned long _address;

    _address = ConvertAdress("");

    /*throw exception if adress couldn't be found*/
    try {
        WSA_CHECK((_address == INADDR_NONE),"Failed to resolve address");
    } catch (wsa_exception& ex) {
        std::cerr<<ex.what();
    }

    memset(&_socket_address, 0, sizeof(_socket_address));
    _socket_address.sin_family = AF_INET;
    _socket_address.sin_addr.s_addr = _address;
}

/**
* @brief constructor
* @param1 ip adress
* @param2 port
*/
mySocket::mySocket(const char* address, u_short port):
socketId(),
blocking(0)
{

    socketId = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    /*throw exception if socket isn't created*/
    try {
        WSA_CHECK ((socketId == INVALID_SOCKET),"Failed to initialize socket");
    } catch (wsa_exception& ex) {
        std::cerr<<ex.what();
    }

    unsigned long _address;

    _address = ConvertAdress(address);

    /*throw exception if adress couldn't be found*/
    try {
         WSA_CHECK((_address == INADDR_NONE),"Failed to resolve address");
    } catch (wsa_exception& ex) {
        std::cerr<<ex.what();
    }

    memset(&_socket_address, 0, sizeof(_socket_address));
    _socket_address.sin_family = AF_INET;
    _socket_address.sin_addr.s_addr = _address;
    _socket_address.sin_port = htons(port);
}

/**
* @brief constructor
* @param1 socket ( from WSAAccept)
*/
mySocket::mySocket(const SOCKET& socket):
    socketId(socket),
    blocking(0)
{
    char buff[INET_ADDRSTRLEN];
    int size = sizeof(_socket_address);
    memset(&_socket_address, 0, sizeof(_socket_address));
    memset(buff,'\0',INET_ADDRSTRLEN);

    getpeername(socket, reinterpret_cast<struct sockaddr*>(&_socket_address),
                &size);

    inet_ntop(AF_INET, &_socket_address.sin_addr.S_un.S_addr, buff, INET_ADDRSTRLEN);

    adress.assign(buff);
}


/*destructor*/
mySocket::~mySocket()
{
    free();
}

/**
* @brief destructor supporting function
* @note
*/
void mySocket::free()
{
    closesocket(socketId);
    socketId = INVALID_SOCKET;
}

/**
* @brief closes a socket
* @note
*/
void mySocket::Close()
{
    free();
}

/**
* @brief converts the adress to a number
* @note
* @retval address number
*/
unsigned long mySocket::ConvertAdress(const char* address)
{
    if (!strlen(address))
        return INADDR_ANY;

    uint32_t  ret = INADDR_NONE;
    struct addrinfo hints;
    struct addrinfo *hostPtr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    if (inet_pton(AF_INET,address,&ret) != 1)
    {
        if (getaddrinfo(address,nullptr,&hints,&hostPtr) == 0)
        {
            _socket_address =  *(reinterpret_cast<struct sockaddr_in*>(hostPtr->ai_addr));
            ret = _socket_address.sin_addr.S_un.S_addr;
            freeaddrinfo(hostPtr);
        }
    }
    return ret;
}

/**
* @brief get address
* @note
* @retval socket adress like a string
*/
std::string mySocket::getAddrstr()
{
    return adress;
}

/**
* @brief binds a socket
* @note
*/
void mySocket::Bind()
{

    WSA_CHECK(bind(socketId, reinterpret_cast<SOCKADDR*>(&_socket_address),
                         sizeof(_socket_address)), "Failed to bound socket");  
}



void mySocket::setDebug(int debugToggle)
{

      WSA_CHECK((setsockopt(socketId, SOL_SOCKET, SO_DEBUG,
                 reinterpret_cast<char *>(&debugToggle), sizeof(debugToggle))), "DEBUG option: ");



}

void mySocket::setReuseAddr(int reuseToggle)
{

    WSA_CHECK(setsockopt(socketId, SOL_SOCKET, SO_REUSEADDR,
                              reinterpret_cast<char *>(&reuseToggle), sizeof(reuseToggle)),"REUSEADDR option: ");

}


void mySocket::setKeepAlive(int aliveToggle)
{

        WSA_CHECK(setsockopt(socketId, SOL_SOCKET, SO_KEEPALIVE,
                             reinterpret_cast<char *>(&aliveToggle), sizeof(aliveToggle)),"ALIVE option: ");
}

void mySocket::setLingerSeconds(int seconds)
{
    struct linger lingerOption;

    if (seconds > 0)
    {
        lingerOption.l_linger = static_cast<u_short>(seconds);
        lingerOption.l_onoff = 1;
    }
    else lingerOption.l_onoff = 0;


     WSA_CHECK(setsockopt(socketId, SOL_SOCKET, SO_LINGER,
                              reinterpret_cast<char*>(&lingerOption), sizeof(struct linger)),"LINGER option: ");


}

void mySocket::setLingerOnOff(bool lingerOn)
{
    struct linger lingerOption;

    if (lingerOn) lingerOption.l_onoff = 1;
    else lingerOption.l_onoff = 0;


    WSA_CHECK(setsockopt(socketId, SOL_SOCKET, SO_LINGER,
           reinterpret_cast<char*>(&lingerOption), sizeof(struct linger)),"LINGER option: ");

}


void mySocket::setSocketBlocking(int blockingToggle)
{
    if (blockingToggle)
    {
        if (getSocketBlocking()) return;
        blocking = 1;
    }
    else
    {
        if (!getSocketBlocking()) return;
        blocking = 0;
    }

    WSA_CHECK(ioctlsocket(socketId, FIONBIO,
              reinterpret_cast<unsigned long *>(&blocking)),
              "Blocking option: ");


}

int mySocket::getDebug()
{
    int myOption;
    int myOptionLen = sizeof(myOption);

    WSA_CHECK(getsockopt(socketId, SOL_SOCKET, SO_DEBUG,
              reinterpret_cast<char*>(&myOption), &myOptionLen),
              "getDEBUG option: ");

    return myOption;
}

int mySocket::getReuseAddr()
{
    int myOption;
    int myOptionLen = sizeof(myOption);

    WSA_CHECK(getsockopt(socketId, SOL_SOCKET, SO_REUSEADDR,
                        reinterpret_cast<char*>(&myOption), &myOptionLen),
                        "get REUSEADDR option: ");
    return myOption;
}

int mySocket::getKeepAlive()
{
    int myOption;
    int myOptionLen = sizeof(myOption);


    WSA_CHECK(getsockopt(socketId, SOL_SOCKET, SO_KEEPALIVE,
                         reinterpret_cast<char*>(&myOption), &myOptionLen),
                            "get KEEPALIVE option: ");

    return myOption;
}

int mySocket::getLingerSeconds()
{
    struct linger lingerOption;
    int myOptionLen = sizeof(struct linger);

    WSA_CHECK(getsockopt(socketId, SOL_SOCKET, SO_LINGER,
                         reinterpret_cast<char*>(&lingerOption), &myOptionLen),
                         "get LINER option: ");

    return lingerOption.l_linger;
}

bool mySocket::getLingerOnOff()
{
    struct linger lingerOption;
    int myOptionLen = sizeof(struct linger);

    WSA_CHECK(getsockopt(socketId, SOL_SOCKET, SO_LINGER,
                         reinterpret_cast<char*>(&lingerOption), &myOptionLen),
                         "get LINER option: ");

    return lingerOption.l_onoff == 1;
}

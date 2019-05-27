/**
  *************************************************************************************************************************
  * @file    mysocket.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   socket wrapper
  **************************************************************************************************************************
*/
#ifndef MYSOCKET_H
#define MYSOCKET_H
#include<WinSock2.h>
#include<string>



class mySocket
{
public:
    /*constructor*/
    explicit mySocket();
    /**
    * @brief constructor
    * @param1 ip adress
    * @param2 port
    */
    mySocket(const char* address, u_short port);

    /**
    * @brief constructor
    * @param1 socket ( from WSAAccept)
    */
    mySocket(const SOCKET&);
    mySocket(const mySocket&) = delete;
    mySocket(mySocket&&) = default;
    mySocket& operator=(const mySocket&) = delete;
    /*destructor*/
    ~mySocket();
    /*socket options : ON/OFF*/
    void setDebug(int);
    void setReuseAddr(int);
    void setKeepAlive(int);
    void setLingerOnOff(bool);
    void setLingerSeconds(int);
    void setSocketBlocking(int);
    /*retrieve socket option settings*/
    int  getDebug();
    int  getReuseAddr();
    int  getKeepAlive();
    int  getSocketBlocking() { return blocking; }
    int  getLingerSeconds();
    bool getLingerOnOff();

    /**
    * @brief get raw socket
    * @note
    * @retval socket file descriptor
    */
    SOCKET getSocketId() { return socketId; }

    /**
    * @brief get address
    * @note
    * @retval socket adress object
    */
    sockaddr_in& getAddress() {return _socket_address; }

    /**
    * @brief get address
    * @note
    * @retval socket adress like a string
    */
    std::string getAddrstr();

    /**
    * @brief binds a socket
    * @note
    */
    void Bind();

    /**
    * @brief closes a socket
    * @note
    */
    void Close();

private:

    /**
    * @brief destructor supporting function
    * @note
    */
    void free();

    /**
    * @brief converts the adress to a number
    * @note
    * @retval address number
    */
    unsigned long ConvertAdress(const char* address);  
    /*Socket file descriptor*/
    SOCKET socketId;
    /*native address object*/
    sockaddr_in _socket_address;
    /**/
    std::string adress;
    /*blocking flag*/
    int blocking;
};

#endif // MYSOCKET_H

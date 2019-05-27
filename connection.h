/**
  *************************************************************************************************************************
  * @file    connecction.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Used to hold a client connection, strore the data sended
  * @brief   by client and received from remote server
  **************************************************************************************************************************
*/
#ifndef CONNECTION_H
#define CONNECTION_H
#include<memory>
#include<vector>
#include<atomic>
#include<WinSock2.h>
#include"buffer.h"

class mySocket;
class Context;

class Connection
{
public:
    /**
    * @brief constructor
    * @param1 accepted socket
    */
    explicit Connection(const SOCKET&);

    Connection(Connection&&) = default;

    Connection(const Connection&) = delete;

    Connection& operator=(const Connection&) = delete;

    Connection& operator=(Connection&&) = delete;

    ~Connection() = default;

    /**
    * @brief close accepted and connected sockets
    * @note
    */
    void Disconnect();

    /**
    * @brief get pointer to the buffer of received from client data
    * @note
    * @retval ptr to the buffer
    */
    Buffer* getRequestBuffer();

    /**
    * @brief get pointer to the buffer of received from remote server data
    * @note
    * @retval ptr to the buffer
    */
    Buffer* getReceivedDataBuffer();

    /**
    * @brief get acecepted socket
    * @note
    * @retval accepted socket
    */
    mySocket* getAcceptSocket();

    /**
    * @brief get connected socket
    * @note
    * @retval connected socket
    */
    mySocket* getConnectSocket();

    /**
    * @brief get connection disconnecting state
    * @note
    * @retval true if connection is disconnecting
    */
    bool Disconneting();

    /**
    * @brief set connection disconnecting state
    * @note after client sended quit request or
    * @note WSARecv/WSASend to client failed connection is set to
    * @note disconnecting state
    * @retval true if connection is disconnecting
    */
    void setDisctonnecting();

private:
    /*both buffers size*/
    static const unsigned BUFFSIZE {50};
    /*pointer to the buffer of data received client*/
    std::unique_ptr<Buffer> clientrequest;
    /*pointer to the buffer of data received remote server*/
    std::unique_ptr<Buffer> receiveddata;
    /**/
    std::unique_ptr<mySocket> acceptSocket;
    /**/
    std::unique_ptr<mySocket> connectSocket;
    /**/
    std::atomic<bool> disconnecting{false};

};



#endif // CONNECTION_H

/**
  *************************************************************************************************************************
  * @file    connecction.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Used to hold a client connection, strore the data sended
  * @brief   by client and received from remote server
  **************************************************************************************************************************
*/
#include "connection.h"
#include"context.h"
#include"buffer.h"
#include"mysocket.h"
#include"ProxyServer.h"
#include"exceptions.h"
#include<iostream>

/**
* @brief constructor
* @param1 accepted socket
*/
Connection::Connection(const SOCKET& socket):
      clientrequest(std::make_unique<Buffer>(Context::Type::RcvFromClient,BUFFSIZE)),
      receiveddata(std::make_unique<Buffer>(Context::Type::RcvFromServer,BUFFSIZE)),
      acceptSocket(std::make_unique<mySocket>(socket)),
      connectSocket(std::make_unique<mySocket>()),
      disconnecting(false)
{
}

/**
* @brief get connection disconnecting state
* @note
* @retval true if connection is disconnecting
*/
bool Connection::Disconneting()
{
    return disconnecting.load();
}

/**
* @brief set connection disconnecting state
* @note after client sended quit request or
* @note WSARecv/WSASend to client failed connection is set to
* @note disconnecting state
* @retval true if connection is disconnecting
*/
void Connection::setDisctonnecting()
{
    disconnecting.store(true);
}

/**
* @brief close accepted and connected sockets
* @note
*/
void Connection::Disconnect()
{
    acceptSocket->Close();
    connectSocket->Close();
}

/**
* @brief get pointer to the buffer of received from client data
* @note
* @retval ptr to the buffer
*/
Buffer* Connection::getRequestBuffer()
{
    return clientrequest.get();
}

/**
* @brief get pointer to the buffer of received from remote server data
* @note
* @retval ptr to the buffer
*/
Buffer* Connection::getReceivedDataBuffer()
{
    return receiveddata.get();
}

/**
* @brief get acecepted socket
* @note
* @retval accepted socket
*/
mySocket* Connection::getAcceptSocket()
{
    return acceptSocket.get();
}

/**
* @brief get connected socket
* @note
* @retval connected socket
*/
mySocket* Connection::getConnectSocket()
{
    return connectSocket.get();
}

/**
  *************************************************************************************************************************
  * @file    threadpool.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   holds the threads
  **************************************************************************************************************************
*/
#ifndef THREADSPOOL_H
#define THREADSPOOL_H
#include<vector>
#include<thread>
#include<memory>
#include"ProxyServer.h"
#include<WinSock2.h>


class ControlConnections;
class Connection;
class ComplationPort;
class Context;
class MySQLQueryParser;
class Logger;


class ThreadsPool
{
public:
    /**
    * @brief constructor
    * @param1 pointer to the proxy server
    * @param2 pointer to the control connection
    * @param3 pointer to the complation port
    * @param4 number of a worker threads
    * @param5 number of a accept threads
    */
    ThreadsPool(ProxyServer*, ControlConnections*,ComplationPort*,uint16_t,uint16_t);
    /*destructor*/
    ~ThreadsPool();

    /**
    * @brief stop threads
    * @note
    */
    void Stop();

    /**
    * @brief set the logger object
    * @note
    * @param1 pointer to the logger object
    */
    void setLogger(Logger*);

    /**
    * @brief reset the logger object
    * @note
    */
    void resetLogger();
private:

    /**
    * @brief destructor supporting function
    * @note
    */
    void free();

    /**
    * @brief worker threads fucntion
    * @note
    */
    void WorkerThread();

    /**
    * @brief accept threads fucntion
    * @note
    */
    void AcceptThread();

    /**
    * @brief receive data from client handling
    * @note
    * @param1 current connenction
    * @param2 received context
    * @param3 number of received bytes
    */
    void HandleReceiveFromClient(Connection& ,Context& ,DWORD  );

    /**
    * @brief receive data from remote server handling
    * @note
    * @param1 current connenction
    * @param2 received context
    * @param3 number of received bytes
    */
    void HandleReceiveFromRemoteServer( Connection& ,Context& ,DWORD   );

    /**
    * @brief send data to remote server handling
    * @note
    * @param1 current connenction
    * @param2 sended context
    * @param3 number of sended bytes
    */
    void HandleSendToRemoteServer( Connection& ,Context& ,DWORD   );

    /**
    * @brief send data to client handling
    * @note
    * @param1 current connenction
    * @param2 sended context
    * @param3 number of sended bytes
    */
    void HandleSendToClient( Connection& ,Context& ,DWORD   );

    /**
    * @brief disconenctr a client from the remote server
    * @note
    * @param1 current connenction
    * @param2 sended quit context
    * @param3 number of sended bytes
    */
    void DisconnectFromRemoteServer(Connection& ,Context& ,DWORD  );

    /**
    * @brief close and delete current connection
    * @note
    */
    void DisconnectClient(Connection&);
    /**/
    std::vector<std::thread> threads;
    /**/
    std::unique_ptr<MySQLQueryParser> Pparser;
    /**/
    ProxyServer* Pserver;
    /**/
    ComplationPort* PCmplPort;
    /**/
    ControlConnections* PConncontrol;
    /**/
    Logger* PLogger;
    /*true if threads are stopped*/
    bool b_stop{false};
};

#endif // THREADSPOOL_H

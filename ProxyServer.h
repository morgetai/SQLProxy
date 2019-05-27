/**
  *************************************************************************************************************************
  * @file    proxyserver.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   server object
  **************************************************************************************************************************
*/
#ifndef PROXYSERVER_H
#define PROXYSERVER_H
#include<memory>
#include<cstdint>
#include<string>
#include<WinSock2.h>



class ThreadsPool;
class ControlConnections;
class mySocket;
class ComplationPort;
class Connection;
class mySocket;
class Logger;


class ProxyServer
{
public:
    enum class ProxyState
    {
        Init,
        Running,
        Stop,
        Quit
    };

    /*constructor*/
    explicit ProxyServer();
    /**/
    ProxyServer(const ProxyServer&) = delete;
    /**/
    ProxyServer(ProxyServer&&) = delete;
    /**/
    ProxyServer& operator=(const ProxyServer&) = delete;
    /*destructor*/
    ~ProxyServer();

    /**
    * @brief initialise the server
    * @note
    */
    void Init();

    /**
    * @brief runs the server
    * @note
    */
    void Run();

    /**
    * @brief stops the sever
    * @note
    */
    void Stop();

    /**
    * @brief connect to remote server
    * @note
    * @param1 pointer to the socket to be connected
    */
    void ConnectToRemoteServer(mySocket*);

    /**
    * @brief get number of connected clients
    * @note
    * @param1 number of clients
    */
    size_t GetNumClients();

    /**
    * @brief get server socket
    * @note
    */
    mySocket* getListensocket();

    /**
    * @brief set logging on
    * @note
    */
    void setLogOn();

    /**
    * @brief set logging off
    * @note
    */
    void setLogOff();

    /**
    * @brief set the remote server adress
    * @note
    * @param1 server ip adress passed in dot notation or server name
    */
    void setServerAdress(const std::string&);

    /**
    * @brief set the remote server port
    * @note
    * @param1 port number
    */
    void setServerPort(unsigned short);

    /**
    * @brief set remote server adress
    * @note
    * @param1 server ip adress
    */
    void setRemoteServerAdress(const std::string& );

    /**
    * @brief set remote server port
    * @note
    * @param1 port number
    */
    void setRemoteServerPort(unsigned short);

private:

    /**
    * @brief runs the server
    * @note
    */
    void Running();

    /**
    * @brief initialize winsock
    * @note
    */
    void InitializeWinsock();

    /**
    * @brief initialize IOCP
    * @note
    */
    void InitializeIocp();

    /**
    * @brief initialize threadpool
    * @note
    * @param1 pointer tp a the server
    * @param2 pointer to a controlconnection object
    * @param3 pointer to a complation port wrapper
    * @param4 number of worker threads
    * @param5 number of accept threads
    */
    void InitializeThreadPool(ProxyServer*, ControlConnections*,
                              ComplationPort*, unsigned int,unsigned int);
    /**
    * @brief initialize server socket
    * @note
    */
    void InitializeSocket();
    /*server state*/
    ProxyState State;
    /*pointer to a threadpool*/
    std::unique_ptr<ThreadsPool> PThreadPool;
    /**/
    std::unique_ptr<mySocket> ListenSocket;
    /**/
    std::unique_ptr<ControlConnections> ConnControl;
    /**/
    std::unique_ptr<ComplationPort> CmplPort;
    /**/
    std::unique_ptr<Logger> log;
    /*native remote server address object*/
    sockaddr_in RemoteServer;
    /*server name or IP adress*/
    std::string Name;
    /*server port*/
    unsigned short Port;
    /*true if logging is on*/
    bool bLogging{false};
    /*true if server is initialized*/
    bool b_Inited{false};
};

#endif // IOCPS_H

/**
  *************************************************************************************************************************
  * @file    proxyserver.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   server object
  **************************************************************************************************************************
*/
#include"ProxyServer.h"
#include"exceptions.h"
#include"Threadspool.h"
#include"controlconnections.h"
#include"mysocket.h"
#include"complationport.h"
#include"logger.h"
#include<Ws2tcpip.h>
#include<iostream>

/*constructor*/
ProxyServer::ProxyServer():
    log(std::make_unique<Logger>())
{
    RemoteServer.sin_family = AF_INET;
    Name = "127.0.0.1";
    Port = 999;
    State = ProxyServer::ProxyState::Init;
}

/*destructor*/
ProxyServer::~ProxyServer()
{
    if (!b_Inited)
    {
        WSACleanup();
    }

}

/**
* @brief runs the server
* @note
*/
void ProxyServer::Run()
{
    while (State != ProxyServer::ProxyState::Quit)
    {
        switch(State)
        {
        case ProxyServer::ProxyState::Init:
            Init();
            setLogOn();//turn on logging
            State = ProxyServer::ProxyState::Running;
            break;
        case ProxyServer::ProxyState::Running:
            Running();
            break;
        case ProxyServer::ProxyState::Stop:
            //deinit all
            Stop();
            break;
        case ProxyServer::ProxyState::Quit:
            //close program
            return;
        }
    }
}

/**
* @brief stops the sever
* @note
*/
void ProxyServer::Stop()
{
    if (b_Inited)
    {
        /**/
        PThreadPool.reset();
        /**/
        ConnControl.reset();
        /**/
        ListenSocket.reset();
        /**/
        CmplPort.reset();
        /**/
        WSACleanup();
        /**/
        b_Inited = false;
    }
}

/**
* @brief runs the server
* @note
*/
void ProxyServer::Running()
{
    if (log)
    {
        log->ActualLogging();
    }
}

/**
* @brief set logging on
* @note
*/
void ProxyServer::setLogOn()
{
    log = std::make_unique<Logger>();
    PThreadPool->setLogger(log.get());
}

/**
* @brief set logging off
* @note
*/
void ProxyServer::setLogOff()
{
    if (State != ProxyServer::ProxyState::Running && log)
    {
        log = nullptr;
    }

}

/**
* @brief initialise the server
* @note
*/
void ProxyServer::Init()
{
    unsigned int Processors_number =
            std::thread::hardware_concurrency();
    if (Processors_number == 0)
    {
        Processors_number = 2;
    }

    /*if some initialization has failed shutdown the program*/
    try {

        InitializeWinsock();

        ConnControl = std::make_unique<ControlConnections>();

        InitializeIocp();

        InitializeSocket();

        InitializeThreadPool(this,ConnControl.get(),CmplPort.get(),Processors_number ,2);

    } catch (wsa_exception& excp) {
        std::cerr<<excp.what();
        exit(1);
    }

    int size = sizeof(sockaddr_in);
    char adress[INET_ADDRSTRLEN];

    getpeername(ListenSocket->getSocketId(), reinterpret_cast<struct sockaddr*>(&ListenSocket->getAddress()),
                &size);

    inet_ntop(AF_INET, &ListenSocket->getAddress().sin_addr.S_un.S_addr, adress, INET_ADDRSTRLEN);

    std::cout << "Proxy IP adress: "<< adress << "\n";
    std::cout << "Proxy Port: "<< Port << "\n";

    b_Inited = true;
}

/**
* @brief connect to remote server
* @note
* @param1 pointer to the socket to be connected
*/
void ProxyServer::ConnectToRemoteServer(mySocket* socket)
{   
    WSA_CHECK(connect(socket->getSocketId(),
                      reinterpret_cast<sockaddr*>(&RemoteServer),
                      sizeof(RemoteServer)) == SOCKET_ERROR,
              "Error connect to remote server ");
}

/**
* @brief set remote server adress
* @note
* @param1 server ip adress
*/
void ProxyServer::setRemoteServerAdress(const std::string& adress)
{
    struct addrinfo hints;
    struct addrinfo *hostPtr;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;
    /*if couldn't find the remote server adress close the program*/
    try {
        if (inet_pton(AF_INET,adress.c_str(),&RemoteServer.sin_addr) != 1)
        {
            if (getaddrinfo(adress.c_str(),nullptr,&hints,&hostPtr) != 0)
            {
                WSA_CHECK(true,"cannot resolve remote server name ");
            }
            else {
                RemoteServer = *(reinterpret_cast<struct sockaddr_in*>(hostPtr->ai_addr));
                freeaddrinfo(hostPtr);
            }
        }
    } catch (wsa_exception& ex) {
        std::cerr<<ex.what() << "\n";
        exit(1);
    }
}

/**
* @brief set the remote server port
* @note
* @param1 port number
*/
void ProxyServer::setRemoteServerPort(unsigned short port)
{
    RemoteServer.sin_port = htons(port);
}

/**
* @brief set the remote server adress
* @note
* @param1 server ip adress passed in dot notation or server name
*/
void ProxyServer::setServerAdress(const std::string& adress)
{
    Name = adress;
}

/**
* @brief set the remote server port
* @note
* @param1 port number
*/
void ProxyServer::setServerPort(unsigned short port)
{
     Port = port;
}

/**
* @brief get server socket
* @note
*/
mySocket* ProxyServer::getListensocket()
{
    return ListenSocket.get();
}

/**
* @brief initialize winsock
* @note
*/
void ProxyServer::InitializeWinsock()
{
    WSADATA wsaData;

    WSA_CHECK(WSAStartup(MAKEWORD(2,2), &wsaData), "WSAStartup error" );

}

/**
* @brief initialize IOCP
* @note
*/
void ProxyServer::InitializeIocp()
{
    //Create I/O completion port
    CmplPort = std::make_unique<ComplationPort>();

}

/**
* @brief initialize server socket
* @note
*/
void ProxyServer::InitializeSocket()
{
    ListenSocket = std::make_unique<mySocket>(Name.c_str(),Port);
    ListenSocket->Bind();
    WSA_CHECK(listen(ListenSocket->getSocketId(), 5) == SOCKET_ERROR,"Listen failed");

}

/**
* @brief initialize threadpool
* @note
* @param1 pointer tp a the server
* @param2 pointer to a controlconnection object
* @param3 pointer to a complation port wrapper
* @param4 number of worker threads
* @param5 number of accept threads
*/
void ProxyServer::InitializeThreadPool(ProxyServer* serverptr, ControlConnections* ccontrol,
                                 ComplationPort* CPort,unsigned int workthreads,unsigned int accthreads)
{
    PThreadPool = std::make_unique<ThreadsPool>(serverptr,ccontrol,CPort,workthreads,accthreads);
}

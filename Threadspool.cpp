/**
  *************************************************************************************************************************
  * @file    threadpool.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   holds the threads
  **************************************************************************************************************************
*/
#include<algorithm>
#include"Threadspool.h"
#include"connection.h"
#include"exceptions.h"
#include"controlconnections.h"
#include"context.h"
#include"buffer.h"
#include"mysocket.h"
#include"complationport.h"
#include"logger.h"
#include<iostream>
#include"mysqlqueryparser.h"

/**
* @brief constructor
* @param1 pointer to the proxy server
* @param2 pointer to the control connection
* @param3 pointer to the complation port
* @param4 number of a worker threads
* @param5 number of a accept threads
*/
ThreadsPool::ThreadsPool( ProxyServer* serverptr,
                          ControlConnections* conncontrol,
                          ComplationPort* cmplport,
                          uint16_t workthreads_qty,
                          uint16_t acceptreads_qty):
    Pparser(std::make_unique<MySQLQueryParser>()),
    Pserver(serverptr),
    PCmplPort(cmplport),
    PConncontrol(conncontrol),
    PLogger(nullptr)
{
    for (uint16_t i = 0; i < workthreads_qty; ++i)
    {
        threads.emplace_back(std::thread([&]{WorkerThread();}));

    }

    for (uint16_t i = 0; i < acceptreads_qty; ++i)
    {
        threads.emplace_back(std::thread([&]{AcceptThread();}));
    }

}

/*destructor*/
ThreadsPool::~ThreadsPool()
{
    free();
}

/**
* @brief destructor supporting function
* @note
*/
void ThreadsPool::free()
{
    b_stop = true;
    std::for_each(threads.begin(),threads.end(),[](std::thread& th)
    {
        th.join();
    });
}

/**
* @brief accept threads fucntion
* @note
*/
void ThreadsPool::AcceptThread()
{
    SOCKET AccepptSocket = INVALID_SOCKET ;
    SOCKET ListenSocket = Pserver->getListensocket()->getSocketId();
    SOCKADDR_IN saRemote = Pserver->getListensocket()->getAddress();
    ULONG ulFlags = MSG_PARTIAL;
    DWORD dwIoSize=0;
    int RemoteLen = sizeof(saRemote);

    Connection* tmp_connection;
    Context* tmp_context;

    while(!b_stop)
    {
        //accept
        try
        {
            WSA_CHECK((AccepptSocket = WSAAccept(ListenSocket,
                                                 reinterpret_cast<SOCKADDR *>(&saRemote), &RemoteLen,
                                                 nullptr, NULL)) == INVALID_SOCKET, "WSAAccept error");
            //create new connection , add new connection
            while((tmp_connection = PConncontrol->AddConnection(
                       std::make_unique<Connection>(AccepptSocket))) == nullptr)
            {
            }
            //update complation port with a new connection
            PCmplPort->Update(tmp_connection);

            //connect to remote server
            Pserver->ConnectToRemoteServer(tmp_connection->getConnectSocket());

            //reset wsabuff in received data buffer
            tmp_context = tmp_connection->getReceivedDataBuffer()->getCurrentPos();
            tmp_context->ResetWsaBuf();

            //post receive from remote server
            if (WSARecv(tmp_connection->getConnectSocket()->getSocketId(),
                        &tmp_context->m_wsaBuffer,
                        1,
                        &dwIoSize,
                        &ulFlags,
                        &tmp_context->Overlapped,
                        nullptr) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                    WSA_CHECK( true, "WSARecv from server error");
            }

            //reset wsabuff in request data buffer
            tmp_context = tmp_connection->getRequestBuffer()->getCurrentPos();
            tmp_context->ResetWsaBuf();

            //post receive from client
            if (WSARecv(AccepptSocket,
                        &(tmp_context->m_wsaBuffer),
                        1,
                        &dwIoSize,&ulFlags,
                        &(tmp_context->Overlapped),
                        nullptr) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                    WSA_CHECK( true, "\nWSARecv receive from client error");
            }
        }

        catch (wsa_exception& excp) {
            std::cerr << excp.what();
            DisconnectClient(*tmp_connection);
        }
    }
}

/**
* @brief worker threads fucntion
* @note
*/
void ThreadsPool::WorkerThread()
{
    DWORD bytesTransferred = 0;
    int searchresult = 0;
    ULONG_PTR LPConnection;
    Context *LPContext = nullptr;
    Connection* Pconnection = nullptr;
    while(!b_stop)
    {
       searchresult = -1;

       /*GetQueuedCompletionStatus can failed*/
        try {

            WSA_CHECK(GetQueuedCompletionStatus(PCmplPort->getHandle(),
                                                &bytesTransferred,
                                                &LPConnection,
                                                reinterpret_cast<LPOVERLAPPED*>(&LPContext),
                                                INFINITE) == 0,
                      " GetQueuedCompletionStatus() failed ");
        }
        catch (wsa_exception& excp) {
            std::cerr << excp.what();
            continue;
        }

        while((searchresult = PConncontrol->GetConnection(reinterpret_cast<Connection*>(LPConnection),
                                                          Pconnection)) == -1)
        {}

        if(searchresult > 0)
        {
            switch(LPContext->getContextType())
            {
            case Context::Type::RcvFromClient:
                if (!Pconnection->Disconneting())
                {
                    HandleReceiveFromClient(*Pconnection,*LPContext,bytesTransferred);
                }
                break;
            case Context::Type::SendToServer:
                if (!Pconnection->Disconneting())
                {
                    HandleSendToRemoteServer(*Pconnection,*LPContext,bytesTransferred);
                }
                break;
            case Context::Type::RcvFromServer:
                if (!Pconnection->Disconneting())
                {
                    HandleReceiveFromRemoteServer(*Pconnection,*LPContext,bytesTransferred);
                }
                break;
            case Context::Type::SendToClient:
                if (!Pconnection->Disconneting())
                {
                    HandleSendToClient(*Pconnection,*LPContext,bytesTransferred);
                }
                break;
            case Context::Type::DisconnectClient:
                DisconnectClient(*Pconnection);
                break;
            case Context::Type::DisconnectFromServer:
                break;

            }
        }
        else
        {
            //connection not found
        }
    }

}

/**
* @brief receive data from client handling
* @note
* @param1 current connenction
* @param2 received context
* @param3 number of received bytes
*/
void ThreadsPool::HandleReceiveFromClient(Connection& Pconnection,
                                          Context& Pcontext, DWORD bytes)
{
    ULONG ulFlags = MSG_PARTIAL;
    DWORD dwSendNumBytes = 0;
    DWORD dwIoSize=0;
    if (bytes > 0)
    {
        //save stored bytes number
        Pcontext.setStoredBytes(bytes);
        //parse and log if needed
        switch (Pparser->GetCommandType(Pcontext)) {
        /*case MySQLQueryParser::SQL_BATCH:
            log->AddToLogging(parser->GetQuery(context),curr_connection->getAcceptSocket());
            break;*/
        case MySQLQueryParser::COM_QUIT:
            DisconnectFromRemoteServer(Pconnection,Pcontext, bytes);
            return;
        case MySQLQueryParser::COM_QUERY:
            if (PLogger != nullptr)
                PLogger->AddToLogging(Pparser->GetQuery(Pcontext),Pconnection.getAcceptSocket());
            break;
        case MySQLQueryParser::COM_STMT_PREPARE:
            if (PLogger != nullptr)
                PLogger->AddToLogging(Pparser->GetQuery(Pcontext),Pconnection.getAcceptSocket());
            break;
        }
        //change context type
        Pcontext.setContextType(Context::Type::SendToServer);
        //set length of send buffer
        Pcontext.setWSABUFFToSentbytes();
        //disconnect client if fails
        try
        {
            //post send to remote server and log the data
            if(WSASend(Pconnection.getConnectSocket()->getSocketId(),
                       &Pcontext.m_wsaBuffer,
                       1,
                       &dwSendNumBytes,
                       ulFlags,
                       &Pcontext.Overlapped,
                       nullptr) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                    WSA_CHECK( true, "\nWSASend to remote server error ");
            }
            //increment position in buffer
            Pconnection.getRequestBuffer()->IncrementCurrentPos();


            //reset wsabuff
            Pconnection.getRequestBuffer()->getCurrentPos()->ResetWsaBuf();
            //post receive from client
            if (WSARecv(Pconnection.getAcceptSocket()->getSocketId(),
                        &Pconnection.getRequestBuffer()->getCurrentPos()->m_wsaBuffer,
                        1,
                        &dwIoSize,&ulFlags,
                        &Pconnection.getRequestBuffer()->getCurrentPos()->Overlapped,
                        nullptr) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                    WSA_CHECK( true, "\nWSARecv from client error");
            }


        }
        catch (wsa_exception& excp) {
            //disconnect client
            std::cerr << excp.what();
            if (!Pconnection.Disconneting())
            {
                Pconnection.setDisctonnecting();
                DisconnectClient(Pconnection);
            }
        }
    }
}

/**
* @brief send data to remote server handling
* @note
* @param1 current connenction
* @param2 sended context
* @param3 number of sended bytes
*/
void ThreadsPool::HandleSendToRemoteServer(Connection& Pconnection,
                                           Context& Pcontext, DWORD bytes)
{
    DWORD dwSendNumBytes = 0;
    ULONG ulFlags = MSG_PARTIAL;
    try {
        if (bytes > 0)
        {
            //check send bytes
            if (!Pcontext.addSentBytes(bytes))
            {
                //
                // the previous write operation didn't send all the data,
                // post another send to complete the operation
                //
                Pcontext.setWSABUFFToSentbytes();
                if (WSASend(Pconnection.getConnectSocket()->getSocketId(),
                            &Pcontext.m_wsaBuffer,
                            1,
                            &dwSendNumBytes,
                            ulFlags,
                            &Pcontext.Overlapped,
                            nullptr) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != ERROR_IO_PENDING)
                        WSA_CHECK( true, "WSASend to remote server error");
                }
            }
            else
            {
                //
                // the previous write operation completed
                // so change context to defult
                Pcontext.setContextType(Context::Type::RcvFromClient);
            }
        }
    }
    catch (wsa_exception& excp) {
        //disconnect client
        std::cerr << excp.what();
        if (!Pconnection.Disconneting())
        {
            Pconnection.setDisctonnecting();
            DisconnectClient(Pconnection);
        }
    }
}

/**
* @brief disconenctr a client from the remote server
* @note
* @param1 current connenction
* @param2 sended quit context
* @param3 number of sended bytes
*/
void ThreadsPool::HandleReceiveFromRemoteServer(Connection& Pconnection,
                                                Context& Pcontext, DWORD bytes)
{
    DWORD dwIoSize=0;
    DWORD dwSendNumBytes = 0;
    ULONG ulFlags = MSG_PARTIAL;
    /*disconnect client if some fuction fails*/
    try {
        if (bytes > 0)
        {
            //save received bytes number
            Pcontext.setStoredBytes(bytes);
            //change context type
            Pcontext.setContextType(Context::Type::SendToClient);

            //post send to client
            Pcontext.setWSABUFFToSentbytes();
            if(WSASend(Pconnection.getAcceptSocket()->getSocketId(),
                              &Pcontext.m_wsaBuffer,
                              1,
                              &dwSendNumBytes,
                              ulFlags,
                              &Pcontext.Overlapped,
                              nullptr) == SOCKET_ERROR)
            {
                if (WSAGetLastError() != ERROR_IO_PENDING)
                    WSA_CHECK( true, "\nWSASend to client error");
            }

            //increment position in buffer
            Pconnection.getReceivedDataBuffer()->IncrementCurrentPos();
        }
        //reset wsabuff
        Pconnection.getReceivedDataBuffer()->getCurrentPos()->ResetWsaBuf();
        //post receive from remote server
        if (WSARecv(Pconnection.getConnectSocket()->getSocketId(),
                        &Pconnection.getReceivedDataBuffer()->getCurrentPos()->m_wsaBuffer,
                        1,
                        &dwIoSize,
                        &ulFlags,
                        &Pconnection.getReceivedDataBuffer()->getCurrentPos()->Overlapped,
                        nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
                WSA_CHECK( true, "\nWSARecv from server error");
        }

    } catch (wsa_exception& excp) {
        std::cerr << excp.what();
        if (!Pconnection.Disconneting())
        {
            Pconnection.setDisctonnecting();
            DisconnectClient(Pconnection);
        }
    }
}

/**
* @brief send data to client handling
* @note
* @param1 current connenction
* @param2 sended context
* @param3 number of sended bytes
*/
void ThreadsPool::HandleSendToClient(Connection& Pconnection,
                                     Context& Pcontext, DWORD bytes)
{
    DWORD dwSendNumBytes = 0;
    ULONG ulFlags = MSG_PARTIAL;
    /*disconnect client if some fuction fails*/
    try {
        if (bytes > 0)
        {
            //check send bytes
            if (!Pcontext.addSentBytes(bytes))
            {
                //
                // the previous write operation didn't send all the data,
                // post another send to complete the operation
                //
                Pcontext.setWSABUFFToSentbytes();

                if(WSASend(Pconnection.getAcceptSocket()->getSocketId(),
                           &Pcontext.m_wsaBuffer,
                           1,
                           &dwSendNumBytes,
                           ulFlags,
                           &Pcontext.Overlapped,
                           nullptr) == SOCKET_ERROR)
                {
                    if (WSAGetLastError() != ERROR_IO_PENDING)
                        WSA_CHECK( true, "WSASend error");
                }

            }
            else
            {
                //
                // the previous write operation completed
                // so change context to default
                Pcontext.setContextType(Context::Type::RcvFromServer);
            }
        }
        else
        {
            WSA_CHECK(false,"\nWSASend error. NULL bytes was send");
        }
    } catch (wsa_exception& excp) {
        std::cerr << excp.what();
        if (!Pconnection.Disconneting())
        {
            Pconnection.setDisctonnecting();
            DisconnectClient(Pconnection);
        }
    }
}

/**
* @brief disconenctr a client from the remote server
* @note
* @param1 current connenction
* @param2 sended quit context
* @param3 number of sended bytes
*/
void ThreadsPool::DisconnectFromRemoteServer(Connection& Pconnection,
                                             Context& Pcontext, DWORD bytes)
{
    DWORD dwIoSize=0;
    DWORD dwSendNumBytes = 0;
    ULONG ulFlags = MSG_PARTIAL;
    Pconnection.setDisctonnecting();
    Pcontext.setContextType(Context::Type::DisconnectFromServer);
    Pcontext.setStoredBytes(bytes);
    try{
        //send disconnect to client
        shutdown(Pconnection.getAcceptSocket()->getSocketId(),SD_BOTH);
        //
        // send disconnect request to remote server
        //
        Pcontext.setWSABUFFToSentbytes();
        if(WSASend(Pconnection.getConnectSocket()->getSocketId(),
                   &Pcontext.m_wsaBuffer,
                   1,
                   &dwSendNumBytes,
                   ulFlags,
                   &Pcontext.Overlapped,
                   nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
                WSA_CHECK( true, "WSASend error ");
        }
        //increment current position at receive data buffer
        Pconnection.getReceivedDataBuffer()->IncrementCurrentPos();
        //set receive context type to "DisconnectFromClient"
        Pconnection.getReceivedDataBuffer()->getCurrentPos()->setContextType(Context::Type::DisconnectClient);
        //reset wsabuff
        Pconnection.getReceivedDataBuffer()->getCurrentPos()->ResetWsaBuf();
        //post receive from remote server
        if (WSARecv(Pconnection.getConnectSocket()->getSocketId(),
                    &Pconnection.getReceivedDataBuffer()->getCurrentPos()->m_wsaBuffer,
                    1,
                    &dwIoSize,
                    &ulFlags,
                    &Pconnection.getReceivedDataBuffer()->getCurrentPos()->Overlapped,
                    nullptr) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != ERROR_IO_PENDING)
                WSA_CHECK( true, "\nWSARecv from server error");
        }
    }
    catch (wsa_exception& excp)
    {
        std::cerr << excp.what();
        DisconnectClient(Pconnection);
    }
}

/**
* @brief close and delete current connection
* @note
*/
void ThreadsPool::DisconnectClient(Connection& Pconnection)
{
    Pconnection.Disconnect();
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    while(PConncontrol->RemoveConnection(Pconnection) == -1)
            {}
}

/**
* @brief set the logger object
* @note
* @param1 pointer to the logger object
*/
void ThreadsPool::setLogger(Logger * _logger)
{
    PLogger = _logger;
}

/**
* @brief reset the logger object
* @note
*/
void ThreadsPool::resetLogger()
{
    PLogger = nullptr;
}


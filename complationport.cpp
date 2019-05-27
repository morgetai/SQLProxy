/**
  *************************************************************************************************************************
  * @file    ComplationPort.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Complation port wrapper
  **************************************************************************************************************************
*/
#include"complationport.h"
#include"exceptions.h"
#include"connection.h"
#include"mysocket.h"
#include<iostream>

/**
* @brief constructor
* @param1 number of worker threads
* @note if parameter is zero system allow to run as many
* @note concurrently threads as there are processors in the system
*/
ComplationPort::ComplationPort(uint16_t threads) try
{
    CmplPort = CreateIoCompletionPort(
                INVALID_HANDLE_VALUE,nullptr,0,threads);
    //if creation failed shutdown the program
    WSA_CHECK((CmplPort == nullptr), "CreateIoCompletionPort error ");
}
catch(wsa_exception& ex)
{
    std::cerr << ex.what();
    exit(1);
}

/*destructor*/
ComplationPort::~ComplationPort()
{
    if (CmplPort != nullptr)
    {
        free();
    }
}

/**
* @brief destructor supporting function
* @note
*/
void ComplationPort::free()
{
    CloseHandle(CmplPort);
    CmplPort = nullptr;
}

/**
* @brief update complation port with a new connection
* @param1 new connection
* @note
*/
void ComplationPort::Update(Connection* connection)
{
    std::lock_guard<std::mutex> lock(m_update);
    //check if update succeed
    WSA_CHECK( CreateIoCompletionPort(reinterpret_cast<HANDLE>(connection->getAcceptSocket()->getSocketId()),
                                      CmplPort,
                                      reinterpret_cast<DWORD_PTR>(connection), 0) == nullptr,
                                      "CompletionPort update failed ");
    WSA_CHECK( CreateIoCompletionPort(reinterpret_cast<HANDLE>(connection->getConnectSocket()->getSocketId()),
                                      CmplPort,
                                      reinterpret_cast<DWORD_PTR>(connection), 0) == nullptr,
                                      "CompletionPort update failed ");
}

/**
* @brief get pointer to a complation port
* @note
* @retval returns complation port pointer
*/
HANDLE ComplationPort::getHandle() const
{
    return CmplPort;
}

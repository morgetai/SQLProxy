/**
  *************************************************************************************************************************
  * @file    controlconnections.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Holds all connected clients
  **************************************************************************************************************************
*/
#include "controlconnections.h"
#include<algorithm>
#include"connection.h"
#include"mysocket.h"
#include<iostream>

/**
* @brief adds a new connection
* @note
* @param1 ptr to a new connection
* @retval raw pointer to the added connection
*/
Connection* ControlConnections::AddConnection(std::unique_ptr<Connection>&& client)
{
    Connection* ret = nullptr;
    int  cmp{0},val{-1};
    if (a_operationthreads.compare_exchange_strong(cmp,val))
    {
        auto insertion =
                Clients.insert(std::make_pair(reinterpret_cast<uint64_t>(&(*client)), std::move(client)));
        if (insertion.second)
        {
            ret = insertion.first->second.get();
        }
        a_operationthreads.store(0);
    }
    return ret;
}

/**
* @brief removes connection
* @note
* @param1 reference to the removed connection
* @retval -1(removing didn't started); 0(connection already removed); 1(succesfully removed)
*/
int ControlConnections::RemoveConnection(Connection& client)
{
    int ret {-1}, cmp{0},val{-1};
    if (a_operationthreads.compare_exchange_strong(cmp,val))
    {
        ConnMap::iterator it = Clients.find(reinterpret_cast<uint64_t>(&client));
        if (it != Clients.end())
        {
            CancelIoEx(reinterpret_cast<HANDLE>(it->second->getAcceptSocket()->getSocketId()),nullptr);
            CancelIoEx(reinterpret_cast<HANDLE>(it->second->getConnectSocket()->getSocketId()),nullptr);
            Clients.erase(it);
            ret = 1;
        }
        else ret = 0;
        a_operationthreads.store(0);
    }
    return ret;
}

/**
* @brief get a connection
* @note  the key of connection is its adress in memory
* @param1 pointer to the search connection
* @param2 if connection was found it stores in this pointer
* @retval -1(searching didn't started); 0(connection not found); 1(connection was found)
*/
int ControlConnections::GetConnection(Connection* LPconnection, Connection*& Pconnection)
{

    int ret {-1}, cmp{0},val{1};
    if (a_operationthreads.compare_exchange_strong(cmp,val))
    {
        ConnMap::iterator itr = Clients.find(reinterpret_cast<uint64_t>(&(*LPconnection)));
        if (Clients.end() != itr)
        {
            ret = 1;
            Pconnection = itr->second.get();
        }
        else {
            ret = 0;
            Pconnection = nullptr;
        }
        a_operationthreads.fetch_sub(1);
    }
    else if(cmp > 0)
    {
        a_operationthreads.fetch_add(1);
        ConnMap::iterator itr = Clients.find(reinterpret_cast<uint64_t>(&(*LPconnection)));
        if (Clients.end() != itr)
        {
            ret = 1;
            Pconnection = itr->second.get();
        }
        else {
            ret = 0;
            Pconnection = nullptr;
        }
        a_operationthreads.fetch_sub(1);
    }

    return ret;
}

/**
* @brief removes all connections
* @note
* @retval true if succed
*/
bool ControlConnections::CloseAllConnections()
{
    bool ret = false;
    int  cmp{0},val{1};
    if (a_operationthreads.compare_exchange_strong(cmp,val))
    {
        ret = true;
        if (!Clients.empty())
        {

            std::for_each(Clients.begin(),Clients.end(),[&](typename ConnMap::value_type& elem)
            {
                CancelIoEx(reinterpret_cast<HANDLE>(elem.second->getAcceptSocket()->getSocketId()),
                           nullptr);
                CancelIoEx(reinterpret_cast<HANDLE>(elem.second->getConnectSocket()->getSocketId()),
                         nullptr);
                Clients.erase(elem.first);
            });

        }      
    }
    return ret;
}

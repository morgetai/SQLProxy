/**
  *************************************************************************************************************************
  * @file    controlconnections.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Holds all connected clients
  **************************************************************************************************************************
*/
#ifndef CONTROLCONNECTIONS_H
#define CONTROLCONNECTIONS_H
#include<map>
#include<memory>
#include<atomic>
#include<mutex>
#include<WinSock2.h>
#include<condition_variable>



class Connection;

class ControlConnections
{
public:
    /*constructor*/
    explicit ControlConnections() = default;
    /**/
    ControlConnections(const ControlConnections&) = delete;
    /**/
    ControlConnections(ControlConnections&&) = delete;
    /**/
    ControlConnections& operator=(const ControlConnections&) = delete;
    /**/
    ControlConnections& operator=(ControlConnections&&) = delete;
    /*destructor*/
    ~ControlConnections() = default;

    /**
    * @brief adds a new connection
    * @note
    * @param1 ptr to a new connection
    * @retval raw pointer to the added connection
    */
    Connection* AddConnection(std::unique_ptr<Connection>&& client);

    /**
    * @brief get a connection
    * @note  the key of connection is its adress in memory
    * @param1 pointer to the search connection
    * @param2 if connection was found it stores in this pointer
    * @retval -1(searching didn't started); 0(connection not found); 1(connection was found)
    */
    int GetConnection(Connection*, Connection*&);

    /**
    * @brief removes connection
    * @note
    * @param1 reference to the removed connection
    * @retval -1(removing didn't started); 0(connection already removed); 1(succesfully removed)
    */
    int RemoveConnection(Connection&);

    /**
    * @brief removes all connections
    * @note
    * @retval true if succed
    */
    bool CloseAllConnections();

private:

    typedef std::map<uint64_t,std::unique_ptr<Connection>> ConnMap;
    /*map of connected clients*/
    ConnMap Clients;
    /*number of threads which are serching in the map. -1 if removing or adding is performed*/
    std::atomic<int> a_operationthreads = {0};



};

#endif // CONTROLCONNECTIONS_H

/**
  *************************************************************************************************************************
  * @file    ComplationPort.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Complation port wrapper
  **************************************************************************************************************************
*/
#ifndef COMPLATIONPORT_H
#define COMPLATIONPORT_H

#include<WinSock2.h>
#include<memory>
#include<mutex>
#include"connection.h"


class ComplationPort
{
public:
    /**
    * @brief constructor
    * @param1 number of worker threads
    * @note if parameter is zero system allow to run as many
    * @note concurrently threads as there are processors in the system
    */
    explicit ComplationPort(uint16_t threads = 0);
    /**/
    ComplationPort(const ComplationPort&) = delete;
    /**/
    ComplationPort(ComplationPort&&) = delete;
    /**/
    ComplationPort& operator=(const ComplationPort&) = delete;
    /*destructor*/
    ~ComplationPort();
    /**
    * @brief update complation port with a new connection
    * @param1 new connection
    * @note
    */
    void Update(Connection*);
    /**
    * @brief get pointer to a complation port
    * @note
    * @retval returns complation port pointer
    */
    HANDLE getHandle() const;
private:
    /**
    * @brief destructor supporting function
    * @note
    */
    void free();
    /*pointer to the created complation port*/
    HANDLE CmplPort;
    /*mutex for complation port update function*/
    std::mutex m_update;

};

#endif // COMPLATIONPORT_H

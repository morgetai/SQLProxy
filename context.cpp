/**
  *************************************************************************************************************************
  * @file    context.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Holds the type and data of a current IO operation
  **************************************************************************************************************************
*/
#include "context.h"
#include"exceptions.h"
#include"iostream"
#include <malloc.h>

/**
* @brief default constructor
*/
Context::Context() try:
 m_data(4096)
{
    /*if new chanck of memory could not be allocated throw exception*/
    CHECK(m_data.empty(), "Couldn't create new context");
    ResetWsaBuf();
    /* Clear out the overlapped struct.
     Otherwise the overlap object will be rejected*/
    memset(&Overlapped, 0, sizeof(OVERLAPPED));

}
catch(some_exception& ex)
{
    std::cerr<<ex.what();
}

/**
* @brief resets m_wsaBuffer
* @note  Should be called each time the context is used.
*/
void Context::ResetWsaBuf()
{
    m_wsaBuffer.buf = reinterpret_cast<char *>(m_data.data());
    m_wsaBuffer.len = static_cast<u_long>(
        m_data.size() * sizeof(m_data[0])
        );
}

/**
* @brief get pointer to the actual data
* @note
* @retval pointer to a head of vector of data
*/
uint8_t* Context::getBuff()
{
    return m_data.data();
}

/**
* @brief set context type
* @note
* @param1 new context type
*/
void Context::setContextType(Context::Type new_type)
{
    m_type = new_type;
}

/**
* @brief get context type
* @note
* @retval  enum class Type
*/
Context::Type Context::getContextType()
{
    return m_type;
}

/**
* @brief save the number of received bytes
* @note
*/
void Context::setStoredBytes(uint32_t bytes)
{
    bytes_sent = 0;
    bytes_stored = bytes;
}

/**
* @brief get the number of stored bytes
* @note
* @retval number of stored bytes
*/
uint32_t Context::getStoredBytes() const
{
    return bytes_stored;
}


/**
* @brief sets the m_wsaBuffer to a number of sended bytes
* @note  used in send process. if send operation didn't send all the data
* @note  this fucntion helps to send the rest of it
*/
void Context::setWSABUFFToSentbytes()
{
    m_wsaBuffer.buf = reinterpret_cast<char *>(m_data.data()+bytes_sent);
    m_wsaBuffer.len = static_cast<u_long>((bytes_stored - bytes_sent) * sizeof(m_data[0]));
}

/**
* @brief add sended bytes
* @note
* @retval true if sent bytes is greater or equal to the stored bytes
*/
bool Context::addSentBytes(uint32_t bytes)
{
    bool ret = false;
    if ((bytes_sent += bytes) >= bytes_stored)
        ret = true;
    return ret;
}

/**
* @brief get sent bytes
* @note
* @retval sent bytes
*/
uint32_t Context::getSentBytes() const
{
    return bytes_sent;
}


/**
  *************************************************************************************************************************
  * @file    exceptions.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   winsock exception wrappepr
  **************************************************************************************************************************
*/
#include"exceptions.h"
#include<WinSock2.h>
#include<cstdio>

/*contructor*/
wsa_exception::wsa_exception():
      std::exception(get_message())
    { }
/*contructor*/
wsa_exception::wsa_exception(const char* prefix):
        std::exception(get_message(prefix))
    { }

/**
* @brief get message from buffer with added prefix
* @note
* @param1 prefix which is added to an error message
* @retval pointer to the buffer
*/
const char* wsa_exception::get_message(const char* prefix)
{
    sprintf(_message,"\nWinSock Error: %s. Error code: %d",prefix,WSAGetLastError());
    return _message;
}

/*contructor*/
some_exception::some_exception(const char* message):
    std::exception(message) {}

/**
* @brief throws exception if cond is true
* @note
* @param1 checked condition
* @param2 appended to the error message
*/
void WSA_CHECK(bool condition, const char* message)
{
    if (condition)
        throw wsa_exception(message);
}

/**
* @brief throws exception if cond is true
* @note
* @param1 checked condition
* @param2 error message
*/
void CHECK(bool condition, const char* message)
{
    if (condition)
        throw some_exception(message);
}

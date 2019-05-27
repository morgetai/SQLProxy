/**
  *************************************************************************************************************************
  * @file    exceptions.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   winsock exception wrappepr
  **************************************************************************************************************************
*/
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include<exception>


class wsa_exception : public std::exception
{
public:
    /*conctructors*/
    explicit wsa_exception();
    wsa_exception(const char* prefix);

private:
    /**
    * @brief get message from buffer with added prefix
    * @note
    * @param1 prefix which is added to an error message
    * @retval pointer to the buffer
    */
    const char* get_message(const char* prefix = "");
    /*buffer*/
    char _message[1024]{};
};


class some_exception : public std::exception
{
public:
    /*conctructors*/
    some_exception(const char* message);
};

/**
* @brief throws exception if cond is true
* @note
* @param1 checked condition
* @param2 appended to the error message
*/
void WSA_CHECK(bool condition, const char* message);

/**
* @brief throws exception if cond is true
* @note
* @param1 checked condition
* @param2 error message
*/
void CHECK(bool condition, const char* message);

#endif // EXCEPTIONS_H

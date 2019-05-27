/**
  *************************************************************************************************************************
  * @file    buffer.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Buffer for network data
  **************************************************************************************************************************
*/

#include"buffer.h"
#include"context.h"
#include"exceptions.h"
#include<iostream>
#include<algorithm>

/**
* @brief constructor
* @param1 context type
* @param2 buffer size
*/
Buffer::Buffer(Context::Type newtype,unsigned buff_size):
    buffer(buff_size)
{
    //set context type
    std::for_each(buffer.begin(),buffer.end(),[&](Context& context)
    {
        context.setContextType(newtype);
    });
    /*if new chanck of memory could not be allocated throw exception*/
    try {
        CHECK(buffer.empty(), " Couldn't create new buffer ");
    } catch (some_exception& ex) {
        std::cerr << ex.what();
    }
}

/**
* @brief get a current context on which operation is performed
* @note
* @retval pointer to a current context
*/
Context* Buffer::getCurrentPos()
{
    if (!buffer.empty())
    {
        return &buffer[a_currentpos.load()];
    }
    return nullptr;
}

/**
* @brief increment current position
* @note
*/
void Buffer::IncrementCurrentPos()
{
    if (buffer.size() - 1 != a_currentpos.load())
    {
        a_currentpos.fetch_add(1);
    }
    else
    {
        a_currentpos.store(0);
    }
}

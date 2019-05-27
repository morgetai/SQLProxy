/**
  *************************************************************************************************************************
  * @file    buffer.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Buffer for network data
  **************************************************************************************************************************
*/


#ifndef BUFFER_H
#define BUFFER_H
#include<vector>
#include<memory>
#include<atomic>
#include"context.h"


class Buffer
{
public:
    /**
    * @brief constructor
    * @param1 context type
    * @param2 buffer size
    */
    Buffer(Context::Type,unsigned);
    /**/
    Buffer(const Buffer&) = delete;
    /**/
    Buffer(Buffer&&) = delete;
    /**/
    Buffer& operator=(const Buffer&) = delete;
    /**/
    ~Buffer() = default;

    /**
    * @brief get a current context on which operation is performed
    * @note
    * @retval pointer to a current context
    */
    Context* getCurrentPos();
    /**
    * @brief increment current position
    * @note
    */
    void IncrementCurrentPos();


private:
    /*buffer in which data is stored*/
    std::vector<Context> buffer;
    /*position of working context*/
    std::atomic<size_t> a_currentpos{0};

};

#endif // WRITEBUFFER_H

/**
  *************************************************************************************************************************
  * @file    logger.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   logging object
  **************************************************************************************************************************
*/
#include "logger.h"
#include"mysocket.h"
#include"exceptions.h"
#include<algorithm>
#include<chrono>
#include<ctime>
#include<string>
#include<algorithm>

/*constructor*/
Logger::Logger():
    data(BUFFSSIZE),
    file("log.txt",std::ofstream::out),
    logforPos(LOGQTY - 1)
{
    /*if buffer is not criated throw an exception*/
    try {
        CHECK(data.empty(), " Couldn't create logger ");
    } catch (some_exception& ex) {
        std::cerr << ex.what();
    }
}

/**
* @brief provides the writing to a file
* @note  this function is called in main and
* @note  does the actual logging
*/
void Logger::ActualLogging()
{
    size_t push = a_pushPos.load();
    size_t threads = a_writingthreads.load();
    if (!threads &&  (( push > logforPos ) || rewritten))
    {
        if (file.is_open())
        {
            for (;loggingPos < logforPos; ++loggingPos)
            {
                /*write data to the file*/
                file.write(data[loggingPos].getData(),data[loggingPos].getDatalen());
            }
            file.flush();
        }

        if (data.size() - 1  == logforPos)
        {
            /*if buffer is rewritten*/
            logforPos = LOGQTY - 1;
            loggingPos = 0;
            rewritten = false;
        }
        else {
            logforPos += LOGQTY;
        }
    }
}

/**
* @brief add some data to be logged
* @note
* @param1 logged query
* @param2 socket
*/
void Logger::AddToLogging(const std::string& query,mySocket* currentsocket)
{
    size_t pos = a_pushPos.load();
    a_writingthreads.fetch_add(1);
    if (data.size() - 1 == pos)
    {
        a_pushPos.store(0);
        rewritten = true;
    }
    else
    {
        a_pushPos.fetch_add(1);
    }
    data[pos].CopyToBuff(query,currentsocket);
    a_writingthreads.fetch_sub(1);
}


/*contructor*/
Logger::Buffer::Buffer()
{
    buff.resize(BuffSize);
    try {
        CHECK(buff.empty(), " Couldn't create new buffer for logging ");
    } catch (some_exception& ex) {
        std::cerr << ex.what();
    }
}

/**
* @brief get pointer tp a buffer
* @note
* @retval raw pointer
*/
const char* Logger::Buffer::getData()
{
    return buff.c_str();
}

/**
* @brief get number of bytes saved in buffer
* @note
* @retval number of bytes
*/
size_t Logger::Buffer::getDatalen()
{
    return buff.size();
}

/**
* @brief add some data to be logged
* @note
* @param1 logged query
* @param2 socket
*/
void Logger::Buffer::CopyToBuff(const std::string& query, mySocket* currentsocket)
{
    /*clear buff*/
   buff.clear();
   /*get current time*/
   std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
   ctime_s( time, TimeDatalen, &t );
   buff = static_cast<const char*>(time) + query + " from IP:" + currentsocket->getAddrstr() + "\n";
}

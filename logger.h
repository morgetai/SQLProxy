/**
  *************************************************************************************************************************
  * @file    logger.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   logging object
  **************************************************************************************************************************
*/
#ifndef LOGGER_H
#define LOGGER_H
#include<iostream>
#include<fstream>
#include<atomic>
#include<array>
#include<vector>
#include<string>

class mySocket;


class Logger
{
public:
    /*constructor*/
    explicit Logger();
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    /*destructor*/
    ~Logger()
    {file.close();}

    /**
    * @brief add some data to be logged
    * @note
    * @param1 logged query
    * @param2 socket
    */
    void AddToLogging(const std::string&,mySocket*);

    /**
    * @brief provides the writing to a file
    * @note  this function is called in main and
    * @note  does the actual logging
    */
    void ActualLogging();
private:
    class Buffer;
    /*buffer size*/
    static const size_t BUFFSSIZE = 100;
    /*number of elements to be written for a time*/
    static const size_t LOGQTY = 20;
    /*buffer*/
    std::vector<Buffer> data;
    /*log file*/
    std::ofstream file;
    /*current logging position in buffer*/
    size_t loggingPos{0};
    /*after reaching this position writing to a file is performed*/
    size_t logforPos;
    /*empty position in buffer*/
    std::atomic<size_t> a_pushPos{0};
    /*number of writing threds*/
    std::atomic<uint16_t> a_writingthreads{0};
    /*true if buffer is rewritten*/
    bool rewritten{false};
};

/*buffer implementation*/
class Logger::Buffer
{
public:
    /*contructor*/
    Buffer();
    Buffer(const Buffer&) = delete;
    Buffer(Buffer&&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    /*destructor*/
    ~Buffer() = default;

    /**
    * @brief add some data to be logged
    * @note
    * @param1 logged query
    * @param2 socket
    */
    void CopyToBuff(const std::string&,mySocket*);

    /**
    * @brief get pointer tp a buffer
    * @note
    * @retval raw pointer
    */
    const char* getData();

    /**
    * @brief get number of bytes saved in buffer
    * @note
    * @retval number of bytes
    */
    size_t getDatalen();
private:
    /*number of bytes in buffer*/
    static const size_t BuffSize = 256;
    /*length of time string*/
    static const size_t TimeDatalen = 28;
    /*actual buffer*/
    std::string buff;
    /*buffer for time and date*/
    char time[TimeDatalen]{};
};
#endif // LOGGER_H

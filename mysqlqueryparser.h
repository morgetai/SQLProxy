/**
  *************************************************************************************************************************
  * @file    mysqlparser.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   parser for sql querys
  **************************************************************************************************************************
*/
#ifndef MYSQLQYERYPARSER_H
#define MYSQLQYERYPARSER_H
#include<cstdint>
#include<string>



class Context;
class MySQLQueryParser
{
public:
    /*constructor*/
    explicit MySQLQueryParser() = default;

    /**
    * @brief returns type of a current received command
    * @note
    * @param1 received context
    * @retval type of operation
    */
    uint8_t GetCommandType(Context&);

    /**
    * @brief returns a command text
    * @note
    * @param1 received context
    * @retval command string
    */
    std::string GetQuery(Context&);
    /*type of sql packet*/
    static const uint8_t COM_QUERY = 3;
    /*type of sql packet*/
    static const uint8_t COM_STMT_PREPARE = 22;
    /*type of sql packet*/
    static const uint8_t COM_QUIT  = 1;
    //static const uint8_t SQL_BATCH = 1;
};

#endif // MYSQLQYERYPARSER_H

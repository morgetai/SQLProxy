/**
  *************************************************************************************************************************
  * @file    mysqlparser.cpp
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   parser for sql querys
  **************************************************************************************************************************
*/
#include "mysqlqueryparser.h"
#include "context.h"
#include <vector>
#include<numeric>
#include<algorithm>
#include<iostream>

/**
* @brief returns type of a current received command
* @note
* @param1 received context
* @retval type of operation
*/
uint8_t MySQLQueryParser::GetCommandType(Context& context)
{
    uint8_t ret = 0;
    if (context.getStoredBytes() > 4)
    {
        ret = *(context.getBuff() + 4) ;
    }
    return ret;
}

/**
* @brief returns a command text
* @note
* @param1 received context
* @retval command string
*/
std::string MySQLQueryParser::GetQuery(Context& context)
{

    return std::string(std::next(context.m_data.begin(), 5),
                           std::next(context.m_data.begin(), static_cast<int>(context.getStoredBytes())));
}

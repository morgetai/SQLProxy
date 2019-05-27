/**
  *************************************************************************************************************************
  * @file    context.h
  * @author  Hridin Andrii
  *          abgridin@gmail.com
  * @date    27-May-2019
  * @brief   Holds the type and data of a current IO operation
  **************************************************************************************************************************
*/
#ifndef CONTEXT_H
#define CONTEXT_H
#include<vector>
#include<memory>
#include<WinSock2.h>

class MySQLQueryParser;

class Context
{
    friend class MySQLQueryParser;
public:
    enum class Type
    {
        RcvFromClient,
        SendToClient,
        RcvFromServer,
        SendToServer,
        DisconnectFromServer,
        DisconnectClient
    };
  /**
  * @brief default constructor
  */
  explicit Context();
  /**/
  ~Context() = default;
  /**/
  Context(const Context&) = delete;
  /**/
  Context(Context&&) = delete;
  /**/
  Context& operator=(const Context&) = delete;
  /**/
  Context& operator=(Context&&) = delete;
  /*used in overlapped I/O operation*/
  OVERLAPPED Overlapped;
  /*this buffer is used in SWASend/WSARecv*/
  WSABUF m_wsaBuffer;

  /**
  * @brief get context type
  * @note
  * @retval  enum class Type
  */
  Type getContextType();

  /**
  * @brief set context type
  * @note
  * @param1 new context type
  */
  void setContextType(Type);

  /**
  * @brief get pointer to the actual data
  * @note
  * @retval pointer to a head of vector of data
  */
  uint8_t* getBuff();

  /**
  * @brief save the number of received bytes
  * @note
  */
  void setStoredBytes(uint32_t);

  /**
  * @brief get the number of stored bytes
  * @note
  * @retval number of stored bytes
  */
  uint32_t getStoredBytes() const;

  /**
  * @brief sets the m_wsaBuffer to a number of sended bytes
  * @note  used in send process. if send operation didn't send all the data
  * @note  this fucntion helps to send the rest of it
  */
  void setWSABUFFToSentbytes();

  /**
  * @brief add sended bytes
  * @note
  * @retval true if sent bytes is greater or equal to the stored bytes
  */
  bool addSentBytes(uint32_t);

  /**
  * @brief get sent bytes
  * @note
  * @retval sent bytes
  */
  uint32_t getSentBytes() const;

  /**
  * @brief resets m_wsaBuffer
  * @note  Should be called each time the context is used.
  */
  void ResetWsaBuf();

protected:
  /*type of context*/
  Type m_type{Type::RcvFromClient};

  /*the actual buffer that holds the data*/
  std::vector<uint8_t> m_data;
  /*number of sended bytes*/
  uint32_t bytes_sent{0};
  /*number of stored bytes*/
  uint32_t bytes_stored{0};
};






#endif // CONTEXT_H

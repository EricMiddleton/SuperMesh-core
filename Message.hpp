#pragma once

#include <vector>
#include <cstdint>

#include <memory>

#include "types.hpp"

namespace OLSR {
  class Packet;
  class MessageData;

  class Message {
  public:
    //From 18.4 of RFC3626
    enum class Type {
      Hello = 1,
      TC = 2,
      MID = 3,
      HNA = 4,
      User = 5
    };

    //Construct from existing message (no copy)
    Message(uint8_t* data, uint16_t length, const Packet* p = nullptr);

    //Construct with specific message data (will be copied). Object will own data
    //Generic message parameters will be undefined until set with setter methods
    Message(const MessageData& data, const Packet* p = nullptr);

    ~Message();

    operator bool() const;

    Type messageType() const;
    void messageType(Type t);

    uint8_t vTime() const;
    void vTime(uint8_t t);

    uint16_t messageSize() const;
    void messageSize(uint16_t size);
    
    uint32_t originatorAddr() const;
    void originatorAddr(uint32_t addr);

    uint8_t ttl() const;
    void ttl(uint8_t ttl);

    uint8_t hopCount() const;
    void hopCount(uint8_t hopCount);

    uint16_t seqNum() const;
    void seqNum(uint16_t seqNum);

    MessageData* payload();
    const MessageData* payload() const;

    const Packet* packet() const;

    const uint8_t* begin() const;
    const uint8_t* end() const;

  private:
    friend class MessageData;

    const Packet* m_p;
    uint8_t* m_data;
    std::unique_ptr<MessageData> m_messageData;

    bool m_ownData;
  };
}

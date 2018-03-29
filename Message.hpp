#pragma once

#include <cstdint>

namespace OLSR {
  class Message {
  public:
    Message(uint8_t* data, uint16_t length);

    operator bool() const;

    uint8_t messageType() const;
    uint8_t vTime() const;
    uint16_t messageSize() const;
    
    uint32_t originatorAddr() const;

    uint8_t ttl() const;
    uint8_t hopCount() const;
    uint16_t seqNum() const;

  private:
    uint8_t* m_data;
  };
}
#pragma once

#include <cstdint>

namespace OLSR {
  class Message;

  class Packet {
  public:
    Packet(uint8_t* packet, uint16_t length);

    operator bool() const;

    uint16_t length() const;
    uint16_t seqNum() const;

    Message& message();
    const Message& message() const;

  private:
    uint8_t* m_data;
    uint16_t m_length;
  };
}
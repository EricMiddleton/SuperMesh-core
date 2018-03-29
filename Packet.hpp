#pragma once

#include <vector>
#include <cstdint>

#include "Message.hpp"

namespace OLSR {
  class Packet {
  public:
    using iterator = std::vector<Message>::iterator;
    using const_iterator = std::vector<Message>::const_iterator;

    Packet(uint8_t* packet, uint16_t length);

    operator bool() const;

    uint16_t length() const;
    uint16_t seqNum() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

  private:
    uint8_t* m_data;

    std::vector<Message> m_messages;
  };
}
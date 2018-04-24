#pragma once

#include <vector>
#include <cstdint>

#include "Message.hpp"
#include "types.hpp"

namespace OLSR {
  class Packet {
  public:
    using iterator = std::vector<Message>::iterator;
    using const_iterator = std::vector<Message>::const_iterator;

    Packet(uint8_t* packet, uint16_t length, Address sendInterface, Address recvInterface,
      Address localNodeAddress);

    operator bool() const;

    uint16_t length() const;
    uint16_t seqNum() const;

    Address sendInterface() const;
    Address recvInterface() const;
    Address localNodeAddress() const;

    iterator begin();
    const_iterator begin() const;

    iterator end();
    const_iterator end() const;

  private:
    uint8_t* m_data;

    Address m_sendInterface, m_recvInterface, m_localNodeAddress;

    std::vector<Message> m_messages;
  };
}

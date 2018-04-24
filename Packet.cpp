#include "Packet.hpp"
#include "util.hpp"

#include <iostream>

namespace OLSR {

Packet::Packet(uint8_t* packet, uint16_t length, Address sendInterface, Address recvInterface,
  Address localNodeAddress)
  : m_data{nullptr}
  , m_sendInterface{sendInterface}
  , m_recvInterface{recvInterface}
  , m_localNodeAddress{localNodeAddress} {
  
  if(length >= 16) {
    uint16_t remainder = length - 4;
    uint8_t* ptr = packet + 4;
   
    while(remainder > 0) {
      //Attempt to parse message, store into message vector
      m_messages.push_back({ptr, remainder, this});

      if(m_messages.back()) {
        //Valid message, advance to start of next (if exists)
        auto msgLen = m_messages.back().messageSize();
        remainder -= msgLen;
        ptr += msgLen;
      }
      else {
        //Invalid message, stop parsing packet
        break;
      }
    }

    if(remainder == 0) {
      //Packet parsing successful
      m_data = packet;
    }
    else {
      //Packet parsing unsucessful, clear message storage
      m_messages.clear();
    }
  }
}

Packet::operator bool() const {
  return m_data != nullptr;
}

uint16_t Packet::length() const {
  return util::parseU16(m_data);
}

uint16_t Packet::seqNum() const {
  return util::parseU16(m_data + 2);
}

Address Packet::sendInterface() const {
  return m_sendInterface;
}

Address Packet::recvInterface() const {
  return m_recvInterface;
}

Address Packet::localNodeAddress() const {
  return m_localNodeAddress;
}

Packet::iterator Packet::begin() {
  return m_messages.begin();
}
Packet::const_iterator Packet::begin() const {
  return m_messages.begin();
}

Packet::iterator Packet::end() {
  return m_messages.end();
}

Packet::const_iterator Packet::end() const {
  return m_messages.end();
}

}

#include "Packet.hpp"
#include "util.hpp"

#include <iostream>

OLSR::Packet::Packet(uint8_t* packet, uint16_t length)
  : m_data{nullptr} {
  
  if(length >= 16) {
    uint16_t remainder = length - 4;
    uint8_t* ptr = packet + 4;
   
    while(remainder > 0) {
      //Attempt to parse message, store into message vector
      m_messages.emplace_back(ptr, remainder);

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

OLSR::Packet::operator bool() const {
  return m_data != nullptr;
}

uint16_t OLSR::Packet::length() const {
  return OLSR::util::parseU16(m_data);
}

uint16_t OLSR::Packet::seqNum() const {
  return OLSR::util::parseU16(m_data + 2);
}

OLSR::Packet::iterator OLSR::Packet::begin() {
  return m_messages.begin();
}
OLSR::Packet::const_iterator OLSR::Packet::begin() const {
  return m_messages.begin();
}

OLSR::Packet::iterator OLSR::Packet::end() {
  return m_messages.end();
}

OLSR::Packet::const_iterator OLSR::Packet::end() const {
  return m_messages.end();
}
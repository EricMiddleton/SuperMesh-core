#include "Packet.hpp"
#include "util.hpp"

OLSR::Packet(uint8_t* packet, uint16_t length) {
  
  if(m_length >= 4) {
    m_data = packet;
    m_length = length;
  }
  else {
    m_data = nullptr;
  }
}

OLSR::Packet::operator bool() const {
  return m_data != nullptr;
}

uint16_t OLSR::Packet::length() const {
  return parseU16(m_data + 0);
}

uint16_t OLSR::Packet::seqNum() const {
  return parseU16(m_data + 2);
}

OLSR::Message& OLSR::Packet::message() const {

}
#include "Message.hpp"

#include "util.hpp"

#include <iostream>

OLSR::Message::Message(uint8_t* data, uint16_t length) {
  if(length < 12) {
    //Too short to contain message header
    m_data = nullptr;
  }
  else {
    m_data = data;

    auto messageLength = messageSize();
    if(messageLength < 12) {
      m_data = nullptr;
    }
    else if(length < messageSize()) {
      //Too short to contain full message (based on message header)
      m_data = nullptr;
    }
  }
}

OLSR::Message::operator bool() const {
  return m_data != nullptr;
}

uint8_t OLSR::Message::messageType() const {
  return OLSR::util::parseU8(m_data);
}

uint8_t OLSR::Message::vTime() const {
  return OLSR::util::parseU8(m_data + 1);
}

uint16_t OLSR::Message::messageSize() const {
  return OLSR::util::parseU16(m_data + 2);
}

uint32_t OLSR::Message::originatorAddr() const {
  return OLSR::util::parseU32(m_data + 4);
}

uint8_t OLSR::Message::ttl() const {
  return OLSR::util::parseU8(m_data + 8);
}

uint8_t OLSR::Message::hopCount() const {
  return OLSR::util::parseU8(m_data + 9);
}

uint16_t OLSR::Message::seqNum() const {
  return OLSR::util::parseU16(m_data + 10);
}
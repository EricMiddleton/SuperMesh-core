#include "Message.hpp"

#include "util.hpp"

#include <iostream>

namespace OLSR {

Message::Message(const Packet* p, uint8_t* data, uint16_t length)
  : m_p{p}
  , m_ownData{false} {
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
    else {
      m_messageData = std::make_unique<MessageData>(m_data, messageLength - 12);
    }
  }
}

Message::Message(const MessageData& data, const Packet* p)
  : m_p{p}
  , m_data{new uint8_t[data.size() + 12]}
  , m_messageData{std::make_unique<MessageData>(m_data, data.size())}
  , m_ownData{true} {

  std::memcpy(m_data + 12, data.raw(), data.size());
}

Message::~Message() {
  if(m_ownData && m_data != nullptr) {
    delete[] m_data;
  }
}

Message::operator bool() const {
  return m_data != nullptr;
}

Message::Type Message::messageType() const {
  return static_cast<Type>(util::parseU8(m_data));
}

void Message::messageType(Type t) {
  m_data[0] = static_cast<uint8_t>(t);
}

uint8_t Message::vTime() const {
  return util::parseU8(m_data + 1);
}

void Message::vTime(uint8_t t) {
  m_data[1] = t;
}

uint16_t Message::messageSize() const {
  return util::parseU16(m_data + 2);
}

void Message::messageSize(uint16_t size) {
  util::pack16(m_data+2, size);
}

uint32_t Message::originatorAddr() const {
  return util::parseU32(m_data + 4);
}

void Message::originatorAddr(uint32_t addr) {
  util::pack32(m_data + 4, addr);
}

uint8_t Message::ttl() const {
  return util::parseU8(m_data + 8);
}

void Message::ttl(uint8_t ttl) {
  m_data[8] = ttl;
}

uint8_t Message::hopCount() const {
  return util::parseU8(m_data + 9);
}

void Message::hopCount(uint8_t hopCount) {
  m_data[9] = hopCount;
}

uint16_t Message::seqNum() const {
  return util::parseU16(m_data + 10);
}

void Message::seqNum(uint16_t seqNum) {
  util::packU16(m_data + 10, seqNum);
}

MessageData* Message::payload() {
  return m_messageData;
}

const MessageData* Message::payload() {
  return m_messageData;
}

const uint8_t* Message::payload() const {
  return m_data + 12;
}

uint16_t Message::payloadLength() const {
  return messageSize() - 12;
}

const Packet* Message::packet() const {
  return m_p;
}

void Message::copy(uint8_t* out) const {
  std::memcpy(out, m_data, messageSize());
}

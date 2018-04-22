#include "MessageData.hpp"

namespace OLSR {

MessageData::MessageData(uint8_t* data, uint16_t size)
  : m_data{data}
  , m_size{size}
  , m_ownData{false} {
}

MessageData::MessageData(uint16_t size)
  : m_data{new uint8_t[size]}
  , m_size{size}
  , m_ownData{true} {
}

MessageData::~MessageData() {
  if(m_ownData) {
    delete[] m_data;
  }
}

uint8_t* MessageData::raw() {
  return m_data;
}

const uint8_t* MessageData::raw() const {
  return m_data;
}

uint16_t MessageData::size() const {
  return m_size;
}

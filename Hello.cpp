#include "Hello.hpp"

#include "util.hpp"

namespace OLSR {

Hello::Hello(uint8_t htime, uint8_t willingness, const std::vector<LinkInfo>& links)
  : MessageData(4 + linkInfoSize(links)) {

  m_data[2] = htime;
  m_data[3] = willingness;
  
  int index = 4;

  for(const auto& link : links) {
    m_data[index] = link.linkCode;
    util::packU16(m_data + index + 2, 4 + 4*link.neighborInterfaces.size());
    index += 4;

    for(const auto& interface : link.neighborInterfaces) {
      util::packU32(m_data + index, interface);
      index += 4;
    }
  }
}

uint8_t Hello::htime() const {
  return m_data[2];
}

void Hello::htime(uint8_t htime) {
  m_data[2] = htime;
}

uint8_t Hello::willingness() const {
  return m_data[3];
}

void Hello::willingness(uint8_t willingness) {
  m_data[3] = willingness;
}

int Hello::countLinks() const {
  int count = 0;

  for(int i = 4; i < m_size; ) {
    count++;

    uint16_t linkMsgSize = util::parseU16(m_data+i+2);
    i += linkMsgSize;
  }

  return count;
}

LinkInfo Hello::getLinkInfo(int index) const {
  LinkInfo info;

  int ptr = 4;
  int linkMsgSize = 0;
  for(int count = 0; count < index; ++count) {
    linkMsgSize = util::parseU16(m_data+ptr+2);
    ptr += linkMsgSize;
  }

  info.linkCode = m_data[ptr];
  for(int i = 0; i < (linkMsgSize-4)/4; ++i) {
    info.neighborInterfaces.push_back(util::parseU32(m_data+index+4+4*i));
  }

  return info;
}

int Hello::linkInfoSize(const std::vector<LinkInfo>& links) {
  int size = 0;
  for(const auto& link : links) {
    size += 4 + 4*link.neighborInterfaces.size();
  }

  return size;
}

}

#pragma once

#include "Message.hpp"

namespace OLSR {
  class MessageData {
  public:
    //Construct from existing data. No copy, does not own data
    MessageData(uint8_t* data, uint16_t size);
    
    //Construct and allocate own memory of given size
    MessageData(uint16_t size);

    virtual ~MessageData();
    
    uint8_t* raw();
    const uint8_t* raw() const;
    uint16_t size() const;
  protected:
    uint8_t* m_data;
    uint16_t m_size;

    bool m_ownData;
  };
}

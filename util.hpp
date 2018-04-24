#pragma once

#include <cstdint>

namespace OLSR {
  namespace util {
    uint8_t parseU8(const uint8_t* ptr);
    uint16_t parseU16(const uint8_t* ptr);
    uint32_t parseU32(const uint8_t* ptr);

    void packU16(uint8_t* ptr, uint16_t value);
    void packU32(uint8_t* ptr, uint32_t value);

    uint32_t vtimeToMillis(uint8_t vtime);
    uint8_t millisToVtime(uint32_t millis);
  }
}

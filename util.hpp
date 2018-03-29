#pragma once

#include <cstdint>

namespace OLSR {
  namespace util {
    uint8_t parseU8(const uint8_t* ptr);
    uint16_t parseU16(const uint8_t* ptr);
    uint32_t parseU32(const uint8_t* ptr);
  }
}
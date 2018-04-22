#include "util.hpp"

uint8_t OLSR::util::parseU8(const uint8_t* ptr) {
  return *ptr;
}

uint16_t OLSR::util::parseU16(const uint8_t* ptr) {
  return (*ptr) | ((*(ptr+1)) << 8);
}

uint32_t OLSR::util::parseU32(const uint8_t* ptr) {
  return (*ptr) | ((*(ptr+1)) << 8) | ((*(ptr+2)) << 16) | ((*(ptr+3)) << 24);
}

void OLSR::util::packU16(uint8_t* ptr, uint16_t value) {
  *ptr = value & 0xFF;
  *(ptr+1) = (value >> 8);
}

uint32_t OLSR::util::vtimeToMillis(uint8_t vtime) {
  uint8_t mantissa = (vtime >> 8) & 0x0F,
    exponent = vtime & 0xFF;

  uint32_t millis = ((1000U*mantissa)/16) << exponent;
  millis /= 16; //C factor = 1/16s

  return millis;
}

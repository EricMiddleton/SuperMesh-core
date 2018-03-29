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
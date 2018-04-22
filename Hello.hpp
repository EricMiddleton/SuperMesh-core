#pragma once

#include "MessageData.hpp"

namespace OLSR {
  class Hello : public MessageData {
  public:

    Hello(uint8_t htime, uint8_t willingness, const std::vector<LinkInfo>& links);

    uint8_t htime() const;
    void htime(uint8_t htime);

    uint8_t willingness() const;
    void willingness(uint8_t willingness);

    int countLinks() const;
    LinkInfo getLinkInfo(int index) const;

    std::vector<Address> neighborAddresses() const;
  private:
    static int linkInfoSize(const std::vector<LinkInfo>& links);
  };
}

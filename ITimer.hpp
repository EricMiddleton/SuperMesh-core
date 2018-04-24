#pragma once

#include "types.hpp"

namespace OLSR {
  class ITimer {
  public:
    virtual ~ITimer() {}

    virtual void setPeriod(TimeValue millis) = 0;
    virtual void setFn(const TimerFn& fn) = 0;
    virtual void start() = 0;
  };
}

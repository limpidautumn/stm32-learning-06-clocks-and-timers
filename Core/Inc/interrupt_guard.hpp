#pragma once

#include "stm32f1xx.h"

namespace app {

class InterruptGuard {
 public:
  InterruptGuard() : primask_(__get_PRIMASK()) { __disable_irq(); };
  ~InterruptGuard() { __set_PRIMASK(primask_); }

  InterruptGuard(const InterruptGuard&) = delete;
  InterruptGuard& operator=(const InterruptGuard&) = delete;

 private:
  const uint32_t primask_;
};

}  // namespace app

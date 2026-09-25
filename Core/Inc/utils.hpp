#pragma once

#include <cstdint>
#include <cstdio>

#include "cmsis_gcc.h"
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

inline void EnableCycCnt() {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

inline void DelayUs(const uint32_t us) {
  const uint32_t start = DWT->CYCCNT;
  const uint32_t cycles =
      static_cast<uint64_t>(us) * SystemCoreClock / 1000000U;
  while (DWT->CYCCNT - start < cycles) {
  }
}

}  // namespace app

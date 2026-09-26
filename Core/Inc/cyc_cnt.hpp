#pragma once

#include <climits>
#include <cstdint>

#include "stm32f1xx.h"

namespace app {

template <typename T>
class SerialTick {
  static_assert(T(0) < T(-1), "T must be unsigned");

 public:
  static constexpr T kHalf = T(1) << (sizeof(T) * CHAR_BIT - 1);

  // Determines whether `now` has reached or passed `reference`. This is
  // meaningful only when the actual time difference is less than half a count
  // cycle.
  static constexpr bool HasReached(T now, T reference) noexcept {
    return static_cast<T>(now - reference) < kHalf;
  }
};

namespace {
constexpr uint32_t kUsPerSecond = 1000000U;
constexpr uint32_t CycAt(const uint32_t us, const uint32_t clock) {
  return static_cast<uint64_t>(us) * clock / kUsPerSecond;
}
constexpr uint32_t UsAt(const uint32_t cyc, const uint32_t clock) {
  return static_cast<uint64_t>(cyc) * kUsPerSecond / clock;
}
}  // namespace

class CycCnt {
 public:
  static void Enable() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  }
  static bool Enabled() { return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0U; }

  static uint32_t Cyc(const uint32_t us) { return CycAt(us, SystemCoreClock); }

  static uint32_t Get() { return DWT->CYCCNT; }
  static void Delay(const uint32_t cycles) {
    const uint32_t start = Get();
    while (Get() - start < cycles) continue;
  }
  static bool HasReached(uint32_t reference) {
    return SerialTick<uint32_t>::HasReached(Get(), reference);
  }

 private:
  static constexpr uint32_t kMaxClock = 72000000U;  // STM32F103 Max HCLK
  static constexpr uint32_t kMaxDelayUs =
      UsAt(SerialTick<uint32_t>::kHalf, kMaxClock);
};

}  // namespace app

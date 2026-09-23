#include "tim.hpp"

#include "usart.h"

#include <cinttypes>
#include <cstdio>

namespace tim {

constexpr uint32_t kXmitTimeoutMs = 100;
constexpr uint32_t kXmitIntervalMs = 500;

Timer tim2(&htim2, []() {
  static constexpr uint8_t updMsg[] = "An automatic reload was triggered.\n";
  static constexpr uint8_t trigMsg[] = "A reset was triggered.\n";
  if (__HAL_TIM_GET_FLAG(&htim2, TIM_FLAG_TRIGGER)) {
    __HAL_TIM_CLEAR_FLAG(&htim2, TIM_FLAG_TRIGGER);
    HAL_UART_Transmit(&huart2, trigMsg, sizeof(trigMsg) - 1, kXmitTimeoutMs);
  } else
    HAL_UART_Transmit(&huart2, updMsg, sizeof(updMsg) - 1, kXmitTimeoutMs);
});

void setup() { Timer::dispatchSetup(); }
void loop() {
  Timer::dispatchLoop();
  const uint32_t cnt = tim2.getCnt();
  static uint8_t msg[256];
  const uint16_t len = snprintf(reinterpret_cast<char *>(msg), sizeof(msg),
                                "cnt=%" PRIu32 "\n", cnt);
  HAL_UART_Transmit(&huart2, msg, len, kXmitTimeoutMs);
  HAL_Delay(kXmitIntervalMs);
}

} // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tim::Timer::dispatchPeriodElapsed(htim);
}

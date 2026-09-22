#include "tim.hpp"

#include "usart.h"

#include <cinttypes>

namespace tim {

Timer tim4(&htim4);
Timer tim2(&htim2);

void setup() { Timer::dispatchSetup(); }
void loop() {
  Timer::dispatchLoop();

  uint32_t cnt = tim2.getCnt();
  static uint8_t msg[255];
  snprintf(reinterpret_cast<char *>(msg), sizeof(msg), "cnt=%" PRIu32 "\n",
           cnt);
  HAL_UART_Transmit_IT(&huart2, msg, sizeof(msg) - 1);
}

} // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tim::Timer::dispatchPeriodElapsed(htim);
}

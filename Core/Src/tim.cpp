#include "tim.hpp"

#include "usart.h"

namespace tim {

static constexpr uint8_t testData[] = "Hello, world!\n";
Timer tim4(&htim4, []() {
  HAL_UART_Transmit_IT(&huart2, testData, sizeof(testData) - 1);
});

void setup() { Timer::dispatchSetup(); }
void loop() { Timer::dispatchLoop(); }

} // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tim::Timer::dispatchPeriodElapsed(htim);
}

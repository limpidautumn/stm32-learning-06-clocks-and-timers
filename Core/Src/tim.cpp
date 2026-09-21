#include "tim.hpp"

#include "usart.h"

namespace tim {

Timer tim4(&htim4);

void setup() { Timer::dispatchSetup(); }
void loop() { Timer::dispatchLoop(); }

} // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tim::Timer::dispatchPeriodElapsed(htim);
}

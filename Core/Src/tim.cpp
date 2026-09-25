#include "tim.hpp"

#include "usart.h"

namespace tim {

Timer tim4(&htim4);

void Setup() { Timer::DispatchSetup(); }
void Loop() { Timer::DispatchLoop(); }

}  // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  tim::Timer::DispatchPeriodElapsed(htim);
}

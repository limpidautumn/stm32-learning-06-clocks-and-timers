#include "tim.hpp"

namespace app {}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  app::Timer::DispatchPeriodElapsed(htim);
}

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
  app::Timer::DispatchCapture(htim);
}

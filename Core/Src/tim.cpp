#include "tim.hpp"

namespace app {}

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
  app::TimerBase::DispatchPeriodElapsed(htim);
}

extern "C" void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef* htim) {
  app::TimerBase::DispatchCapture(htim);
}

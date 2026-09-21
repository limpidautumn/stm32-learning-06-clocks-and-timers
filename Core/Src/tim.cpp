#include "tim.hpp"

namespace tim {

Timer tim4(&htim4);

void setup() { tim4.setup(); }
void loop() {}

} // namespace tim

extern "C" void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  tim::Timer::dispatchPeriodElapsed(htim);
}

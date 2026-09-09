#pragma once
#include "error.hpp"

#include "tim.h"

namespace tim {

void setup();
void loop();

class Timer {
public:
  Timer(TIM_HandleTypeDef *const htim) : htim(htim) {}
  void setup() {
    if (HAL_TIM_Base_Start(htim) != HAL_OK)
      err::fatalErrorHandler();
  }
  uint32_t getCnt() { return __HAL_TIM_GET_COUNTER(htim); }

private:
  TIM_HandleTypeDef *const htim;
};

} // namespace tim

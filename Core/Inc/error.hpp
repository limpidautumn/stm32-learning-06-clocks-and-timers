#pragma once

#include "stm32f1xx_hal.h"

namespace err {

inline void fatalErrorHandler() {
  HAL_Delay(100);
  HAL_NVIC_SystemReset();
}

} // namespace err

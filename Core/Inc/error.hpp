#pragma once

#include "stm32f1xx_hal.h"

#include <etl/error_handler.h>

namespace err {

inline void fatalErrorHandler() {
  HAL_Delay(100);
  HAL_NVIC_SystemReset();
}

inline void etlErrorHandler(const etl::exception &) { fatalErrorHandler(); }

inline void setup() {
  etl::error_handler::set_callback<&etlErrorHandler>();
}

} // namespace err

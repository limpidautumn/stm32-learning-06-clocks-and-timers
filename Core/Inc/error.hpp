#pragma once

#include <etl/error_handler.h>

#include "stm32f1xx_hal.h"

namespace err {

inline void FatalErrorHandler() {
  HAL_Delay(100);
  HAL_NVIC_SystemReset();
}

inline void EtlErrorHandler(const etl::exception&) { FatalErrorHandler(); }

inline void Setup() { etl::error_handler::set_callback<&EtlErrorHandler>(); }

}  // namespace err

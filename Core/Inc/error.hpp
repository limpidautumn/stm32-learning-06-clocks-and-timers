#pragma once

#include <etl/error_handler.h>

#include "utils.hpp"

namespace app {

class Error {
 public:
  static void Setup() {
    EnableCycCnt();
    etl::error_handler::set_callback<&OnError>();
  }

  static void Fatal() {
    EnableCycCnt();
    DelayUs(100000);
    HAL_NVIC_SystemReset();
  }

 private:
  static void OnError(const etl::exception&) { Fatal(); }
};

}  // namespace app

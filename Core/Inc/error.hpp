#pragma once

#include <etl/error_handler.h>

#include <cassert>

#include "cyc_cnt.hpp"

namespace app {

class Error {
 public:
  static void Setup() { etl::error_handler::set_callback<&OnError>(); }

  static void Fatal() {
    assert(CycCnt::Enabled());
    CycCnt::Delay(CycCnt::Cyc(100000));
    HAL_NVIC_SystemReset();
  }

 private:
  static void OnError(const etl::exception&) { Fatal(); }
};

}  // namespace app

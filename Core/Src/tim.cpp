#include "tim.hpp"

#include "usart.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>

namespace tim {

Timer tim4(&htim4);
void setup() { tim4.setup(); }
void loop() {
  uint32_t cnt = tim4.getCnt();
  static char msg[255];
  snprintf(msg, sizeof(msg), "cnt=%" PRIu32 "\n", cnt);
  HAL_UART_Transmit_IT(&huart2, reinterpret_cast<uint8_t *>(msg), strlen(msg));

  HAL_Delay(100 - 1);
}

} // namespace tim

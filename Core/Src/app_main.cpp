#include "app_main.hpp"

#include "tim.hpp"

void app_setup() {
  err::setup();
  tim::setup();
}

void app_loop() {
  while (true) {
    tim::loop();
  }
}

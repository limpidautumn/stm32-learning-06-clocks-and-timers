#include "app_main.hpp"

#include "tim.hpp"

void AppSetup() {
  err::Setup();
  tim::Setup();
}

void AppLoop() {
  while (true) {
    tim::Loop();
  }
}

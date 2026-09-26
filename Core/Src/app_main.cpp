#include "app_main.hpp"

#include "error.hpp"
#include "pending.hpp"
#include "tim.hpp"

void AppSetup() {
  app::CycCnt::Enable();
  app::Error::Setup();
  app::Timer::Setup();
}

void AppLoop() {
  while (true) {
    app::Pending::Run();
  }
}

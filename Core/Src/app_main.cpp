#include "app_main.hpp"

#include "error.hpp"
#include "hc_sr04.hpp"
#include "pending.hpp"
#include "tim.hpp"
#include "tim_channel.hpp"

namespace app {

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  TimerBase::Setup();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

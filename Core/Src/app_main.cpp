#include "app_main.hpp"

#include "error.hpp"
#include "hc_sr04.hpp"
#include "pending.hpp"
#include "tim.hpp"
#include "tim_channel.hpp"

namespace app {

Timer tim1(&htim1);
TimerIc tim1_ic3(&htim1, TIM_CHANNEL_3);
TimerIc tim1_ic4(&htim1, TIM_CHANNEL_4);

HcSr04 hc_sr04(HC_SR04_Trig_GPIO_Port, HC_SR04_Trig_Pin, tim1_ic3, tim1_ic4);

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  tim1.AddChannel(tim1_ic3).AddChannel(tim1_ic4);
  Timer::Setup();

  hc_sr04.Setup();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

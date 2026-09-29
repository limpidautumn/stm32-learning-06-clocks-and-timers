#include "app_main.hpp"

#include "error.hpp"
#include "hc_sr04.hpp"
#include "pending.hpp"
#include "stm32f1xx_hal_def.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.hpp"
#include "tim_channel.hpp"

namespace app {

Timer tim3(&htim3);
TimerPwm tim3_ch1(&htim3, TIM_CHANNEL_1);
TimerPwm tim3_ch2(&htim3, TIM_CHANNEL_2);
TimerPwm tim3_ch3(&htim3, TIM_CHANNEL_3);

namespace {

uint8_t LedDuty(const uint32_t& x) {
  if (x < 100u) return x;
  if (x < 200u) return 200u - x;
  return 0;
}

void LedStep() {
  static constexpr uint32_t step_us = 20000;  // 20ms
  static constexpr uint8_t n = 3;
  static constexpr uint32_t mod = 100u * n;
  static constexpr TimerPwm* pwm[n] = {&tim3_ch1, &tim3_ch2, &tim3_ch3};
  static uint32_t idx = 0;

  idx = (idx + 1u) % mod;
  for (int i = 0; i < n; ++i) {
    const uint8_t duty = LedDuty((idx + i * 100) % mod);
    pwm[i]->SetDuty(duty);
  }

  Pending::Push([]() { LedStep(); }, step_us);
}

}  // namespace

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  tim3.AddChannel(tim3_ch1).AddChannel(tim3_ch2).AddChannel(tim3_ch3);
  Timer::Setup();

  LedStep();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

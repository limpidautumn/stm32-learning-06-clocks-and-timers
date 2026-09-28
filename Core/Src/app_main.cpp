#include "app_main.hpp"

#include <cinttypes>
#include <cstdio>

#include "cyc_cnt.hpp"
#include "error.hpp"
#include "gpio.h"
#include "hc_sr04.hpp"
#include "main.h"
#include "pending.hpp"
#include "stm32f1xx_hal_gpio.h"
#include "stm32f1xx_hal_uart.h"
#include "tim.h"
#include "tim.hpp"
#include "tim_ic.hpp"
#include "usart.h"

namespace app {

Timer tim2(&htim2);
TimerIc tim2_ic1(&htim2, TIM_CHANNEL_1);
TimerIc tim2_ic2(&htim2, TIM_CHANNEL_2);

namespace {

constexpr uint32_t kPulseUs = 1000u;                          // 1ms
constexpr uint32_t kWaitUs = 10000u - kPulseUs;               // 10ms
constexpr uint32_t kDelayUs = 1000000u - kPulseUs - kWaitUs;  // 1s

uint32_t ts_ic1, ts_ic2, pulse_width;

void TestSetup() {
  tim2_ic1.SetCallback([]() { ts_ic1 = tim2_ic1.Value(); });
  tim2_ic2.SetCallback([]() { ts_ic2 = tim2_ic2.Value(); });
  tim2.AddIc(tim2_ic1).AddIc(tim2_ic2);
  Timer::Setup();
}

void TestLoop() {
  CycCnt::Delay(CycCnt::Cyc(kDelayUs));

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  CycCnt::Delay(CycCnt::Cyc(kPulseUs));
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);

  CycCnt::Delay(kWaitUs);

  pulse_width = ts_ic2 - ts_ic1;

  return;  // Breakpoint here
}

}  // namespace

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  TestSetup();
}

void Loop() { TestLoop(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

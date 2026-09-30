#include "app_main.hpp"

#include <cinttypes>
#include <cstdio>

#include "error.hpp"
#include "hc_sr04.hpp"
#include "pending.hpp"
#include "tim.hpp"
#include "tim_channel.hpp"
#include "usart.h"

namespace app {

Timer tim1(&htim1);  // Encoder

Timer tim3(&htim3);  // LED
TimerPwm tim3_ch1(&htim3, TIM_CHANNEL_1);
TimerPwm tim3_ch2(&htim3, TIM_CHANNEL_2);
TimerPwm tim3_ch3(&htim3, TIM_CHANNEL_3);

namespace {

void ReportEncoder() {
  static constexpr uint32_t kDelayUs = 50000;  // 50ms
  static uint8_t buf[32];
  uint8_t len = snprintf(reinterpret_cast<char*>(buf), sizeof(buf),
                         "%" PRIu32 "\n", tim1.Count());
  HAL_UART_Transmit_IT(&huart2, buf, len);

  Pending::Push([]() { ReportEncoder(); }, kDelayUs);
}

}  // namespace

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  tim3.AddChannel(tim3_ch1).AddChannel(tim3_ch2);
  Timer::Setup();

  ReportEncoder();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

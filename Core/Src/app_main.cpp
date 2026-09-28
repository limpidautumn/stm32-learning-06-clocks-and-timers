#include "app_main.hpp"

#include <cinttypes>
#include <cstdio>

#include "error.hpp"
#include "hc_sr04.hpp"
#include "main.h"
#include "pending.hpp"
#include "stm32f1xx_hal_uart.h"
#include "tim.hpp"
#include "tim_ic.hpp"
#include "usart.h"

namespace app {

Timer tim1(&htim1);
TimerIc tim1_ic3(&htim1, TIM_CHANNEL_3);
TimerIc tim1_ic4(&htim1, TIM_CHANNEL_4);

HcSr04 hc_sr04(HC_SR04_Trig_GPIO_Port, HC_SR04_Trig_Pin, tim1_ic3, tim1_ic4);

namespace {
void ReportPulseUs() {
  static constexpr uint32_t kReportPeriodUs = 50000U;
  static uint8_t tx_buf[64];
  const uint32_t t_us = hc_sr04.PulseUs();

  if (hc_sr04.IsValid()) {
    const uint32_t x_mm = (t_us * 343u + 500u) / 1000u / 2u;
    const int len = snprintf(reinterpret_cast<char*>(tx_buf), sizeof(tx_buf),
                             "%" PRIu32 "\n", x_mm);
    HAL_UART_Transmit_IT(&huart2, tx_buf, len);
  }

  Pending::Push(Callback::create<&ReportPulseUs>(), kReportPeriodUs);
}
}  // namespace

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  tim1.AddIc(tim1_ic3).AddIc(tim1_ic4);
  Timer::Setup();

  hc_sr04.Setup();

  ReportPulseUs();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

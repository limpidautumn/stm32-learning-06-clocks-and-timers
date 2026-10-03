#include "app_main.hpp"

#include <cinttypes>
#include <cstdio>

#include "error.hpp"
#include "hc_sr04.hpp"
#include "pending.hpp"
#include "stm32f1xx_hal_tim.h"
#include "stm32f1xx_hal_uart.h"
#include "tim.hpp"
#include "tim_channel.hpp"
#include "usart.h"

namespace app {

TimerPwm tim4_ch3(&htim4, TIM_CHANNEL_3);  // Servo motor
Timer<TimerPwm> tim4(&htim4, tim4_ch3);

TimerEncoder tim1_enc(&htim1);  // EC11 Encoder
Timer<TimerEncoder> tim1(&htim1, tim1_enc);

namespace {

// Encoder: 20 pulses / 360 deg, 4x multiplication
constexpr uint32_t kEncRangeMax = 40;       // inclusive [0, 40]
constexpr uint32_t kTimerCntMax = 0xFFFFu;  // 16-bit timer
constexpr uint32_t kEncWrapMidpoint = (kEncRangeMax + kTimerCntMax) / 2;
static uint32_t g_enc_wrapped_cnt = 0;  // [0, kModRangeMax]

// Servo: 2.5%-12.5% duty
constexpr uint32_t kServoPwmPeriod = 10u * kEncRangeMax;
constexpr uint32_t kServoPwmStep = 1u;
constexpr uint32_t kServoPwmBase = 10u;
static_assert(kServoPwmBase * 40 == kServoPwmPeriod);  // 2.5%
static_assert((kServoPwmBase + kServoPwmStep * kEncRangeMax) * 8 ==
              kServoPwmPeriod);  // 12.5%

void Configure() {
  static constexpr uint32_t kConfigurePeriodUs = 5000;  // 5ms

  // Capture
  g_enc_wrapped_cnt = tim1.Counter();
  if (g_enc_wrapped_cnt > kEncRangeMax) {
    if (g_enc_wrapped_cnt > kEncWrapMidpoint)
      g_enc_wrapped_cnt = 0;
    else
      g_enc_wrapped_cnt = kEncRangeMax;
  }
  tim1.SetCounter(g_enc_wrapped_cnt);

  // Configure
  tim4_ch3.SetDuty(kServoPwmBase + kServoPwmStep * g_enc_wrapped_cnt,
                   kServoPwmPeriod);

  Pending::Push([]() { Configure(); }, kConfigurePeriodUs);
}

void Report() {
  static constexpr uint32_t kReportPeriodUs = 50000;  // 50ms

  static uint8_t buf[256] = {};
  std::size_t len = snprintf(reinterpret_cast<char*>(buf), sizeof(buf),
                             "%" PRIu32 "\n", g_enc_wrapped_cnt);
  HAL_UART_Transmit_IT(&huart2, buf, len);

  Pending::Push([]() { Report(); }, kReportPeriodUs);
}

}  // namespace

void Setup() {
  CycCnt::Enable();
  Error::Setup();

  TimerBase::Setup();

  Configure();
  Report();
}

void Loop() { Pending::Run(); }

}  // namespace app

void AppSetup() { app::Setup(); }

void AppLoop() { app::Loop(); }

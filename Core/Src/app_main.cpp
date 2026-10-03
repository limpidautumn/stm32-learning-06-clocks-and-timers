#include "app_main.hpp"

#include <cinttypes>
#include <cstdio>

#include "error.hpp"
#include "hc_sr04.hpp"
#include "main.h"
#include "pending.hpp"
#include "tim.hpp"
#include "tim_channel.hpp"
#include "usart.h"

namespace app {

TimerPwm tim4_ch3(&htim4, TIM_CHANNEL_3);  // Servo motor
Timer<TimerPwm> tim4(&htim4, tim4_ch3);

TimerPwm tim2_ch1(&htim2, TIM_CHANNEL_1);  // DRV8833 AIN1
TimerPwm tim2_ch2(&htim2, TIM_CHANNEL_2);  // DRV8833 AIN2
Timer<TimerPwm, TimerPwm> tim2(&htim2, tim2_ch1, tim2_ch2);

TimerEncoder tim1_enc(&htim1);  // EC11 Encoder
Timer<TimerEncoder> tim1(&htim1, tim1_enc);

namespace {

// Encoder: 20 pulses / 360 deg, 4x multiplication
constexpr uint32_t kEncRangeMax = 40;       // inclusive [0, 40]
constexpr uint32_t kTimerCntMax = 0xFFFFu;  // 16-bit timer
constexpr uint32_t kEncWrapMidpoint = (kEncRangeMax + kTimerCntMax) / 2;
static uint32_t g_enc_wrapped_cnt = 0;  // [0, kEncRangeMax]

// Servo Motor: 2.5%-12.5% duty
constexpr uint32_t kServoPwmPeriod = 10u * kEncRangeMax;
constexpr uint32_t kServoPwmStep = 1u;
constexpr uint32_t kServoPwmBase = 10u;
static_assert(kServoPwmBase * 40 == kServoPwmPeriod);  // 2.5%
static_assert((kServoPwmBase + kServoPwmStep * kEncRangeMax) * 8 ==
              kServoPwmPeriod);  // 12.5%

// DC Motor: DRV8833
constexpr uint32_t kEncRangeMid = kEncRangeMax / 2;

enum class MotorDirection : uint8_t {
  kForward,
  kReverse,
};

// Drives one DRV8833 H-bridge using slow-decay PWM.
// numerator / denominator is the effective drive duty cycle.
void Drv8833(const uint32_t num, const uint32_t den = 1,
             const MotorDirection direction = MotorDirection::kForward) {
  if (num > den) Error::Fatal();
  TimerPwm& in1 = tim2_ch1;
  TimerPwm& in2 = tim2_ch2;
  TimerPwm& a = direction == MotorDirection::kForward ? in1 : in2;
  TimerPwm& b = direction == MotorDirection::kForward ? in2 : in1;

  // Fast decay PWM
  a.SetDuty(den, den);
  b.SetDuty(den - num, den);
}

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

  // Servo motor configuration
  tim4_ch3.SetDuty(kServoPwmBase + kServoPwmStep * g_enc_wrapped_cnt,
                   kServoPwmPeriod);

  // DC motor configuration
  if (HAL_GPIO_ReadPin(KEY_1_GPIO_Port, KEY_1_Pin) == GPIO_PIN_RESET) {
    if (g_enc_wrapped_cnt < kEncRangeMid) {
      Drv8833(kEncRangeMid - g_enc_wrapped_cnt, kEncRangeMid,
              MotorDirection::kForward);
    } else {
      Drv8833(g_enc_wrapped_cnt - kEncRangeMid, kEncRangeMax - kEncRangeMid,
              MotorDirection::kReverse);
    }
  } else {
    Drv8833(0);
  }

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

#pragma once

#include <cstdint>

#include "cyc_cnt.hpp"
#include "pending.hpp"
#include "stm32f103xb.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal_gpio.h"
#include "tim.h"
#include "tim.hpp"
#include "tim_channel.hpp"

namespace app {
class HcSr04 {
 private:
  static constexpr uint32_t kXmitPulseWidthUs = 10U + 40U;
  static constexpr uint32_t kTickHz = 1000000U;
  static constexpr uint32_t kDelayUs = 50000U;
  static constexpr uint32_t kTimeoutUs = kDelayUs + 0U;
  static constexpr uint32_t kInf = ~uint32_t{0};

 public:
  HcSr04(GPIO_TypeDef* trig_port, uint16_t trig_pin, TimerIc& echo_rise,
         TimerIc& echo_fall)
      : trig_port_(trig_port),
        trig_pin_(trig_pin),
        ic_rise_(echo_rise),
        ic_fall_(echo_fall) {
    ic_rise_.SetCallback(Callback::create<HcSr04, &HcSr04::OnRise>(*this));
    ic_fall_.SetCallback(Callback::create<HcSr04, &HcSr04::OnFall>(*this));
  }

  void Setup() { Schedule(kDelayUs); }

  uint32_t PulseUs() const { return recv_pulse_us_; }
  bool IsValid() const { return recv_pulse_us_ != kInf; }

 private:
  GPIO_TypeDef* const trig_port_;
  const uint16_t trig_pin_;
  TimerIc& ic_rise_;
  TimerIc& ic_fall_;

  const Callback timeout_ = Callback::create<HcSr04, &HcSr04::OnTimeout>(*this);

  uint32_t ts_rise_ = 0, ts_fall_ = 0;
  uint32_t recv_pulse_us_ = kInf;

  void Schedule(const uint32_t delay_us = 0) {
    Pending::Push(Callback::create<HcSr04, &HcSr04::AdvanceState>(*this),
                  delay_us);
  }

  void OnRise() { ts_rise_ = ic_rise_.Value(); }
  void OnFall() {
    ts_fall_ = ic_fall_.Value();
    Schedule();
  }
  void OnTimeout() {
    recv_pulse_us_ = kInf;
    state_ = StateEnum::kIdle;
    Schedule();
  }

  void AdvanceState() {
    switch (state_) {
      case StateEnum::kIdle:
        HAL_GPIO_WritePin(trig_port_, trig_pin_, GPIO_PIN_SET);
        state_ = StateEnum::kTriggering;
        Schedule(kXmitPulseWidthUs);
        break;
      case StateEnum::kTriggering:
        HAL_GPIO_WritePin(trig_port_, trig_pin_, GPIO_PIN_RESET);
        state_ = StateEnum::kWaitingEcho;
        Pending::Push(timeout_, kTimeoutUs);
        break;
      case StateEnum::kWaitingEcho:
        Pending::Remove(timeout_);
        recv_pulse_us_ =
            static_cast<uint64_t>(ts_fall_ - ts_rise_) * kUsPerSecond / kTickHz;
        state_ = StateEnum::kIdle;
        Schedule(kDelayUs);
        break;
    }
  }

  enum class StateEnum : uint8_t {
    kIdle,
    kTriggering,
    kWaitingEcho,
  };
  StateEnum state_ = StateEnum::kIdle;
};

}  // namespace app

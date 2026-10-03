#pragma once

#include "error.hpp"
#include "pending.hpp"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"

namespace app {

// CRTP static interface for a timer channel.
template <typename Derived>
class TimerChannel {
 public:
  TimerChannel(const TimerChannel&) = delete;
  TimerChannel& operator=(const TimerChannel&) = delete;

  void Start() { static_cast<Derived*>(this)->Start(); }
  void OnCapture(const TIM_HandleTypeDef* const handle) {
    static_cast<Derived*>(this)->OnCapture(handle);
  }

 protected:
  TimerChannel(TIM_HandleTypeDef* const htim, const uint32_t channel)
      : htim_(htim), channel_(channel) {}
  ~TimerChannel() = default;

  TIM_HandleTypeDef* Handle() const { return htim_; }
  uint32_t Channel() const { return channel_; }

 private:
  TIM_HandleTypeDef* const htim_;
  const uint32_t channel_;
};

class TimerIc final : public TimerChannel<TimerIc> {
 public:
  TimerIc(TIM_HandleTypeDef* const htim, const uint32_t channel)
      : TimerChannel(htim, channel) {}

  void SetCallback(const Callback& callback) { callback_ = callback; }

  void Start() {
    if (HAL_TIM_IC_Start_IT(Handle(), Channel()) != HAL_OK) Error::Fatal();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) {
    if (handle->Channel == ActiveChannel(Channel())) callback_.call_if();
  }

  uint32_t Value() const {
    return HAL_TIM_ReadCapturedValue(Handle(), Channel());
  }

 private:
  static HAL_TIM_ActiveChannel ActiveChannel(const uint32_t channel) {
    switch (channel) {
      case TIM_CHANNEL_1:
        return HAL_TIM_ACTIVE_CHANNEL_1;
      case TIM_CHANNEL_2:
        return HAL_TIM_ACTIVE_CHANNEL_2;
      case TIM_CHANNEL_3:
        return HAL_TIM_ACTIVE_CHANNEL_3;
      case TIM_CHANNEL_4:
        return HAL_TIM_ACTIVE_CHANNEL_4;
      default:
        return HAL_TIM_ACTIVE_CHANNEL_CLEARED;
    }
  }

  Callback callback_;
};

class TimerOc final : public TimerChannel<TimerOc> {
 public:
  TimerOc(TIM_HandleTypeDef* const htim, const uint32_t channel)
      : TimerChannel(htim, channel) {}

  void Start() {
    if (HAL_TIM_OC_Start(Handle(), Channel()) != HAL_OK) Error::Fatal();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) { UNUSED(handle); }
};

class TimerPwm final : public TimerChannel<TimerPwm> {
 public:
  TimerPwm(TIM_HandleTypeDef* const htim, const uint32_t channel)
      : TimerChannel(htim, channel) {}

  void Start() {
    if (HAL_TIM_PWM_Start(Handle(), Channel()) != HAL_OK) Error::Fatal();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) { UNUSED(handle); }

  // duty = numerator / denominator
  void SetDuty(const uint32_t num, uint32_t den = 100) {
    if (den == 0) Error::Fatal();
    static constexpr uint32_t kTimerCcrMax = 0xFFFFu;  // 16-bit CCR
    const uint32_t period = __HAL_TIM_GET_AUTORELOAD(Handle());
    uint32_t compare = static_cast<uint64_t>(period + 1u) * num / den;
    if (compare > kTimerCcrMax) compare = kTimerCcrMax;
    __HAL_TIM_SET_COMPARE(Handle(), Channel(), compare);
  }
};

// Combined channels: TIM_CHANNEL_1/2 form one quadrature encoder interface.
class TimerEncoder final : public TimerChannel<TimerEncoder> {
 public:
  explicit TimerEncoder(TIM_HandleTypeDef* const htim)
      : TimerChannel(htim, TIM_CHANNEL_ALL) {}

  void Start() {
    if (HAL_TIM_Encoder_Start(Handle(), Channel()) != HAL_OK) Error::Fatal();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) { UNUSED(handle); }
};

}  // namespace app

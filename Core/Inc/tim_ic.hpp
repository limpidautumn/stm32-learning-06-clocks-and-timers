#pragma once

#include "error.hpp"
#include "pending.hpp"
#include "tim.h"

namespace app {

class TimerIc {
 public:
  TimerIc(TIM_HandleTypeDef* const htim, const uint32_t channel,
          const Callback& callback = {}, void* const user = nullptr)
      : htim_(htim), channel_(channel), callback_(callback), user_(user) {}

  void Start() {
    if (HAL_TIM_IC_Start_IT(htim_, channel_) != HAL_OK) Error::Fatal();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) {
    if (handle->Channel == ActiveChannel(channel_)) callback_.call_if(user_);
  }

  uint32_t Value() const { return HAL_TIM_ReadCapturedValue(htim_, channel_); }

 private:
  TIM_HandleTypeDef* const htim_;
  const uint32_t channel_;
  const Callback callback_;
  void* const user_;

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
};

}  // namespace app

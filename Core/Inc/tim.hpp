#pragma once

#include <etl/vector.h>

#include "error.hpp"
#include "pending.hpp"
#include "tim.h"
#include "tim_channel.hpp"

namespace app {

class Timer {
 public:
  explicit Timer(TIM_HandleTypeDef* const htim) : htim_(htim) {
    Timers().push_back(this);
  }

  ~Timer() { Unregister(); }

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  Timer& AddChannel(TimerChannel& channel) {
    if (started_) Error::Fatal();
    if (channels_.full()) Error::Fatal();
    channels_.push_back(&channel);
    return *this;
  }

  static void Setup() {
    for (Timer* timer : Timers()) timer->OnSetup();
  }

  static void DispatchPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    for (Timer* timer : Timers()) timer->OnPeriodElapsed(handle);
  }

  static void DispatchCapture(const TIM_HandleTypeDef* const handle) {
    for (Timer* timer : Timers()) timer->OnCapture(handle);
  }

  void SetPeriod(const Callback& callback) { period_ = callback; }

  uint32_t Count() const { return __HAL_TIM_GET_COUNTER(htim_); }

 private:
  bool started_ = false;
  void OnSetup() {
    if (HAL_TIM_Base_Start_IT(htim_) != HAL_OK) Error::Fatal();
    for (TimerChannel* channel : channels_) channel->Start();
    started_ = true;
  }

  void OnPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    if (handle == htim_) period_.call_if();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) {
    if (handle != htim_) return;
    for (TimerChannel* channel : channels_) channel->OnCapture(handle);
  }

  static constexpr std::size_t kMaxChannels = 4;
  static constexpr std::size_t kMaxTimers = 8;
  using ChannelList = etl::vector<TimerChannel*, kMaxChannels>;
  using TimerList = etl::vector<Timer*, kMaxTimers>;

  // To avoid static initialization order issues
  static TimerList& Timers() {
    static TimerList list;
    return list;
  }

  void Unregister() {
    auto& list = Timers();
    for (auto it = list.begin(); it != list.end(); ++it) {
      if (*it == this) {
        list.erase(it);
        return;
      }
    }
  }

  TIM_HandleTypeDef* const htim_;
  Callback period_;
  ChannelList channels_;
};

}  // namespace app

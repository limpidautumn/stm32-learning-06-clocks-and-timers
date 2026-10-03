#pragma once

#include <etl/vector.h>

#include <cstddef>
#include <tuple>

#include "error.hpp"
#include "pending.hpp"
#include "tim.h"
#include "tim_channel.hpp"

namespace app {

// Type-erased base of every Timer so that setup and interrupt dispatch can
// reach timers with different channel sets.
class TimerBase {
 public:
  TimerBase(const TimerBase&) = delete;
  TimerBase& operator=(const TimerBase&) = delete;

  static void Setup() {
    for (TimerBase* timer : Timers()) timer->OnSetup();
  }

  static void DispatchPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    for (TimerBase* timer : Timers()) timer->OnPeriodElapsed(handle);
  }

  static void DispatchCapture(const TIM_HandleTypeDef* const handle) {
    for (TimerBase* timer : Timers()) timer->OnCapture(handle);
  }

  void SetPeriod(const Callback& callback) { period_ = callback; }

  uint32_t Count() const { return __HAL_TIM_GET_COUNTER(htim_); }

 protected:
  explicit TimerBase(TIM_HandleTypeDef* const htim) : htim_(htim) {
    Timers().push_back(this);
  }

  ~TimerBase() { Unregister(); }

  virtual void StartChannels() = 0;
  virtual void OnChannelCapture(const TIM_HandleTypeDef* const handle) = 0;

 private:
  static constexpr std::size_t kMaxTimers = 8;
  using TimerList = etl::vector<TimerBase*, kMaxTimers>;

  // To avoid static initialization order issues
  static TimerList& Timers() {
    static TimerList list;
    return list;
  }

  void OnSetup() {
    if (HAL_TIM_Base_Start_IT(htim_) != HAL_OK) Error::Fatal();
    StartChannels();
  }

  void OnPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    if (handle == htim_) period_.call_if();
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) {
    if (handle != htim_) return;
    OnChannelCapture(handle);
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
};

// Timer that owns a compile-time fixed set of channels.
template <typename... Channels>
class Timer final : public TimerBase {
 public:
  Timer(TIM_HandleTypeDef* const htim, Channels&... channels)
      : TimerBase(htim),
        channels_(static_cast<TimerChannel<Channels>*>(&channels)...) {}

 private:
  void StartChannels() override {
    std::apply([](auto*... channel) { (channel->Start(), ...); }, channels_);
  }

  void OnChannelCapture(const TIM_HandleTypeDef* const handle) override {
    std::apply(
        [handle](auto*... channel) { (channel->OnCapture(handle), ...); },
        channels_);
  }

  std::tuple<TimerChannel<Channels>*...> channels_;
};

}  // namespace app

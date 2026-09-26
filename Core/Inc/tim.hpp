#pragma once

#include <etl/vector.h>

#include "error.hpp"
#include "pending.hpp"
#include "tim.h"
#include "tim_ic.hpp"

namespace app {

class Timer {
 public:
  explicit Timer(TIM_HandleTypeDef* const htim) : htim_(htim) {
    Timers().push_back(this);
  }

  ~Timer() { Unregister(); }

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

  Timer& AddIc(TimerIc& ic) {
    ics_.push_back(&ic);
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

  void SetPeriod(const Callback& callback, void* const user = nullptr) {
    period_ = callback;
    period_user_ = user;
  }

  uint32_t Count() const { return __HAL_TIM_GET_COUNTER(htim_); }

 private:
  void OnSetup() {
    if (HAL_TIM_Base_Start_IT(htim_) != HAL_OK) Error::Fatal();
    for (TimerIc* ic : ics_) ic->Start();
  }

  void OnPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    if (handle == htim_) period_.call_if(period_user_);
  }

  void OnCapture(const TIM_HandleTypeDef* const handle) {
    if (handle != htim_) return;
    for (TimerIc* ic : ics_) ic->OnCapture(handle);
  }

  static constexpr std::size_t kMaxIcs = 4;
  static constexpr std::size_t kMaxTimers = 8;
  using IcList = etl::vector<TimerIc*, kMaxIcs>;
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
  void* period_user_ = nullptr;
  IcList ics_;
};

}  // namespace app

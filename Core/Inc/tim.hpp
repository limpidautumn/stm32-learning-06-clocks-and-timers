#pragma once
#include <etl/delegate.h>
#include <etl/queue_spsc_atomic.h>
#include <etl/vector.h>

#include "error.hpp"
#include "tim.h"

namespace tim {

void Setup();
void Loop();

class Timer {
 public:
  using Callback = etl::delegate<void()>;

  explicit Timer(TIM_HandleTypeDef* const htim, const Callback& callback = {})
      : htim_(htim), callback_(callback) {
    Timers().push_back(this);
  }

  ~Timer() { Unregister(); }

  void OnSetup() {
    if (HAL_TIM_Base_Start_IT(htim_) != HAL_OK) err::FatalErrorHandler();
  }

  void OnLoop() {
    Callback callback;
    while (pending_.pop(callback)) callback.call_if();
  }

  void OnPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    if (handle == htim_ && callback_) pending_.push(callback_);
  }

  static void DispatchSetup() {
    for (Timer* timer : Timers()) timer->OnSetup();
  }

  static void DispatchLoop() {
    for (Timer* timer : Timers()) timer->OnLoop();
  }

  static void DispatchPeriodElapsed(const TIM_HandleTypeDef* const handle) {
    for (Timer* timer : Timers()) timer->OnPeriodElapsed(handle);
  }

  uint32_t cnt() const { return __HAL_TIM_GET_COUNTER(htim_); }

 private:
  static constexpr std::size_t kMaxTimers = 8;
  static constexpr std::size_t kMaxPending = 8;
  using TimerList = etl::vector<Timer*, kMaxTimers>;
  using PendingQueue = etl::queue_spsc_atomic<Callback, kMaxPending>;

  // To avoid static initialization order issues
  static TimerList& Timers() {
    static TimerList list;
    return list;
  }

  void Unregister() {
    auto& list = Timers();
    for (auto it = list.begin(); it != list.end(); ++it)
      if (*it == this) {
        list.erase(it);
        return;
      }
  }

  TIM_HandleTypeDef* const htim_;
  const Callback callback_;
  PendingQueue pending_;
};

extern Timer tim4;

}  // namespace tim

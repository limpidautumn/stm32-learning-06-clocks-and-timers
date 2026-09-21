#pragma once
#include "error.hpp"

#include "tim.h"

#include <etl/delegate.h>
#include <etl/vector.h>

namespace tim {

void setup();
void loop();

class Timer {
public:
  using Callback = etl::delegate<void()>;

  explicit Timer(TIM_HandleTypeDef *const htim, const Callback &callback = {})
      : htim(htim), callback_(callback) {
    timers().push_back(this);
  }

  ~Timer() { unregister(); }

  void setup() {
    if (HAL_TIM_Base_Start_IT(htim) != HAL_OK)
      err::fatalErrorHandler();
  }

  void onPeriodElapsed(const TIM_HandleTypeDef *const handle) {
    if (handle == htim && callback_)
      callback_();
  }

  static void dispatchPeriodElapsed(const TIM_HandleTypeDef *const handle) {
    for (Timer *timer : timers())
      timer->onPeriodElapsed(handle);
  }

  uint32_t getCnt() const { return __HAL_TIM_GET_COUNTER(htim); }

private:
  static constexpr std::size_t MaxTimers = 8;
  using TimerList = etl::vector<Timer *, MaxTimers>;

  // To avoid static initialization order issues
  static TimerList &timers() {
    static TimerList list;
    return list;
  }

  void unregister() {
    auto &list = timers();
    for (auto it = list.begin(); it != list.end(); ++it)
      if (*it == this) {
        list.erase(it);
        return;
      }
  }

  TIM_HandleTypeDef *const htim;
  const Callback callback_;
};

extern Timer tim4;

} // namespace tim

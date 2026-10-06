#pragma once

#include <etl/delegate.h>
#include <etl/vector.h>

#include <cstddef>

#include "cyc_cnt.hpp"
#include "error.hpp"
#include "interrupt_guard.hpp"
#include "stm32f1xx.h"

namespace app {

using Callback = etl::delegate<void()>;

class Pending {
 public:
  static void Push(const Callback& callback, const uint32_t delay_us = 0) {
    const InterruptGuard guard;  // Multiple interrupts may be producers.
    if (Tasks().full()) Error::Fatal();
    Tasks().push_back({callback, CycCnt::Get() + CycCnt::Cyc(delay_us)});
  }

  static void Run() {
    for (std::size_t i = 0;;) {  // [0,i) was not due.
      Callback callback;
      {
        const InterruptGuard guard;
        const std::size_t size = Tasks().size();
        while (i < size && !CycCnt::HasReached(Tasks()[i].deadline)) ++i;
        if (i == size) return;
        callback = Tasks()[i].callback;
        RemoveAt(i);
      }
      callback.call_if();
    }
  }

  static void Remove(const Callback& callback) {
    const InterruptGuard guard;
    for (std::size_t i = 0; i < Tasks().size();) {
      if (Tasks()[i].callback == callback)
        RemoveAt(i);
      else
        ++i;
    }
  }

 private:
  struct Task {
    Callback callback;
    uint32_t deadline;
    bool operator<(const Task& t) const { return deadline > t.deadline; }
  };

  static constexpr std::size_t kMaxPending = 64;
  using PendingTasks = etl::vector<Task, kMaxPending>;

  // To avoid static initialization order issues
  static PendingTasks& Tasks() {
    static PendingTasks tasks;
    return tasks;
  }

  static void RemoveAt(const std::size_t i) {
    Tasks()[i] = Tasks().back();
    Tasks().pop_back();
  }
};

}  // namespace app

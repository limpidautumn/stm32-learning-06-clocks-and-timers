#pragma once

#include <etl/delegate.h>
#include <etl/priority_queue.h>

#include <cstddef>

#include "cyc_cnt.hpp"
#include "error.hpp"
#include "interrupt_guard.hpp"
#include "stm32f1xx.h"

namespace app {

using Callback = etl::delegate<void(void*)>;

class Pending {
 public:
  static void Push(const Callback& callback, const uint32_t delay_us,
                   void* const user = nullptr) {
    const InterruptGuard guard;  // Multiple interrupts may be producers.
    if (Tasks().full()) Error::Fatal();
    Tasks().push({callback, CycCnt::Get() + CycCnt::Cyc(delay_us), user});
  }

  static void Run() {
    Task task;
    // Callbacks run outside the guard so they may Push again.
    while (PopDue(task)) task.callback.call_if(task.user);
  }

 private:
  struct Task {
    Callback callback;
    uint32_t deadline;
    void* user;
    bool operator<(const Task& t) const { return deadline > t.deadline; }
  };
  static bool PopDue(Task& task) {
    const InterruptGuard guard;
    if (Tasks().empty() || !CycCnt::HasReached(Tasks().top().deadline))
      return false;
    Tasks().pop_into(task);
    return true;
  }

  static constexpr std::size_t kMaxPending = 64;
  using PendingTasks = etl::priority_queue<Task, kMaxPending>;

  // To avoid static initialization order issues
  static PendingTasks& Tasks() {
    static PendingTasks tasks;
    return tasks;
  }
};

}  // namespace app

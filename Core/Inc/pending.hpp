#pragma once

#include <etl/delegate.h>
#include <etl/queue_spsc_atomic.h>

#include "stm32f1xx.h"
#include "utils.hpp"

namespace app {

using Callback = etl::delegate<void(void*)>;

class Pending {
 public:
  static void Push(const Callback& callback, void* const user = nullptr) {
    const InterruptGuard guard;  // Multiple interrupts may be producers.
    Task task{callback, user};
    Queue().push(task);
  }

  static void Run() {
    Task task;
    while (Queue().pop(task)) task.callback.call_if(task.user);
  }

 private:
  struct Task {
    Callback callback;
    void* user;
  };

  static constexpr std::size_t kMaxPending = 256;
  using PendingQueue = etl::queue_spsc_atomic<Task, kMaxPending>;

  // To avoid static initialization order issues
  static PendingQueue& Queue() {
    static PendingQueue queue;
    return queue;
  }
};

}  // namespace app

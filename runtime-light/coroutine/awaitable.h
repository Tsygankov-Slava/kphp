#pragma once

#include <coroutine>

#include "runtime-light/component/component.h"

struct platform_switch_t {
  constexpr bool await_ready() const noexcept {
    return false;
  }

  void await_suspend(std::coroutine_handle<> h) const noexcept {
    get_component_context()->poll_status = PollStatus::PollBlocked;
    printf("suspend %d\n", get_component_context()->awaited_stream);
    get_component_context()->suspend_point = h;
  }

  constexpr void await_resume() const noexcept {}
};

struct test_yield_t {
  bool await_ready() const noexcept {
    return !get_platform_context()->please_yield.load();
  }

  void await_suspend(std::coroutine_handle<> h) const noexcept {
    get_component_context()->poll_status = PollStatus::PollReschedule;
    printf("suspend yield\n");
    get_component_context()->suspend_point = h;
  }

  constexpr void await_resume() const noexcept {}
};

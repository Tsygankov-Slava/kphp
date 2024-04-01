#include "runtime-light/stdlib/misc.h"

#include "runtime-light/component/component.h"
#include "runtime-light/utils/panic.h"
#include "runtime-light/coroutine/awaitable.h"
#include "runtime-light/streams/streams.h"

static int ob_merge_buffers() {
  Response &response = get_component_context()->response;
  php_assert (response.current_buffer >= 0);
  int ob_first_not_empty = 0;
  while (ob_first_not_empty < response.current_buffer && response.output_buffers[ob_first_not_empty].size() == 0) {
    ob_first_not_empty++;
  }
  for (int i = ob_first_not_empty + 1; i <= response.current_buffer; i++) {
    response.output_buffers[ob_first_not_empty].append(response.output_buffers[i].c_str(), response.output_buffers[i].size());
  }
  return ob_first_not_empty;
}

task_t<void> finish(int64_t exit_code) {
  co_await test_yield_t{};

  int ob_total_buffer = ob_merge_buffers();
  ComponentState &ctx = *get_component_context();
  Response &response = ctx.response;
  auto &buffer = response.output_buffers[ob_total_buffer];

  co_await write_all_to_stream(ctx.standard_stream, buffer.c_str(), buffer.size());
  get_component_context()->poll_status = PollStatus::PollFinished;
  co_return;
}

task_t<void> f$yield() {
  co_await test_yield_t{};
}

void f$check_shutdown() {
  if (get_platform_context()->please_graceful_shutdown.load()) {
    siglongjmp(get_component_context()->exit_tag, 1);
  }
}

task_t<void> f$exit(const mixed &v) {
  if (v.is_string()) {
    Response &response = get_component_context()->response;
    response.output_buffers[response.current_buffer] << v;
    co_await finish(0);
  } else {
    co_await finish(v.to_int());
  }
  panic();
}


task_t<void> f$die(const mixed &v) {
  co_await f$exit(v);
}
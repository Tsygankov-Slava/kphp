// Compiler for PHP (aka KPHP)
// Copyright (c) 2020 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

/* Autogenerated from common.tl and left only used constants */
#pragma once
#define TL__ 0x840e0eccU
#define TL_BOOL_FALSE 0xbc799737U
#define TL_BOOL_STAT 0x92cbcbfaU
#define TL_BOOL_TRUE 0x997275b5U
#define TL_DICTIONARY 0x1f4c618fU
#define TL_DICTIONARY_FIELD 0x239c1b62U
#define TL_DOUBLE 0x2210c154U
#define TL_FLOAT 0x824dab22U
#define TL_INT 0xa8509bdaU
#define TL_INT_KEY_DICTIONARY 0x07bafc42U
#define TL_INT_KEY_DICTIONARY_FIELD 0x721ea8b9U
#define TL_LEFT 0x0a29cd5dU
#define TL_LONG 0x22076cbaU
#define TL_LONG_KEY_DICTIONARY 0xb424d8f1U
#define TL_REQ_RESULT_HEADER 0x8cc84ce1U
#define TL_MAYBE_FALSE 0x27930a7bU
#define TL_MAYBE_TRUE 0x3f9c8ef8U
#define TL_RPC_DEST_ACTOR 0x7568aabdU
#define TL_RPC_DEST_ACTOR_FLAGS 0xf0a5acf7U
#define TL_RPC_DEST_FLAGS 0xe352035eU
#define TL_RPC_INVOKE_REQ 0x2374df3dU
#define TL_RPC_INVOKE_REQ_EXTRA 0xf3ef81a9U
#define TL_RPC_PING 0x5730a2dfU
#define TL_RPC_PONG 0x8430eaa7U
#define TL_RPC_REQ_ERROR 0x7ae432f5U
#define TL_RPC_REQ_RESULT 0x63aeda4eU
#define TL_RPC_REQ_RESULT_EXTRA 0xc5011709U
#define TL_STAT 0x9d56e6b2U
#define TL_STRING 0xb5286e24U
#define TL_TRUE 0x3fedd339U
#define TL_TUPLE 0x9770768aU
#define TL_VECTOR 0x1cb5c415U
#define TL_VECTOR_TOTAL 0x10133f47U

#include <cstdint>
namespace vk {
namespace tl {
namespace common {

namespace rpc_invoke_req_extra_flags {
constexpr static uint32_t return_binlog_pos = 1U << 0U;
constexpr static uint32_t return_binlog_time = 1U << 1U;
constexpr static uint32_t return_pid = 1U << 2U;
constexpr static uint32_t return_request_sizes = 1U << 3U;
constexpr static uint32_t return_failed_subqueries = 1U << 4U;
constexpr static uint32_t return_query_stats = 1U << 6U;
constexpr static uint32_t no_result = 1U << 7U;
constexpr static uint32_t wait_binlog_pos = 1U << 16U;
constexpr static uint32_t string_forward_keys = 1U << 18U;
constexpr static uint32_t int_forward_keys = 1U << 19U;
constexpr static uint32_t string_forward = 1U << 20U;
constexpr static uint32_t int_forward = 1U << 21U;
constexpr static uint32_t custom_timeout_ms = 1U << 23U;
constexpr static uint32_t supported_compression_version = 1U << 25U;
constexpr static uint32_t random_delay = 1U << 26U;
constexpr static uint32_t return_view_number = 1U << 27U;
constexpr static uint32_t persistent_query = 1U << 28U;
constexpr static uint32_t ALL = 0x1ebd00df;
} // namespace rpc_invoke_req_extra_flags

namespace rpc_req_result_extra_flags {
constexpr static uint32_t binlog_pos = 1U << 0U;
constexpr static uint32_t binlog_time = 1U << 1U;
constexpr static uint32_t engine_pid = 1U << 2U;
constexpr static uint32_t request_size = 1U << 3U;
constexpr static uint32_t response_size = 1U << 3U;
constexpr static uint32_t failed_subqueries = 1U << 4U;
constexpr static uint32_t compression_version = 1U << 5U;
constexpr static uint32_t stats = 1U << 6U;
constexpr static uint32_t epoch_number = 1U << 27U;
constexpr static uint32_t view_number = 1U << 27U;
constexpr static uint32_t ALL = 0x0800007f;
} // namespace rpc_req_result_extra_flags

} // namespace common
} // namespace tl
} // namespace vk

// Compiler for PHP (aka KPHP)
// Copyright (c) 2021 LLC «V Kontakte»
// Distributed under the GPL v3 License, see LICENSE.notice.txt

#pragma once

#include "common/algorithms/hashes.h"
#include "common/wrappers/string_view.h"

#include "runtime/kphp_core.h"
#include "runtime/memory_usage.h"
#include "runtime/pdo/abstract_pdo_driver.h"
#include "runtime/refcountable_php_classes.h"

namespace pdo {
  void init_lib();
  void free_lib();
} // namespace pdo


struct C$PDO : public refcountable_polymorphic_php_classes<abstract_refcountable_php_interface> {
  pdo::AbstractPdoDriver *driver{nullptr}; // TODO: script memory leak, clean in destructor?

  virtual ~C$PDO() = default;

  virtual const char *get_class() const noexcept {
    return "PDO";
  }

  virtual int32_t get_hash() const noexcept {
    return static_cast<int32_t>(vk::std_hash(vk::string_view(C$PDO::get_class())));
  }

  virtual void accept(InstanceMemoryEstimateVisitor &) {}
};

class_instance<C$PDO> f$PDO$$__construct(class_instance<C$PDO> const &v$this, const string &dsn, const Optional<string> &username = {},
                                                                const Optional<string> &password = {}, const Optional<array<mixed>> &options= {}) noexcept;

struct C$PDOStatement;

class_instance<C$PDOStatement> f$PDO$$prepare(const class_instance<C$PDO> &v$this, const string &query, const array<mixed> &options = {}) noexcept;


class_instance<C$PDOStatement> f$PDO$$query(const class_instance<C$PDO> &v$this, const string &query, Optional<int64_t> fetchMode = {});


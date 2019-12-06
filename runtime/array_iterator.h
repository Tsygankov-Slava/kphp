#pragma once

#include "common/type_traits/list_of_types.h"

template<class T>
class array_iterator {
private:
  template<typename T1>
  friend class array;

  friend class array_iterator<const T>;

  template<typename S>
  using const_conditional_t = std::conditional_t<std::is_const<T>::value, const S, S>;

public:
  using value_type = T;
  using array_type = const_conditional_t<array<std::remove_const_t<T>>>;
  using key_type = typename array_type::key_type;
  using inner_type = const_conditional_t<typename array_type::array_inner>;
  using list_hash_type = const_conditional_t<typename array_type::list_hash_entry>;
  using int_hash_type = const_conditional_t<typename array_type::int_hash_entry>;
  using string_hash_type = const_conditional_t<typename array_type::string_hash_entry>;

  inline constexpr array_iterator() noexcept __attribute__ ((always_inline)) = default;

  inline array_iterator(inner_type *self, list_hash_type *entry) noexcept __attribute__ ((always_inline)):
    self_(self),
    entry_(entry) {
  }

  inline array_iterator(const array_iterator<std::remove_const_t<value_type>> &other) noexcept __attribute__ ((always_inline)):
    array_iterator(other.self_, other.entry_) {
  }

  inline value_type &get_value() noexcept __attribute__ ((always_inline)) {
    return self_->is_vector() ? *reinterpret_cast<value_type *>(entry_) : static_cast<int_hash_type *>(entry_)->value;
  }

  inline const value_type &get_value() const noexcept __attribute__ ((always_inline)) {
    return self_->is_vector() ? *reinterpret_cast<value_type *>(entry_) : static_cast<int_hash_type *>(entry_)->value;
  }

  inline key_type get_key() const noexcept __attribute__ ((always_inline)) {
    if (self_->is_vector()) {
      return key_type{static_cast<int>(reinterpret_cast<value_type *>(entry_) - reinterpret_cast<value_type *>(self_->int_entries))};
    }
    return self_->is_string_hash_entry(static_cast<string_hash_type *>(entry_))
           ? static_cast<string_hash_type *>(entry_)->get_key()
           : static_cast<int_hash_type *>(entry_)->get_key();
  }

  inline bool is_string_key() const noexcept __attribute__ ((always_inline)) {
    return !self_->is_vector() && self_->is_string_hash_entry(static_cast<const string_hash_type *>(entry_));
  }

  inline const_conditional_t<string> &get_string_key() noexcept __attribute__ ((always_inline)) {
    return static_cast<string_hash_type *>(entry_)->string_key;
  }

  inline const string &get_string_key() const noexcept __attribute__ ((always_inline)) {
    return static_cast<const string_hash_type *>(entry_)->string_key;
  }

  inline array_iterator &operator++() noexcept __attribute__ ((always_inline)) {
    entry_ = self_->is_vector()
             ? reinterpret_cast<list_hash_type *>(reinterpret_cast<value_type *>(entry_) + 1)
             : self_->next(static_cast<string_hash_type *>(entry_));
    return *this;
  }

  inline array_iterator &operator--() noexcept __attribute__ ((always_inline)) {
    entry_ = self_->is_vector()
             ? reinterpret_cast<list_hash_type *>(reinterpret_cast<value_type *>(entry_) - 1)
             : self_->prev(static_cast<string_hash_type *>(entry_));
    return *this;
  }

  inline bool operator==(const array_iterator &other) const noexcept __attribute__ ((always_inline)) {
    return entry_ == other.entry_;
  }

  inline bool operator!=(const array_iterator &other) const noexcept __attribute__ ((always_inline)) {
    return entry_ != other.entry_;
  }

  inline array_iterator &operator*() noexcept __attribute__ ((always_inline)) {
    return *this;
  }

  inline const array_iterator &operator*() const noexcept __attribute__ ((always_inline)) {
    return *this;
  }

  static inline array_iterator make_begin(std::add_const_t<array_type> &arr) noexcept __attribute__ ((always_inline)) {
    static_assert(std::is_const<T>{}, "expected to be const");
    return arr.is_vector()
           ? array_iterator{arr.p, arr.p->int_entries}
           : array_iterator{arr.p, arr.p->begin()};
  }

  static inline array_iterator make_begin(std::remove_const_t<array_type> &arr) noexcept __attribute__ ((always_inline)) {
    static_assert(!std::is_const<T>{}, "expected to be mutable");
    if (arr.is_vector()) {
      arr.mutate_if_vector_shared();
      return array_iterator{arr.p, arr.p->int_entries};
    }

    arr.mutate_if_map_shared();
    return array_iterator{arr.p, arr.p->begin()};
  }

  static inline array_iterator make_end(array_type &arr) noexcept __attribute__ ((always_inline)) {
    return arr.is_vector()
           ? array_iterator{arr.p, reinterpret_cast<list_hash_type *>(reinterpret_cast<value_type *>(arr.p->int_entries) + arr.p->int_size)}
           : array_iterator{arr.p, arr.p->end()};
  }

  static inline array_iterator make_middle(array_type &arr, int n) noexcept __attribute__ ((always_inline)) {
    int l = arr.count();

    if (arr.is_vector()) {
      if (n < 0) {
        n += l;
        if (n < 0) {
          return make_end(arr);
        }
      }
      if (n >= l) {
        return make_end(arr);
      }

      return array_iterator{arr.p, reinterpret_cast<list_hash_type *>(reinterpret_cast<value_type *>(arr.p->int_entries) + n)};
    }

    if (n < -l / 2) {
      n += l;
      if (n < 0) {
        return make_end(arr);
      }
    }

    if (n > l / 2) {
      n -= l;
      if (n >= 0) {
        return make_end(arr);
      }
    }

    const string_hash_type *result = nullptr;
    if (n < 0) {
      result = arr.p->end();
      while (n < 0) {
        n++;
        result = arr.p->prev(result);
      }
    } else {
      result = arr.p->begin();
      while (n > 0) {
        n--;
        result = arr.p->next(result);
      }
    }
    return array_iterator{arr.p, result};
  }

private:
  inner_type *self_{nullptr};
  list_hash_type *entry_{nullptr};
};
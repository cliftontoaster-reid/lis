#pragma once

#include <condition_variable>
#include <mutex>
#include <vector>

namespace fchat {
namespace detail {

template <typename T>
class MpscQueue {
 public:
  explicit MpscQueue(size_t capacity);

  MpscQueue(const MpscQueue&) = delete;
  MpscQueue& operator=(const MpscQueue&) = delete;
  MpscQueue(MpscQueue&&) = delete;
  MpscQueue& operator=(MpscQueue&&) = delete;

  // false: full or closed
  [[nodiscard]] bool push(T&& value);
  [[nodiscard]] bool push(const T& value);
  // wakes consumer, fails further pushes
  void close();

  // blocks until non-empty or closed, then swaps
  [[nodiscard]] bool drain(std::vector<T>& out);
  [[nodiscard]] bool try_drain(std::vector<T>& out);

 private:
  mutable std::mutex mutex_;
  std::condition_variable ready_;
  std::vector<T> data_;
  size_t capacity_;
  bool closed_{false};
};

}  // namespace detail

}  // namespace fchat

#include "mpsc.tpp"

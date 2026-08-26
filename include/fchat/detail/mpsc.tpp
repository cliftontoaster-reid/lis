#pragma once

#include <vector>

#include "mpsc.hpp"

namespace fchat {
namespace detail {

template <typename T>
MpscQueue<T>::MpscQueue(size_t capacity) : capacity_(capacity) {
  data_.reserve(capacity);
}

template <typename T>
bool MpscQueue<T>::push(T&& value) {
  std::lock_guard lock(mutex_);

  if (closed_ || data_.size() >= capacity_) {
    return false;
  }

  data_.emplace_back(std::move(value));
  ready_.notify_one();
  return true;
}
template <typename T>
[[nodiscard]] bool MpscQueue<T>::push(const T& value) {
  std::lock_guard lock(mutex_);

  if (closed_ || data_.size() >= capacity_) {
    return false;
  }

  data_.push_back(value);
  ready_.notify_one();
  return true;
}

template <typename T>
void MpscQueue<T>::close() {
  {
    std::lock_guard lock(mutex_);
    closed_ = true;
  }
  ready_.notify_all();
}

template <typename T>
bool MpscQueue<T>::drain(std::vector<T>& out) {
  out.clear();

  std::unique_lock lock(mutex_);
  ready_.wait(lock, [this] { return closed_ || !data_.empty(); });

  out.swap(data_);
  return closed_ && out.empty();
}
template <typename T>
[[nodiscard]] bool MpscQueue<T>::try_drain(std::vector<T>& out) {
  std::unique_lock<std::mutex> lock(mutex_, std::try_to_lock);

  if (!lock.owns_lock()) {
    return false;
  }
  out.swap(data_);
  return true;
}

}  // namespace detail
}  // namespace fchat
#pragma once

#include <cstddef>
#include <boost/lockfree/spsc_queue.hpp>

// This implementation of serial client protocol
// uses single consumer, single producer pattern
namespace serial {

template <typename T, size_t Capacity> class Producer {
private:
  using SPSC =
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>>;
  SPSC &queue_;

public:
  Producer() = delete;
  explicit Producer(SPSC &queue) : queue_{queue} {};

  inline bool push(const T &data) { return queue_.push(data); }
};

template <typename T, size_t Capacity> class Consumer {
private:
  using SPSC =
      boost::lockfree::spsc_queue<T, boost::lockfree::capacity<Capacity>>;
  SPSC &queue_;

public:
  Consumer() = delete;
  explicit Consumer(SPSC &queue) : queue_{queue} {};

  inline bool pop(T &data) { return queue_.pop(data); }
};

} // namespace serial

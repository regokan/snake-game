#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>

template <typename T>
class MessageQueue {
 public:
  void send(T &&msg) {
    std::lock_guard<std::mutex> lock(_mutex);
    _queue.push_back(std::move(msg));
    _cond.notify_one();
  }

  T receive() {
    std::unique_lock<std::mutex> lock(_mutex);
    _cond.wait(lock, [this] { return !_queue.empty(); });
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
  }

  std::optional<T> try_receive() {
    std::lock_guard<std::mutex> lock(_mutex);
    if (_queue.empty()) return std::nullopt;
    T msg = std::move(_queue.front());
    _queue.pop_front();
    return msg;
  }

  std::size_t size() {
    std::lock_guard<std::mutex> lock(_mutex);
    return _queue.size();
  }

 private:
  std::deque<T> _queue;
  std::mutex _mutex;
  std::condition_variable _cond;
};

#endif  // MESSAGEQUEUE_H

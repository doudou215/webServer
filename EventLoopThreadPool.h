#pragma once
#include <memory>
#include "EventLoopThread.h"
#include "base/Logging.h"
#include "base/noncopyable.h"

class EventLoopThreadPool : noncopyable {
 public:
  EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
  ~EventLoopThreadPool() { LOG << "~EventLoopThreadPool()"; }
  void start();
  EventLoop* getNextLoop();

 private:
  std::vector<std::shared_ptr<EventLoopThread>> threads_;
  EventLoop *baseLoop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<EventLoop*> loops_;
};

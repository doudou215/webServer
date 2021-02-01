#pragma once
#include "EventLoop.h"
#include "base/Condition.h"
#include "base/Mutex.h"
#include "base/noncopyable.h"
#include "base/Thread.h"

class EventLoopThread : noncopyable {
 public:
  EventLoopThread();
  ~EventLoopThread();
  EventLoop *startLoop();

 private:
  EventLoop *loop_;
  void threadFunc();
  Thread thread_;
  Condition cond_;
  MutexLock mutex_;
  bool exiting_;
};
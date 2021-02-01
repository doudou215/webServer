#include "EventLoopThread.h"
#include <functional>

EventLoopThread::EventLoopThread()
    : loop_(NULL),
      exiting_(false),
      mutex_(),
      cond_(mutex_),
      thread_(std::bind(&EventLoopThread::threadFunc, this), "EventLoopThread") {}

EventLoopThread::~EventLoopThread() {
    exiting_ = true;
    if (loop_) {
        loop_->quit();
        thread_.join();
    }
}

void EventLoopThread::threadFunc() {
    EventLoop loop;

    {
        MutexLockGuard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }

    loop.loop();
    loop_ = NULL;
}

EventLoop* EventLoopThread::startLoop() {
    thread_.start();

    {
        MutexLockGuard lock(mutex_);
        while (loop_ == NULL) cond_.wait();
    }
    return loop_;
}
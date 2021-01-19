#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
#include "Epoll.h"
#include "Util.h"
#include "base/CurrentThread.h"
#include "base/Logging.h"
#include "base/Thread.h"
#include "base/noncopyable.h"
#include <iostream>

class Epoll;

class EventLoop : noncopyable {
 public:
    typedef std::function<void()> Functors;
    EventLoop();
    ~EventLoop();
    void assertInLoopThread() { 
        assert(isInLoopTrehad());
    }
    void loop();
    bool isInLoopTrehad() { return threadId_ == CurrentTread::tid(); }

 private:
    
    std::vector<Functors> pendingFunctors;
    const pid_t threadId_;
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors;
    mutable MutexLock mutex_;
    std::shared_ptr<Epoll> poller_;
    int wakeupFd_;
    SPChannel wakeupChannel_;

    
};
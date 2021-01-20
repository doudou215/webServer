#pragma once
#include <functional>
#include <memory>
#include <vector>
#include "Channel.h"
//#include "Epoll.h"
#include "Util.h"
//#include "base/CurrentThread.h"
//#include "base/Logging.h"
//#include "base/Thread.h"
#include "base/noncopyable.h"
#include "base/Mutex.h"
#include <iostream>

class Epoll;

class EventLoop : noncopyable {
 public:
    typedef std::function<void()> Functors;
    EventLoop();
    ~EventLoop();
    void assertInLoopThread() { assert(isInLoopTrehad()); }
    bool isInLoopTrehad() { return threadId_ == CurrentTread::tid(); }
    void loop();
    void quit();
    void runInLoop(Functors &&cb);
    void queueInLoop(Functors &&cb);
    // what's this up to?
    void shutdown(shared_ptr<Channel> channel) { shutDownWR(channel->getFd()); }

    void removeFromPoller(SPChannel channel) {
        poller_->epoll_del(channel);
    }

    void updatePoller(SPChannel channel, int timeout = 0) {
        poller_->epoll_mod(channel, timeout);
    }

    void addPoller(SPChannel channel, int timeout = 0) {
        poller_->epoll_add(channel, timeout);
    }
 private:
    
    std::vector<Functors> pendingFunctors_;
    const pid_t threadId_;
    bool looping_;
    bool quit_;
    bool eventHandling_;
    bool callingPendingFunctors_;
    mutable MutexLock mutex_;
    std::shared_ptr<Epoll> poller_;
    int wakeupFd_;
    SPChannel wakeupChannel_;
    
    void wakeup();
    void handleRead();
    void handleConn();
    void doPendingFunctors();
};
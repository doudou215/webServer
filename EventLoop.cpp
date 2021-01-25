#include "EventLoop.h"
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>
#include "Util.h"
#include "base/Logging.h"

__thread EventLoop *t_loopInThisThread = 0;

int createEventFd() {
    int efd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (efd < 0) {
        LOG << "failed in eventFd";
        abort();
    } 
    return efd;
}

EventLoop::EventLoop():
    looping_(false),
    threadId_(Current::tid()),
    poller_(new Epoll(this)),
    wakeupFd_(createEventFd()),
    wakeupChannel_(new Channel(this, wakeupFd_)),
    callingPendingFunctors_(false),
    quit_(false),
    eventHandling_(false) {
        t_loopInThisThread = this;
        wakeupChannel_->setEvents(EPOLLIN | EPOLLET);
        wakeupChannel_->setReadHandler(std::bind(&EventLoop::handleRead(this)));
        wakeupChannel_->setConnHanler(std::bind(&EventLoop::hanleConn, this));
        poller_->epoll_add(wakeupChannel_, 0);
    }

EventLoop::~EventLoop() {
    assert(!looping_);
    close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::handleConn() {
    updatePoller(wakeupChannel_, 0);
}

void EventLoop::wakeup() {
    u_int64_t one = 1;
    int n = write(wakeupFd_, &one, sizeof(one));
    if (n < 0) {
        LOG << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead() {
    u_int64_t one;
    int n = read(wakeupFd_, &one, sizeof(one));
    if (n < 0) {
        LOG << "EventLoop::handleRead read " << n << " bytes instead of 8";
    }
    wakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

void EventLoop::runInLoop(Functors &&cb) {
    if (isInLoopTrehad())
        cb();
    else {
        //pendingFunctors.push_back(std::move(cb));
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functors &&cb) {
    {
        MutexLockGuard lock(&mutex_);
        pendingFunctors.push_back(std::move(cb));
    }
    if (!isInLoopTrehad() || callingPendingFunctors_)
        wakeup();
}

void EventLoop::loop() {
    assertInLoopThread();
    assert(!looping_);
    looping_ = true;
    quit_ = false;
    std::vector<SPChannel> ret;

    while (!quit_) {
        ret.clear();
        ret = poller_->poll();
        eventHandling_ = true;
        for (auto it : ret)
            it->handleEvents();
        eventHandling_ = false;
        doPendingFunctors();
        poller_->handleExpired();
    }

    looping_ = false;
}

void EventLoop::doPendingFunctors() {
    std::vector<Functors> do;
    {
        MutexLockGuard lock(mutex_);
        do.swap(pendingFunctors_);
    }
    callingPendingFunctors_ = true;
    for (auto it : do)
        it();
    callingPendingFunctors_ = false;
}

void EventLoop::quit() {
    quit_ = true;
    if (!isInLoopTrehad())
        wakeup();
}

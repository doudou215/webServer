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
    t_loopInThisThread = NULL;
}

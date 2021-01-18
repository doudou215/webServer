#include "EventLoop.h"

__thread EventLoop *t_loopInThisThread = 0;

EventLoop::EventLoop():
    looping_(false),
    threadId_(Current::tid()) {
        if (t_loopInThisThread) {

        }
        else {
            t_loopInThisThread = this;
        }
    }

EventLoop::~EventLoop() {
    assert(!looping_);
    t_loopInThisThread = NULL;
}

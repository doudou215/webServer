#include "EventLoopThreadPool.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop, int numThreads)
    : baseLoop_(loop), numThreads_(numThreads), started_(false), next_(0)
{
    if (numThreads_ <= 0) {
        LOG << "numThreads_ <= 0";
        abort();
    }
}

void EventLoopThreadPool::start() {
    baseLoop_->assertInLoopThread();

    for (int i = 0; i < numThreads_; i++) {
        // produce a new thread
        std::shared_ptr<EventLoopThread> t(new EventLoopThread());
        loops_.push_back(t->startLoop());
        threads_.push_back(t);
    }
}

EventLoop *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    EventLoop *loop = baseLoop_; // why for this assginmen ?

    if (!loops_.empty()) {
        loop = loops_[next_];
        next_ = (next_ + 1) % numThreads_;
    }
    return loop;
}
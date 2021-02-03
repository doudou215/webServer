#include "Thread.h"
#include <assert.h>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <memory>
#include "CurrentThread.h"

namespace CurrentThread {
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char *t_threadName = "default";
    void cacheTid();
}

pid_t gettid() { return static_cast<pid_t> (::syscall(SYS_gettid)); }

void CurrentThread::cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = static_cast<int>(gettid());
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
    }
}

struct ThreadData {
    typedef Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    pid_t *tid_;
    CountDownLatch *latch_;

    ThreadData(const ThreadFunc &func, const std::string name, pid_t *tid,
    CountDownLatch *latch)
        : func_(func), name_(name), tid_(tid), latch_(latch) {}
    
    // core function to invoke func_
    void runInThread() {
        *tid_ = CurrentThread::tid();
        tid_ = NULL;
        latch_->countDown();
        latch_ = NULL;

        CurrentThread::t_threadName = name_.empty() ? "Thread" : name_.c_str();
        prctl(PR_SET_NAME, CurrentThread::t_threadName);

        func_();
        CurrentThread::t_threadName = "finished";
    }
};

void* startThread(void *obj) {
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

Thread::Thread(const ThreadFunc &func, const std::string &name)
    : func_(func), 
      joined_(false),
      pthreadId_(0),
      tid_(0),
      latch_(1),
      started_(false),
      name_(name)
{
    setDefaultName();
}

Thread::~Thread() {
    if (started_ && !joined_)
        pthread_detach(pthreadId_);
}

void Thread::setDefaultName() {
    if (name_.empty()) {
        name_ = "Thread";
    }
}

void Thread::start() {
    assert(!started_);
    started_ = true;
    ThreadData *data = new ThreadData(func_, name_, &tid_, &latch_);

    if (pthread_create(&pthreadId_, NULL, &startThread, data)) {
        delete data;
        started_ = false;
    }
    else {
        latch_.wait(); // wait untill another thread start
        assert(tid_ > 0);
    }
}

int Thread::join() {
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}
#include "Timer.h"
#include <sys/time.h>
#include <unistd.h>
#include <queue>

// gettimeofday gives the number of seconds and microseconds since the Epoch
// and aslo the timeout means milliseconds, what a load of crap
TimerNode::TimerNode(SPHttpData data, int timeout): delete_(false), httpData_(data) {
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredTime_ = (now.tv_sec % 1000) * 1000 + now.tv_usec / 1000 + timeout;

}

TimerNode::~TimerNode() {
    if (httpData_)
        httpData_->handleClose();
}

// ct do not set delete_ attribute?
TimerNode::TimerNode(TimerNode &tn) : expiredTime_(0), httpData_(tn.httpData_) {

}

void TimerNode::update(int timeout) {
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredTime_ = (now.tv_sec % 1000) * 1000 + now.tv_usec / 1000 + timeout;
}

bool TimerNode::isValid() {
    size_t expiredTime;
    struct timeval now;
    gettimeofday(&now, NULL);
    expiredTime = (now.tv_sec % 1000) * 1000 + now.tv_usec / 1000;
    if (expiredTime > expiredTime_) {
        setDelete();
        //httpData_->reset();
        return false;
    }
    return true;
}

void TimerNode::clearReq() {
    if (httpData_)
        httpData_.reset();
    setDelete();
}

void TimerManager::add_timer(SPHttpData data, int timeout) {
    SPTimerNode node(new TimerNode(data, timeout));
    TimerQueue_.push(node);
    http_->linkTimer(node);  // not clear with this function?
}

void TimerManager::handleExpiredEvent() {
    while (!TimerQueue_.empty()) {
        auto node = TimerQueue_.top();
        if (node->isDelete())
            TimerQueue_.pop();
        else if (!node->isValid())
            TimerQueue_.pop();
        else
            break;
    }
}

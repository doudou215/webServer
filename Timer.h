#pragma once
#include <unistd.h>
#include <deque>
#include <memory>
#include <queue>
#include "base/Mutex.h"
#include "base/noncopyable.h"
#include "HttpData.h"
class HttpData;
//typedef std::shared_ptr<HttpData> SPHttpData;

class TimerNode {
 public:
    typedef std::shared_ptr<HttpData> SPHttpData;
    TimerNode(SPHttpData httpData, int timeout);
    ~TimerNode();
    TimerNode(TimerNode &tn);
    void update(int timeout);
    void clearReq();
    void setDelete() { delete_ = true; }
    bool isDelete() const { return delete_; }
    size_t getExpiredTime() const { return expiredTime_; }
    bool isValid();

 private:
    SPHttpData httpData_;
    bool delete_;
    size_t expiredTime_;
};

struct TimerComp {
    bool operator()(std::shared_ptr<TimerNode> &t1, std::shared_ptr<TimerNode> &t2) {
        return t1->getExpiredTime() > t2->getExpiredTime();
    }
};

class TimerManager {
 public:
    typedef std::shared_ptr<HttpData> SPHttpData;
    void add_timer(SPHttpData data, int timeout);
    void handleExpiredEvent();
 private:
    typedef std::shared_ptr<TimerNode> SPTimerNode;
    std::priority_queue<SPTimerNode, std::vector<SPTimerNode>, TimerComp> TimerQueue_;
};

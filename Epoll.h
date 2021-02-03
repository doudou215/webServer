#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "Channel.h"
//#include "HttpData.h"
#include "Timer.h"
class HttpData;

class Epoll {
 public:
    Epoll();
    ~Epoll();
    void epoll_add(SPChannel channel, int timeout);
    void epoll_mod(SPChannel channel, int timeout);
    void epoll_del(SPChannel channel);
    std::vector<std::shared_ptr<Channel>> poll();
    std::vector<std::shared_ptr<Channel>> getEventsFromRequest(int events_num);
    void add_timer(SPChannel channel, int timeout);
    int getEpollFd() { return epollFd_; }
    void handleExpired();

 private:
    static const int MAXFDS = 100000;
    int epollFd_;
    std::vector<epoll_event> events_; // to save epoll_wait return result
    std::shared_ptr<Channel> fd2chanel_[MAXFDS]; // see clearly this is not vector;
    std::shared_ptr<HttpData> fd2http_[MAXFDS]; // see clearly this is not vector;

    TimerManager timeManager_;
};
#include "Epoll.h"
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include "Util.h"
//#include "base/Logging.h"
#include <arpa/inet.h>
#include <iostream>

const int EVENTSNUM = 4096;
const int WAITING_TIME = 10000; // counts at milliseconds, and equal to 10s

Epoll::Epoll() : epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM) {
    assert(epollFd_ < 0);
}

Epoll::~Epoll() {

}

void Epoll::epoll_add(SPChannel channel, int timeout) {
    int fd = channel->getFd();
    if (timeout > 0) {
        add_timer(channel, timeout);
    }
    struct epoll_event ev;
    ev.events = channel->getEvents();
    ev.data.fd = fd;
    channel->equalAndUpateLastEvents();

    fd2chanel_[fd] = channel;

    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &ev) < 0) {
        perror("epoll_ctl add error");
        fd2chanel_[fd].reset();
    }
}

void Epoll::epoll_mod(SPChannel channel, int timeout) {
    int fd = channel->getFd();
    if (timeout > 0)
        add_timer(channel, timeout);

    if (!channel->equalAndUpateLastEvents()) {
        struct epoll_event ev;
        ev.events = channel->getEvents();
        ev.data.fd = fd;
        if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &ev) < 0) {
            perror("epoll_ctl mod error");
            fd2chanel_[fd].reset();
        }
    }
}

void Epoll::epoll_del(SPChannel channel) {
    int fd = channel->getFd();
    struct epoll_event ev;
    ev.events = channel->getLastEvents();
    ev.data.fd = fd;

    if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &ev) < 0) {
        perror("epoll_ctl del error");
    }

    fd2chanel_[fd].reset();
    fd2http_[fd].reset();
}

std::vector<std::shared_ptr<Channel>> Epoll::poll() {
    while(true) {
        int events_num = epoll_wait(epollFd_, &(*events_.begin()), events_.size(), WAITING_TIME);
        if (events_num < 0)
            perror("epoll_wait error");
        std::vector<SPChannel> request = getEventsFromRequest(events_num);
        if (request.size() > 0)
            return request;
    }
}

std::vector<std::shared_ptr<Channel>> Epoll::getEventsFromRequest(int events_num) {
    
    std::vector<SPChannel> ret;

    for (int i = 0; i < events_num; i++) {
        int fd = events_[i].data.fd;
        SPChannel channel = fd2chanel_[fd];
        if (channel) {
            channel->setRevents(events_[i].events);
            channel->setEvents(0);
            ret.push_back(channel);
        }
        else
            LOG << "request channel is no valid";
    }
    return ret;
}

void Epoll::handlerExpired() {
    timeManager_.handleExpiredEvent();
}

void Epoll::add_timer(SPChannel channel, int timeout) {
    std::shared_ptr<HttpData> httpHolder = channel->getHolder();

    if (httpHolder) {
        timeManager_.add_timer(httpHolder, timeout);
    }
    else
        LOG << "timer add failed";
    
}
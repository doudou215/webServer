#pragma once
#include <sys/epoll.h>
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
// #include "Timer.h"

class EventLoop;
class HttpData;

class Channel {
 public:
    typedef std::function<void()> CallBack;
    Channel(EventLoop *loop, int fd);
    Channel(EventLoop *loop);

    void setReadHandler(CallBack &&cb) {
        readHandler_ = cb;
    }

    void setWriteHandler(CallBack &&cb) {
        writeHandler_ = cb;
    }

    void setErrorHandler(CallBack &&cb) {
        errorHandler_ = cb;
    }

    void setConnHanler(CallBack &&cb) {
        connHandler_ = cb;
    }

    void setHolder(std::shared_ptr<HttpData> holder) {
        holder_ = holder;
    }
    
    std::shared_ptr<HttpData> getHolder() {
        return holder_.lock();
    }

    int getFd() { return fd_; }

    void setFd(int fd) { fd_ = fd; }

    void setRevents( __uint32_t ev) { revent = ev; }

    void setEvents( __uint32_t ev) { event = ev; }

    __uint32_t getLastEvents() { return lastevent; }

    __uint32_t& getEvents() { return event; }

    bool equalAndUpateLastEvents() {
        bool ret = (lastevent == event);
        lastevent = event;
        return ret;
    }

    void handleRead();
    void handleWrite();
    void handleError();
    void handlerConn();
    void handlerEvents() {
        event = 0;
        // peer stop connection, we will receive a FIN
        if ((revent & EPOLLHUP) && !(revent & EPOLLIN)) {
            event = 0;
            return;
        }
        if (revent & EPOLLERR) {
            event = 0;
            return;
        }
        // EPOLLRDHUP peer stops writing, we cann't read any data from the fd.
        if (revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            handleRead();
        if (revent & EPOLLOUT)
            handleWrite();
        handlerConn();
    }

 private:
    EventLoop *loop_;
    int fd_;

    std::weak_ptr<HttpData> holder_;

    __uint32_t event;
    __uint32_t revent;
    __uint32_t lastevent;

    CallBack readHandler_;
    CallBack writeHandler_;
    CallBack errorHandler_;
    CallBack connHandler_;
};

typedef std::shared_ptr<Channel> SPChannel;
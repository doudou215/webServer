#include "Channel.h"

#include <unistd.h>
#include <cstdlib>
#include <iostream>

#include <queue>
#include "Epoll.h"
#include "EventLoop.h"
#include "Util.h"

Channel::Channel(EventLoop *loop) : loop_(loop), fd_(0), event(0), lastevent(0) {

}

Channel::Channel(EventLoop *loop, int fd) : loop_(loop), fd_(fd), event(0), lastevent(0) {

}

void Channel::handleConn() {
    if (connHandler_)
        connHandler_();
}

void Channel::handleRead() {
    if (readHandler_)
        readHandler_();
}

void Channel::handleWrite() {
    if (writeHandler_)
        writeHandler_();
}

void Channel::handleError() {
    if (errorHandler_)
        errorHandler_();
}
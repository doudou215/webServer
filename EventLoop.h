#include <pthread.h>
#include "./base/noncopyable.h"
#include <assert.h>



class EventLoop : noncopyable {
 public:
    EventLoop();
    ~EventLoop();
    void assertInLoopThread() { 
        assert(isInLoopTrehad());
    }
    void loop();
    bool isInLoopTrehad() { return threadId_ == CurrentTread::tid(); }

 private:
    const pid_t threadId_;
    bool looping_;
};
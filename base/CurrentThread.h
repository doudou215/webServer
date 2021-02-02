#pragma once
#include <stdint.h>

namespace CurrentThread 
{
// internal
  extern __thread int t_cachedTid;
  extern __thread char t_tidString[32];
  extern __thread int t_tidStringLength;
  extern __thread const char* t_threadName;
  void cacheTid(); // see defination in Thread.cpp

  inline int tid() 
  {
    if (__builtin_expect(t_cachedTid == 0, 0)) 
      cacheTid();
    return t_cachedTid;
  }
// expect the t_cahedTid != 0,and if it == 0, invoke cacheTid()
  inline const char* tidString()  // for logging
  {
    return t_tidString;
  }

  inline int tidStringLength()  // for logging
  {
    return t_tidStringLength;
  }

  inline const char* name() { return t_threadName; }

}
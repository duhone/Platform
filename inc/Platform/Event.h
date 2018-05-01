#pragma once
#include <memory>

namespace CR::Platform {
  //creates a manual reset event, starts in signaled state
  struct Event {
    Event();
    ~Event();
    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;

    void Reset();
    void Wait();
    void Notify();
  private:
    std::unique_ptr<class EventImpl> m_impl;
  };
}
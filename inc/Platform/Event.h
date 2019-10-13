#pragma once
#include <memory>

namespace CR::Platform {
	// creates a manual reset event, starts in signaled state
	struct Event {
		Event();
		~Event();
		Event(const Event&) = delete;
		Event(Event&&);
		Event& operator=(const Event&) = delete;
		Event& operator                =(Event&&);

		void Reset();
		void Wait();
		void Notify();

	  private:
		std::unique_ptr<class EventImpl> m_impl;
	};

	inline Event::Event(Event&& other) { *this = std::move(other); }
	inline Event& Event::operator=(Event&& other) {
		if(this == &other) return *this;
		m_impl = std::move(other.m_impl);
		return *this;
	}
}    // namespace CR::Platform
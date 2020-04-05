#pragma once
#include <memory>

namespace CR::Platform {
	// creates a manual reset event, starts in signaled state
	struct Event final {
		Event();
		~Event();
		Event(const Event&) = delete;
		Event(Event&& a_other) noexcept;
		Event& operator=(const Event&) = delete;
		Event& operator                =(Event&& a_other) noexcept;

		void Reset();
		void Wait();
		void Notify();

	  private:
		std::unique_ptr<struct EventData> m_data;
	};
}    // namespace CR::Platform

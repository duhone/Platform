#include "Platform/Event.h"
#include <windows.h>

namespace CR
{
	namespace Platform
	{
		class EventImpl
		{
		public:
			EventImpl();
			~EventImpl();

			void Reset();
			void Wait();
			void Notify();
		private:
			HANDLE m_eventHandle;
		};
	}
}

using namespace CR::Platform;

//impl
EventImpl::EventImpl() { m_eventHandle = CreateEvent(nullptr, TRUE, TRUE, nullptr); }
EventImpl::~EventImpl() { CloseHandle(m_eventHandle);}
void EventImpl::Reset() { ResetEvent(m_eventHandle); }
void EventImpl::Wait() { WaitForSingleObject(m_eventHandle, INFINITE); }
void EventImpl::Notify() { SetEvent(m_eventHandle); }

//wrapper
Event::Event() : m_impl(std::make_unique<EventImpl>()) {}
Event::~Event() {}
void Event::Reset() { m_impl->Reset(); }
void Event::Wait() { m_impl->Wait(); }
void Event::Notify() { m_impl->Notify(); }
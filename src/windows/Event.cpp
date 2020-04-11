#include "Platform/Event.h"

#include "core/Log.h"

#include <windows.h>

namespace CR::Platform {
	struct EventData {
		HANDLE EventHandle;
	};
}    // namespace CR::Platform

using namespace CR;
using namespace CR::Platform;

// impl
Event::Event() {
	m_data              = std::make_unique<EventData>();
	m_data->EventHandle = CreateEvent(nullptr, TRUE, TRUE, nullptr);
}

Event::~Event() {
	if(!m_data) { return; }
	CloseHandle(m_data->EventHandle);
}

Event::Event(Event&& a_other) noexcept {
	*this = std::move(a_other);
}
Event& Event::operator=(Event&& a_other) noexcept {
	if(this == &a_other) { return *this; }
	m_data = std::move(a_other.m_data);
	return *this;
}

void Event::Reset() {
	Core::Log::Assert((bool)m_data, "Tried to use an moved from Event");
	ResetEvent(m_data->EventHandle);
}
void Event::Wait() {
	Core::Log::Assert((bool)m_data, "Tried to use an moved from Event");
	WaitForSingleObject(m_data->EventHandle, INFINITE);
}
void Event::Notify() {
	Core::Log::Assert((bool)m_data, "Tried to use an moved from Event");
	SetEvent(m_data->EventHandle);
}

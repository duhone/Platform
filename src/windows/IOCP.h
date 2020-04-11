#pragma once
#include <functional>
#include <memory>
#include <windows.h>

#include <3rdParty/function2.h>

namespace CR::Platform {
	class IOCPPort final {
	  public:
		using CompletionCallback_t = fu2::unique_function<void(OVERLAPPED*, std::size_t)>;

		IOCPPort() = default;
		IOCPPort(HANDLE a_handle, CompletionCallback_t a_completion);
		~IOCPPort()               = default;
		IOCPPort(const IOCPPort&) = delete;
		IOCPPort& operator=(const IOCPPort&) = delete;
		IOCPPort(IOCPPort&& a_other) noexcept;
		IOCPPort& operator=(IOCPPort&& a_other) noexcept;

		void RunContinuation(OVERLAPPED* a_result, std::size_t a_msgSize) { m_completion(a_result, a_msgSize); }

	  private:
		CompletionCallback_t m_completion;
	};

	inline IOCPPort::IOCPPort(IOCPPort&& a_other) noexcept { *this = std::move(a_other); }

	inline IOCPPort& IOCPPort::operator=(IOCPPort&& a_other) noexcept {
		m_completion = std::move(a_other.m_completion);
		return *this;
	}

}    // namespace CR::Platform

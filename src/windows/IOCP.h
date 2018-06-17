#pragma once
#include <functional>
#include <memory>
#include <windows.h>

namespace CR::Platform {
	struct IIOCPort {
		using CompletionCallbackT = std::function<void(OVERLAPPED*, std::size_t)>;
		virtual ~IIOCPort()       = default;
		IIOCPort(const IIOCPort&) = delete;
		IIOCPort& operator=(const IIOCPort&) = delete;

	  protected:
		IIOCPort() = default;
	};

	std::unique_ptr<IIOCPort> OpenIOCPPort(HANDLE a_handle, IIOCPort::CompletionCallbackT a_completion);
}    // namespace CR::Platform
#pragma once
#include <memory>
#include <windows.h>
#include <functional>

namespace CR
{
	namespace Platform
	{
		struct IIOCPort
		{
			using CompletionCallbackT = std::function<void(OVERLAPPED*, std::size_t)>;
			virtual ~IIOCPort() = default;
		};

		std::unique_ptr<IIOCPort> OpenIOCPPort(HANDLE a_handle, IIOCPort::CompletionCallbackT a_completion);
	}
}
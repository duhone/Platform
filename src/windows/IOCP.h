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
			virtual ~IIOCPort() = default;
		};

		std::unique_ptr<IIOCPort> OpenIOCPPort(HANDLE a_handle, std::function<void(OVERLAPPED*)> a_completion);
	}
}
#pragma once
#include <memory>
#include <chrono>

namespace CR
{
	namespace Platform
	{
		struct IProcess
		{
			virtual ~IProcess() = default;
			//Returns false if timed out waiting for process to close, or if some other error occured
			virtual bool WaitForClose(const std::chrono::milliseconds& a_maxWait) = 0;
		};

		std::unique_ptr<IProcess> CRCreateProcess(const char* a_executablePath, const char* a_commandLine);
	}
}
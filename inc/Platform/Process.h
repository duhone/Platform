#pragma once
#include <chrono>
#include <memory>
#include <optional>

namespace CR::Platform {
	struct IProcess {
	  protected:
		IProcess() = default;

	  public:
		virtual ~IProcess()       = default;
		IProcess(const IProcess&) = delete;
		IProcess& operator=(const IProcess&) = delete;

		// Returns false if timed out waiting for process to close, or if some other error occured
		virtual bool WaitForClose(const std::chrono::milliseconds& a_maxWait) = 0;

		virtual std::optional<int32_t> GetExitCode() = 0;
	};

	std::unique_ptr<IProcess> CRCreateProcess(const char* a_executablePath, const char* a_commandLine);
}    // namespace CR::Platform
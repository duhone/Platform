#include "Platform/Process.h"

#include "core/Log.h"

#include <Windows.h>
#include <string>

using namespace std;

namespace CR::Platform {
	struct ProcessData {
		HANDLE m_processHandle;
		uint32_t m_processID;
	};
}    // namespace CR::Platform

using namespace CR;
using namespace CR::Platform;

Process::Process(const char* a_executablePath, const char* a_commandLine) : m_data(new ProcessData) {
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(processInfo));

	string commandLine = a_executablePath;
	commandLine += " ";
	commandLine += a_commandLine;
	auto created = CreateProcess(nullptr, (LPSTR)commandLine.c_str(), nullptr, nullptr, false, 0, nullptr, nullptr,
	                             &startupInfo, &processInfo);
	Core::Log::Require(created, "Failed to create process {}", a_executablePath);
	CloseHandle(processInfo.hThread);

	m_data->m_processHandle = processInfo.hProcess;
	m_data->m_processID     = processInfo.dwProcessId;
}

Process::~Process() {
	if(m_data) { CloseHandle(m_data->m_processHandle); }
}

Process::Process(Process&& a_other) noexcept {
	*this = std::move(a_other);
}

Process& Process::operator=(Process&& a_other) noexcept {
	m_data = std::move(a_other.m_data);
	return *this;
}

bool Process::WaitForClose(const std::chrono::milliseconds& a_maxWait) {
	return WaitForSingleObject(m_data->m_processHandle, static_cast<DWORD>(a_maxWait.count())) == WAIT_OBJECT_0;
}

std::optional<int32_t> Process::GetExitCode() const {
	int32_t exitCode = 0;
	if(GetExitCodeProcess(m_data->m_processHandle, (DWORD*)&exitCode) == FALSE) { return optional<int32_t>{}; }
	if(exitCode == STILL_ACTIVE) { return optional<int32_t>{}; }
	return optional<int32_t>{exitCode};
}

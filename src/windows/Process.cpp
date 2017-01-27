#include "Platform/Process.h"
#include <Windows.h>

namespace CR
{
	namespace Platform
	{
		class Process : public IProcess
		{
		public:
			Process(HANDLE a_processHandle, uint32_t a_processID);
			virtual ~Process();
			bool WaitForClose(const std::chrono::milliseconds& a_maxWait) override;
		private:
			HANDLE m_processHandle;
			uint32_t m_processID;
		};
	}
}

using namespace CR::Platform;

Process::Process(HANDLE a_processHandle, uint32_t a_processID) : m_processHandle(a_processHandle),
	m_processID(a_processID)
{

}

Process::~Process()
{
	CloseHandle(m_processHandle);
}

bool Process::WaitForClose(const std::chrono::milliseconds& a_maxWait)
{
	return WaitForSingleObject(m_processHandle, static_cast<DWORD>(a_maxWait.count())) == WAIT_OBJECT_0;
}

std::unique_ptr<IProcess> CR::Platform::CRCreateProcess(const char* executablePath, const char* a_commandLine)
{
	STARTUPINFO startupInfo;
	memset(&startupInfo, 0, sizeof(startupInfo));
	startupInfo.cb = sizeof(startupInfo);
	PROCESS_INFORMATION processInfo;
	memset(&processInfo, 0, sizeof(processInfo));

	auto created = CreateProcess(executablePath, (LPSTR)a_commandLine, nullptr, nullptr, false, 0, nullptr, 
		nullptr, &startupInfo, &processInfo);
	if (!created)
		return nullptr;
	CloseHandle(processInfo.hThread);

	return std::make_unique<Process>(processInfo.hProcess, processInfo.dwProcessId);
}

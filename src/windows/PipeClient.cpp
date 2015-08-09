#include "PipeClient.h"
#include <Windows.h>
#include <stdexcept>
#include <cassert>
#include <core\literals.h>
#include <thread>

namespace CR
{
	namespace Platform
	{
		class PipeClient : public IPipeClient
		{
		public:
			PipeClient(HANDLE a_pipeHandle, IPipeClient::MsgHandler&& a_msgHandler);
			virtual ~PipeClient();
			void SendPipeMessage(void* a_msg, size_t a_msgSize) override;
		private:
			void RunMsgHandler();

			HANDLE m_pipeHandle;
			IPipeClient::MsgHandler m_msgHandler;
			std::thread m_msgThread;
			volatile bool m_closed{false};
		};
	}
}

using namespace CR::Platform;
using namespace CR::Core::Literals;

PipeClient::PipeClient(HANDLE a_pipeHandle, IPipeClient::MsgHandler&& a_msgHandler) : m_pipeHandle(a_pipeHandle),
	m_msgHandler(std::move(a_msgHandler))
{
	m_msgThread = std::thread([this]() { this->RunMsgHandler(); });
}

PipeClient::~PipeClient()
{
	m_closed = true;
	CloseHandle(m_pipeHandle);
	if(m_msgThread.joinable())
		m_msgThread.join();
}

void PipeClient::SendPipeMessage(void* a_msg, size_t a_msgSize)
{
	DWORD bytesWritten = 0;
	assert(a_msgSize < 4_Kb); //arbitrary, but if larger than this might want to switch to an async implementation
	BOOL result = WriteFile(m_pipeHandle, a_msg, static_cast<DWORD>(a_msgSize), &bytesWritten, nullptr);
	if(!result || bytesWritten != a_msgSize)
		throw std::runtime_error("Failed to write message");
}

void PipeClient::RunMsgHandler()
{
	char msg[4_Kb];
	DWORD bytesRead;
	while(!m_closed)
	{
		if(ReadFile(m_pipeHandle, msg, static_cast<DWORD>(4_Kb), &bytesRead, nullptr))
		{
			m_msgHandler(msg, bytesRead);
		}
		else
		{
			if(GetLastError() == ERROR_MORE_DATA)
				throw std::runtime_error("Currently msg's must be 4Kb or smaller");
		}
	}
}

std::unique_ptr<IPipeClient> CR::Platform::CreatePipeClient(const char* a_name, IPipeClient::MsgHandler a_msgHandler)
{
	assert(a_msgHandler); //msg handler is required
	auto pipeHandle = CreateFile(a_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	if(pipeHandle == INVALID_HANDLE_VALUE)
		return nullptr;
	DWORD pipeState = PIPE_READMODE_MESSAGE;
	SetNamedPipeHandleState(pipeHandle, &pipeState, nullptr, nullptr);

	return std::make_unique<PipeClient>(pipeHandle, std::move(a_msgHandler));
}
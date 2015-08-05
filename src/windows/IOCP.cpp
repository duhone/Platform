#include "IOCP.h"
#include <core\Singleton.h>
#include <thread>
#include <unordered_map>

namespace CR
{
	namespace Platform
	{
		class IOCPPort : public IIOCPort
		{
		public:
			IOCPPort(HANDLE a_handle, std::function<void(OVERLAPPED*)> a_completion);
			virtual ~IOCPPort() = default;
			void RunContinuation(OVERLAPPED* a_result) { m_completion(a_result); }
		private:
			std::function<void(OVERLAPPED*)> m_completion;
		};

		//Doesn't scale past one thread at the moment. would require some redesign to do so.
		class IOCPThread : public CR::Core::Singleton<IOCPThread>
		{
		public:
			friend CR::Core::Singleton<IOCPThread>;
			void RegisterIOCPPort(IOCPPort* a_port, HANDLE a_handle);
		private:
			IOCPThread();
			~IOCPThread();

			void RunIOCPThread();

			std::thread m_iocpThread;
			HANDLE m_iocpHandle;
		};
	}
}

using namespace CR::Platform;

IOCPPort::IOCPPort(HANDLE a_handle, std::function<void(OVERLAPPED*)> a_completion) : m_completion(std::move(a_completion))
{
	IOCPThread::Instance().RegisterIOCPPort(this, a_handle);
}

IOCPThread::IOCPThread()
{
	m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
	m_iocpThread = std::thread{[this]() { this->RunIOCPThread(); }};
}

IOCPThread::~IOCPThread()
{
	CloseHandle(m_iocpHandle);
	if(m_iocpThread.joinable())
		m_iocpThread.join();
}

void IOCPThread::RunIOCPThread()
{
	DWORD msgSize;
	unsigned long long completionKey;
	OVERLAPPED* msg;
	while(GetQueuedCompletionStatus(m_iocpHandle, &msgSize, &completionKey, &msg, INFINITE))
	{
		((IOCPPort*)completionKey)->RunContinuation(msg);
	}
}

void IOCPThread::RegisterIOCPPort(IOCPPort* a_port, HANDLE a_handle)
{
	CreateIoCompletionPort(a_handle, m_iocpHandle, (ULONG_PTR)a_port, 1);
}

std::unique_ptr<IIOCPort> CR::Platform::OpenIOCPPort(HANDLE a_handle, std::function<void(OVERLAPPED*)> a_completion)
{
	return std::make_unique<IOCPPort>(a_handle, std::move(a_completion));
}
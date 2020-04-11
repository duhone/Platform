#include "IOCP.h"

#include <thread>
#include <unordered_map>

using namespace CR;
using namespace CR::Platform;

namespace {
	// Doesn't scale past one thread at the moment. would require some redesign to do so.
	class IOCPThread {
	  public:
		void RegisterIOCPPort(IOCPPort* a_port, HANDLE a_handle);
		IOCPThread();
		~IOCPThread();
		IOCPThread(const IOCPThread&) = delete;
		IOCPThread& operator=(const IOCPThread&) = delete;

		void RunIOCPThread();

	  private:
		std::thread m_iocpThread;
		HANDLE m_iocpHandle;
	};

	IOCPThread& GetIOCPThread() {
		static IOCPThread iocpThread;
		return iocpThread;
	}
}    // namespace

using namespace CR::Platform;

IOCPPort::IOCPPort(HANDLE a_handle, CompletionCallback_t a_completion) : m_completion(std::move(a_completion)) {
	GetIOCPThread().RegisterIOCPPort(this, a_handle);
}

IOCPThread::IOCPThread() {
	m_iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 1);
	m_iocpThread = std::thread{[this]() { this->RunIOCPThread(); }};
}

IOCPThread::~IOCPThread() {
	CloseHandle(m_iocpHandle);
	if(m_iocpThread.joinable()) m_iocpThread.join();
}

void IOCPThread::RunIOCPThread() {
	DWORD msgSize;
	ULONG_PTR completionKey;
	OVERLAPPED* msg;
	while(GetQueuedCompletionStatus(m_iocpHandle, &msgSize, &completionKey, &msg, INFINITE)) {
		((IOCPPort*)completionKey)->RunContinuation(msg, msgSize);
	}
}

void IOCPThread::RegisterIOCPPort(IOCPPort* a_port, HANDLE a_handle) {
	CreateIoCompletionPort(a_handle, m_iocpHandle, (ULONG_PTR)a_port, 1);
}

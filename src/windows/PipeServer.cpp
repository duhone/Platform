#include "PipeServer.h"
#include <Window.h>
#include "IOCP.h"
#include <core\literals.h>
#include <concurrent_queue.h>
#include <cassert>
#include <algorithm>

using namespace CR::Core::Literals;

namespace CR
{
	namespace Platform
	{
		class PipeServer : public IPipeServer
		{
		public:
			PipeServer(HANDLE a_pipeHandle, WriteFinishedT&& a_writeFinished, ReadFinishedT&& a_readFinished);
			virtual ~PipeServer();
			void WriteMsg(void* a_msg, size_t a_msgSize) override;
		private:
			enum OperationType
			{
				Connection,
				Read,
				Write
			};
			struct Operation : public OVERLAPPED
			{
				OperationType opType;
				void* opData;
				size_t opDataSize;
			};
			Operation* GetOperation();
			void ReturnOperation(Operation* a_operation);
			void OnOverlapped(OVERLAPPED* a_overlapped, size_t a_size);
			void OnConnection(Operation* a_operation, size_t a_size);
			void OnRead(Operation* a_operation, size_t a_size);
			void OnWrite(Operation* a_operation, size_t a_size);
			void ReadMsg();

			HANDLE m_pipeHandle; 
			WriteFinishedT m_writeFinished;
			ReadFinishedT m_readFinished;
			std::unique_ptr<IIOCPort> m_iocpPort;
			concurrency::concurrent_queue<Operation*> m_operationPool;
			size_t m_totalCreatedOps{0}; //for debugging
			using OperationHandlerT = void (PipeServer::*)(Operation*, size_t);
			OperationHandlerT m_operationHandlers[3] = {&PipeServer::OnConnection, &PipeServer::OnRead, &PipeServer::OnWrite};
			char m_readBuffer[4_Kb];
			volatile bool m_finished{false};
		};
	}
}

using namespace CR::Platform;

PipeServer::PipeServer(HANDLE a_pipeHandle, WriteFinishedT&& a_writeFinished, ReadFinishedT&& a_readFinished) : 
	m_pipeHandle(a_pipeHandle),	m_writeFinished(std::move(a_writeFinished)), m_readFinished(std::move(a_readFinished))
{
	m_iocpPort = OpenIOCPPort(m_pipeHandle, [this](auto a_overlapped, auto a_size) { 
		this->OnOverlapped(a_overlapped, a_size); });

	auto connectOp = GetOperation();
	connectOp->opType = OperationType::Connection;
	ConnectNamedPipe(m_pipeHandle, connectOp);
}

PipeServer::~PipeServer()
{
	m_finished = true;
	m_iocpPort.reset();
	CloseHandle(m_pipeHandle);
	//assert(m_totalCreatedOps == m_operationPool.unsafe_size());
	std::for_each(m_operationPool.unsafe_begin(), m_operationPool.unsafe_end(), [](auto op) { delete op; });
}

PipeServer::Operation* PipeServer::GetOperation()
{
	Operation* result{nullptr};
	while(!m_operationPool.try_pop(result))
	{
		for(int i = 0; i < 64; ++i)
		{
			m_operationPool.push(new Operation);
		}
		m_totalCreatedOps += 64;
	}
	assert(result);
	memset(result, 0, sizeof(Operation));
	return result;
}

void PipeServer::ReturnOperation(Operation* a_operation)
{
	if(a_operation->opType != OperationType::Read)
		m_operationPool.push(a_operation);
}

void PipeServer::OnOverlapped(OVERLAPPED* a_overlapped, size_t a_size)
{
	auto op = (Operation*)a_overlapped;
	assert(op->opType == OperationType::Read || op->opDataSize == a_size);
	(this->*m_operationHandlers[op->opType])(op, a_size);
	ReturnOperation(op);
}

void PipeServer::OnConnection(Operation* a_operation, size_t)
{
	a_operation;
	assert(a_operation->opType == OperationType::Connection);
	ReadMsg();
}

void PipeServer::OnRead(Operation* a_operation, size_t a_size)
{
	assert(a_operation->opType == OperationType::Read);
	m_readFinished(a_operation->opData, a_size);
	ReadMsg();
}

void PipeServer::OnWrite(Operation* a_operation, size_t)
{
	assert(a_operation->opType == OperationType::Write);
	m_writeFinished(a_operation->opData, a_operation->opDataSize);
}

void PipeServer::ReadMsg()
{
	if(m_finished)
		return;
	auto op = GetOperation();
	op->opType = OperationType::Read;
	op->opData = m_readBuffer;
	op->opDataSize = sizeof(m_readBuffer);
	DWORD bytesRead;
	auto result = ReadFile(m_pipeHandle, m_readBuffer, (DWORD)sizeof(m_readBuffer), &bytesRead, op);
	auto error = GetLastError();
	if(result || (!result && error != ERROR_IO_PENDING))
	{
		OnRead(op, bytesRead);
	}
	else if(!m_finished)
	{
		assert(error == ERROR_IO_PENDING);
	}
}

void PipeServer::WriteMsg(void* a_msg, size_t a_msgSize)
{
	auto op = GetOperation();
	op->opType = OperationType::Write;
	op->opData = a_msg;
	op->opDataSize = a_msgSize;
	DWORD bytesWritten;
	auto result = WriteFile(m_pipeHandle, a_msg, (DWORD)a_msgSize, &bytesWritten, op);
	auto error = GetLastError();
	if(!result && error != ERROR_IO_PENDING)
	{
		OnWrite(op, a_msgSize);
	}
}

std::unique_ptr<IPipeServer> CR::Platform::CreatePipeServer(const char* a_name, IPipeServer::WriteFinishedT a_writeFinished,
															IPipeServer::ReadFinishedT a_readFinished)
{
	auto pipeHandle = CreateNamedPipe(a_name, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE |
					PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1, (DWORD)4_Kb, (DWORD)4_Kb, 5000, nullptr);
	if(pipeHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<PipeServer>(pipeHandle, std::move(a_writeFinished), std::move(a_readFinished));
}
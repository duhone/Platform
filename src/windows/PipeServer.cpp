#include "Platform/PipeServer.h"

#include "IOCP.h"
#include "Platform/Window.h"
#include "core/Log.h"
#include "core/literals.h"

#include <algorithm>
#include <cassert>
#include <concurrent_queue.h>

using namespace CR;
using namespace CR::Core::Literals;
using namespace CR::Platform;

namespace CR::Platform {
	struct PipeServerData {
		enum OperationType { Connection, Read, Write };
		struct Operation : public OVERLAPPED {
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
		PipeServer::WriteFinished_t m_writeFinished;
		PipeServer::ReadFinished_t m_readFinished;
		IOCPPort m_iocpPort;
		concurrency::concurrent_queue<Operation*> m_operationPool;
		size_t m_totalCreatedOps{0};    // for debugging
		using OperationHandler_t                  = void (PipeServerData::*)(Operation*, size_t);
		OperationHandler_t m_operationHandlers[3] = {&PipeServerData::OnConnection, &PipeServerData::OnRead,
		                                             &PipeServerData::OnWrite};
		char m_readBuffer[4_Kb];
		std::atomic_bool m_finished{false};
	};
}    // namespace CR::Platform

PipeServer::PipeServer(const char* a_name, PipeServer::WriteFinished_t a_writeFinished,
                       PipeServer::ReadFinished_t a_readFinished) {
	m_data->m_pipeHandle =
	    CreateNamedPipe(a_name, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
	                    PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS, 1,
	                    (DWORD)4_Kb, (DWORD)4_Kb, 5000, nullptr);
	Core::Log::Assert(m_data->m_pipeHandle != INVALID_HANDLE_VALUE, "failed to create pipe server named {}", a_name);

	m_data->m_writeFinished = std::move(a_writeFinished);
	m_data->m_readFinished  = std::move(a_readFinished);

	m_data->m_iocpPort = IOCPPort{
	    m_data->m_pipeHandle, [this](auto a_overlapped, auto a_size) { m_data->OnOverlapped(a_overlapped, a_size); }};

	auto connectOp    = m_data->GetOperation();
	connectOp->opType = PipeServerData::OperationType::Connection;
	ConnectNamedPipe(m_data->m_pipeHandle, connectOp);
}

PipeServer::~PipeServer() noexcept {
	if(!m_data) { return; }
	m_data->m_finished.store(true, std::memory_order_release);
	CloseHandle(m_data->m_pipeHandle);
	// assert(m_totalCreatedOps == m_operationPool.unsafe_size());
	std::for_each(m_data->m_operationPool.unsafe_begin(), m_data->m_operationPool.unsafe_end(),
	              [](auto op) { delete op; });
}

PipeServer::PipeServer(PipeServer&& a_other) noexcept {
	*this = std::move(a_other);
}

PipeServer& PipeServer::operator=(PipeServer&& a_other) noexcept {
	m_data = std::move(a_other.m_data);
	return *this;
}

void PipeServer::WriteMsg(void* a_msg, size_t a_msgSize) {
	auto op        = m_data->GetOperation();
	op->opType     = PipeServerData::OperationType::Write;
	op->opData     = a_msg;
	op->opDataSize = a_msgSize;
	DWORD bytesWritten;
	auto result = WriteFile(m_data->m_pipeHandle, a_msg, (DWORD)a_msgSize, &bytesWritten, op);
	auto error  = GetLastError();
	if(!result && error != ERROR_IO_PENDING) { m_data->OnWrite(op, a_msgSize); }
}

PipeServerData::Operation* PipeServerData::GetOperation() {
	Operation* result{nullptr};
	while(!m_operationPool.try_pop(result)) {
		for(int i = 0; i < 64; ++i) { m_operationPool.push(new Operation); }
		m_totalCreatedOps += 64;
	}
	Core::Log::Assert(result, "GetOperation failed");
	memset(result, 0, sizeof(Operation));
	return result;
}

void PipeServerData::ReturnOperation(Operation* a_operation) {
	if(a_operation->opType != OperationType::Read) { m_operationPool.push(a_operation); }
}

void PipeServerData::OnOverlapped(OVERLAPPED* a_overlapped, size_t a_size) {
	auto op = (Operation*)a_overlapped;
	Core::Log::Assert(op->opType == OperationType::Read || op->opDataSize == a_size, "invalid operation");
	(this->*m_operationHandlers[op->opType])(op, a_size);
	ReturnOperation(op);
}

void PipeServerData::OnConnection(Operation* a_operation, size_t) {
	Core::Log::Assert(a_operation->opType == OperationType::Connection, "unexpected operation type");
	ReadMsg();
}

void PipeServerData::OnRead(Operation* a_operation, size_t a_size) {
	Core::Log::Assert(a_operation->opType == OperationType::Read, "unexpected operation type");
	m_readFinished(a_operation->opData, a_size);
	ReadMsg();
}

void PipeServerData::OnWrite(Operation* a_operation, size_t) {
	Core::Log::Assert(a_operation->opType == OperationType::Write, "unexpected operation type");
	m_writeFinished(a_operation->opData, a_operation->opDataSize);
}

void PipeServerData::ReadMsg() {
	if(m_finished) { return; }
	auto op        = GetOperation();
	op->opType     = OperationType::Read;
	op->opData     = m_readBuffer;
	op->opDataSize = sizeof(m_readBuffer);
	DWORD bytesRead;
	auto result = ReadFile(m_pipeHandle, m_readBuffer, (DWORD)sizeof(m_readBuffer), &bytesRead, op);
	auto error  = GetLastError();
	if(result || (!result && error != ERROR_IO_PENDING)) {
		OnRead(op, bytesRead);
	} else if(!m_finished) {
		Core::Log::Assert(error == ERROR_IO_PENDING, "read msg had an unexpected error");
	}
}

#include "Platform/PipeClient.h"
#include <Windows.h>
#include <stdexcept>
#include <cassert>
#include <core/literals.h>
#include <thread>

namespace CR::Platform {
  class PipeClient : public IPipeClient {
  public:
    PipeClient(HANDLE a_pipeHandle, IPipeClient::MsgHandlerT&& a_msgHandler);
    virtual ~PipeClient();
    void SendPipeMessage(const void* a_msg, size_t a_msgSize) override;
  private:
    void RunMsgHandler();

    HANDLE m_pipeHandle;
    IPipeClient::MsgHandlerT m_msgHandler;
    std::thread m_msgThread;
    volatile bool m_closed{false};

    OVERLAPPED m_writeOverlapped;
    HANDLE m_writeEvent;
  };
}

using namespace CR::Platform;
using namespace CR::Core::Literals;

PipeClient::PipeClient(HANDLE a_pipeHandle, IPipeClient::MsgHandlerT&& a_msgHandler) : m_pipeHandle(a_pipeHandle),
m_msgHandler(std::move(a_msgHandler)) {
  m_msgThread = std::thread([this]() { this->RunMsgHandler(); });

  memset(&m_writeOverlapped, 0, sizeof(m_writeOverlapped));
  m_writeEvent = CreateEvent(nullptr, TRUE, TRUE, nullptr);
  m_writeOverlapped.hEvent = m_writeEvent;
}

PipeClient::~PipeClient() {
  m_closed = true;
  CloseHandle(m_pipeHandle);
  if (m_msgThread.joinable())
    m_msgThread.join();
  CloseHandle(m_writeEvent);
}

void PipeClient::SendPipeMessage(const void* a_msg, size_t a_msgSize) {
  DWORD bytesWritten = 0;
  assert(a_msgSize < 4_Kb); //arbitrary, but if larger than this might want to switch to an async implementation
  ResetEvent(m_writeEvent);
  BOOL result = WriteFile(m_pipeHandle, a_msg, static_cast<DWORD>(a_msgSize), nullptr, &m_writeOverlapped);
  auto error = GetLastError();
  if (!result && error == ERROR_IO_PENDING) {
    if (GetOverlappedResult(m_pipeHandle, &m_writeOverlapped, &bytesWritten, TRUE) == 0)
      throw std::runtime_error("Failed to write message");
  }
}

void PipeClient::RunMsgHandler() {
  char msg[4_Kb];
  DWORD bytesRead;
  OVERLAPPED overlapped;
  memset(&overlapped, 0, sizeof(overlapped));
  HANDLE completeEvent = CreateEvent(nullptr, TRUE, TRUE, nullptr);
  overlapped.hEvent = completeEvent;
  while (!m_closed) {
    ResetEvent(completeEvent);
    auto result = ReadFile(m_pipeHandle, msg, static_cast<DWORD>(4_Kb), nullptr, &overlapped);
    auto error = GetLastError();
    if (!result && error == ERROR_IO_PENDING) {
      if (GetOverlappedResult(m_pipeHandle, &overlapped, &bytesRead, TRUE) != 0)
        m_msgHandler(msg, bytesRead);
    }
  }
}

std::unique_ptr<IPipeClient> CR::Platform::CreatePipeClient(const char* a_name, IPipeClient::MsgHandlerT a_msgHandler) {
  assert(a_msgHandler); //msg handler is required
  auto pipeHandle = CreateFile(a_name, GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, nullptr);
  if (pipeHandle == INVALID_HANDLE_VALUE)
    return nullptr;
  DWORD pipeState = PIPE_READMODE_MESSAGE;
  SetNamedPipeHandleState(pipeHandle, &pipeState, nullptr, nullptr);

  return std::make_unique<PipeClient>(pipeHandle, std::move(a_msgHandler));
}
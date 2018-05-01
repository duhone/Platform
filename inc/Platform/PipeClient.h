#pragma once
#include <memory>
#include <core\Concepts.h>
#include <type_traits>
#include <functional>

namespace CR::Platform {
  struct IPipeClient {
  protected:
    IPipeClient() = default;
  public:
    virtual ~IPipeClient() = default;
    IPipeClient(const IPipeClient&) = delete;
    IPipeClient& operator=(const IPipeClient&) = delete;

    //For now sends are blocking/synchronous. Design is currently around small msgs. For bulk data
    //use shared memory. recieving msgs is async though.
    virtual void SendPipeMessage(const void* a_msg, size_t a_msgSize) = 0;
    template<SemiRegular MsgT>
    void SendPipeMessage(const MsgT& a_msg) {
      static_assert(std::is_standard_layout<MsgT>::value, "Messages should be pod types");
      SendPipeMessage(&a_msg, sizeof(a_msg));
    }
    using MsgHandlerT = std::function<void(void*, size_t)>;
  };

  std::unique_ptr<IPipeClient> CreatePipeClient(const char* a_name, IPipeClient::MsgHandlerT a_msgHandler);
}
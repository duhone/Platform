#pragma once
#include <3rdParty/function2.h>

#include <functional>
#include <memory>
#include <type_traits>

namespace CR::Platform {
	class PipeClient final {
	  public:
		using MsgHandlerT = fu2::unique_function<void(void*, size_t)>;

		PipeClient() = default;
		PipeClient(const char* a_name, PipeClient::MsgHandlerT a_msgHandler);
		~PipeClient();
		PipeClient(const PipeClient&) = delete;
		PipeClient& operator=(const PipeClient&) = delete;
		PipeClient(PipeClient&&) noexcept;
		PipeClient& operator=(PipeClient&&) noexcept;

		// For now sends are blocking/synchronous. Design is currently around small msgs. For bulk data
		// use shared memory. recieving msgs is async though.
		void SendPipeMessage(const void* a_msg, size_t a_msgSize);
		template<typename MsgT>
		void SendPipeMessage(const MsgT& a_msg) {
			static_assert(std::is_standard_layout<MsgT>::value, "Messages should be pod types");
			SendPipeMessage(&a_msg, sizeof(a_msg));
		}

	  private:
		void RunMsgHandler();

		std::unique_ptr<struct PipeClientData> m_data;
	};
}    // namespace CR::Platform

#pragma once
#include <functional>
#include <memory>

namespace CR::Platform {
	struct IPipeServer {
	  protected:
		IPipeServer() = default;

	  public:
		virtual ~IPipeServer()          = default;
		IPipeServer(const IPipeServer&) = delete;
		IPipeServer& operator=(const IPipeServer&) = delete;

		using WriteFinishedT                                 = std::function<void(void*, size_t)>;
		virtual void WriteMsg(void* a_msg, size_t a_msgSize) = 0;
		template<typename T>
		void WriteMsg(T* a_msg) {
			WriteMsg(a_msg, sizeof(T));
		}
		using ReadFinishedT = std::function<void(void*, size_t)>;
	};

	std::unique_ptr<IPipeServer> CreatePipeServer(const char* a_name, IPipeServer::WriteFinishedT a_writeFinished,
	                                              IPipeServer::ReadFinishedT a_readFinished);
}    // namespace CR::Platform
#pragma once
#include <memory>
#include <functional>

namespace CR
{
	namespace Platform
	{
		struct IPipeServer
		{
			virtual ~IPipeServer() = default;
			using WriteFinishedT = std::function<void(void*, size_t)>;
			virtual void WriteMsg(void* a_msg, size_t a_msgSize) = 0;
			template<typename T>
			void WriteMsg(T* a_msg)
			{
				WriteMsg(a_msg, sizeof(T));
			}
			using ReadFinishedT = std::function<void(void*, size_t)>;
		};

		std::unique_ptr<IPipeServer> CreatePipeServer(const char* a_name, IPipeServer::WriteFinishedT a_writeFinished,
													  IPipeServer::ReadFinishedT a_readFinished);
	}
}
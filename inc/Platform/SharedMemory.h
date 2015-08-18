#pragma once
#include <memory>
#include <cstdint>

namespace CR
{
	namespace Platform
	{
		struct ISharedMemory
		{
			virtual ~ISharedMemory() = default;

			//follow stl naming convention for compatibility with non member data/size
			virtual std::size_t size() = 0;
			virtual uint8_t* data() = 0;
		};

		std::unique_ptr<ISharedMemory> CreateSharedMemory(const char* a_name, size_t a_size);
		std::unique_ptr<ISharedMemory> OpenSharedMemory(const char* a_name, size_t a_size);
	}
}
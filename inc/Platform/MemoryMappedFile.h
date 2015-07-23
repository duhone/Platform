#pragma once
#include <memory>
#include <cstdint>

namespace CR
{
	namespace Platform
	{
		struct IMemoryMappedFile
		{
			virtual ~IMemoryMappedFile() = default;
						
			//follow stl naming convention for compatibility with non member data/size
			virtual std::size_t size() = 0;
			virtual uint8_t* data() = 0;
		};

		std::unique_ptr<IMemoryMappedFile> OpenMMapFile(const char* a_fileName);
	}
}

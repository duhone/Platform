#pragma once
#include "math/Types.h"
#include <memory>

namespace CR::Platform {
	struct IMemoryMappedFile {
	  protected:
		IMemoryMappedFile() = default;

	  public:
		virtual ~IMemoryMappedFile()                = default;
		IMemoryMappedFile(const IMemoryMappedFile&) = delete;
		IMemoryMappedFile& operator=(const IMemoryMappedFile&) = delete;

		// follow stl naming convention for compatibility with non member data/size
		virtual std::size_t size() = 0;
		virtual uchar* data()      = 0;
	};

	std::unique_ptr<IMemoryMappedFile> OpenMMapFile(const char* a_fileName);
}    // namespace CR::Platform

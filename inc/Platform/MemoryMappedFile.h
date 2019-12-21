#pragma once
#include <cstdint>
#include <memory>
#include <filesystem>

namespace CR::Platform {
	struct IMemoryMappedFile {
	  protected:
		IMemoryMappedFile() = default;

	  public:
		virtual ~IMemoryMappedFile()                = default;
		IMemoryMappedFile(const IMemoryMappedFile&) = delete;
		IMemoryMappedFile(IMemoryMappedFile&&) = delete;
		IMemoryMappedFile& operator=(const IMemoryMappedFile&) = delete;
		IMemoryMappedFile& operator=(IMemoryMappedFile&&) = delete;

		// follow stl naming convention for compatibility with non member data/size
		virtual std::size_t size() = 0;
		virtual uint8_t* data()    = 0;
	};

	std::unique_ptr<IMemoryMappedFile> OpenMMapFile(const std::filesystem::path& a_filePath);
}    // namespace CR::Platform

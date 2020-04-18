#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace CR::Platform {
	class MemoryMappedFile final {
	  public:
		MemoryMappedFile() = default;
		MemoryMappedFile(const std::filesystem::path& a_filePath);
		~MemoryMappedFile();
		MemoryMappedFile(const MemoryMappedFile&) = delete;
		MemoryMappedFile(MemoryMappedFile&& a_other) noexcept;
		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator                           =(MemoryMappedFile&& a_other) noexcept;

		// follow stl naming convention for compatibility with non member data/size
		std::size_t size() const noexcept;
		std::byte* data() noexcept;
		const std::byte* data() const noexcept;

	  private:
		std::unique_ptr<struct MemoryMappedFileData> m_fileData;
	};
}    // namespace CR::Platform

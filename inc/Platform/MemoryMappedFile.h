﻿#pragma once
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>

namespace CR::Platform {
	class MemoryMappedFile final {
	  public:
		MemoryMappedFile();
		MemoryMappedFile(const std::filesystem::path& a_filePath);
		~MemoryMappedFile();
		MemoryMappedFile(const MemoryMappedFile&) = delete;
		MemoryMappedFile(MemoryMappedFile&& a_other) noexcept;
		MemoryMappedFile& operator=(const MemoryMappedFile&) = delete;
		MemoryMappedFile& operator                           =(MemoryMappedFile&& a_other) noexcept;

		// follow stl naming convention for compatibility with non member data/size
		[[nodiscard]] std::size_t size() const noexcept;
		[[nodiscard]] std::byte* data() noexcept;
		[[nodiscard]] const std::byte* data() const noexcept;

	  private:
		std::unique_ptr<struct MemoryMappedFileData> m_fileData;
	};
}    // namespace CR::Platform

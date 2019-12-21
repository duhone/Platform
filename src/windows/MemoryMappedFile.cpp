#include "Platform/MemoryMappedFile.h"
#include <windows.h>

using namespace CR::Platform;

namespace CR::Platform {
	class MemoryMappedFile : public IMemoryMappedFile {
	  public:
		MemoryMappedFile(HANDLE a_fileHandle, std::size_t a_fileSize);
		virtual ~MemoryMappedFile();

		uint8_t* data() override { return m_data; }
		std::size_t size() override { return m_fileSize; }

	  private:
		HANDLE m_fileHandle;
		HANDLE m_fileMapping;
		uint8_t* m_data;
		std::size_t m_fileSize;
	};
}    // namespace CR::Platform

MemoryMappedFile::MemoryMappedFile(HANDLE a_fileHandle, std::size_t a_fileSize) :
    m_fileHandle(a_fileHandle), m_fileSize(a_fileSize) {
	m_fileMapping = CreateFileMapping(a_fileHandle, nullptr, PAGE_READONLY, 0, 0, nullptr);
	m_data        = (uint8_t*)MapViewOfFile(m_fileMapping, FILE_MAP_READ, 0, 0, 0);
}

MemoryMappedFile::~MemoryMappedFile() {
	UnmapViewOfFile(m_data);
	CloseHandle(m_fileMapping);
	CloseHandle(m_fileHandle);
}

std::unique_ptr<IMemoryMappedFile> CR::Platform::OpenMMapFile(const std::filesystem::path& a_filePath) {
	auto handle = CreateFileW(a_filePath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if(handle == INVALID_HANDLE_VALUE) return nullptr;
	LARGE_INTEGER fileSize;
	if(!GetFileSizeEx(handle, &fileSize)) return nullptr;
	if(fileSize.QuadPart == 0) return nullptr;
	return std::make_unique<MemoryMappedFile>(handle, static_cast<size_t>(fileSize.QuadPart));
}

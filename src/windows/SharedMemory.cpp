#include "Platform/SharedMemory.h"
#include <Windows.h>

namespace CR
{
	namespace Platform
	{
		class SharedMemory : public ISharedMemory
		{
		public:
			SharedMemory(HANDLE a_handle, size_t a_size);
			virtual ~SharedMemory();

			//follow stl naming convention for compatibility with non member data/size
			std::size_t size() override;
			uint8_t* data() override;
		private:
			HANDLE m_memoryHandle;
			size_t m_size{0};
			uint8_t* m_data{nullptr};
		};
	}
}

using namespace CR::Platform;

SharedMemory::SharedMemory(HANDLE a_handle, size_t a_size) : m_memoryHandle(a_handle), m_size(a_size)
{
	m_data = (uint8_t*)MapViewOfFile(m_memoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, m_size);
}

SharedMemory::~SharedMemory()
{
	UnmapViewOfFile(m_data);
	CloseHandle(m_memoryHandle);
}

std::size_t SharedMemory::size()
{
	return m_size;
}
uint8_t* SharedMemory::data()
{
	return m_data;
}

std::unique_ptr<ISharedMemory> CR::Platform::CreateSharedMemory(const char* a_name, size_t a_size)
{
	auto memoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (DWORD)a_size, a_name);
	if(memoryHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<SharedMemory>(memoryHandle, a_size);
}

std::unique_ptr<ISharedMemory> CR::Platform::OpenSharedMemory(const char* a_name, size_t a_size)
{
	auto memoryHandle = OpenFileMapping(PAGE_READWRITE, FALSE, a_name);
	if(memoryHandle == INVALID_HANDLE_VALUE)
		return nullptr;

	return std::make_unique<SharedMemory>(memoryHandle, a_size);
}
﻿#include "Platform/SharedMemory.h"

#include "core/Log.h"

#include <Windows.h>

namespace CR::Platform {
	struct SharedMemoryData {
		HANDLE m_memoryHandle;
		size_t m_size{0};
		uint8_t* m_data{nullptr};
	};
}    // namespace CR::Platform

using namespace CR;
using namespace CR::Platform;

SharedMemory::SharedMemory(const char* a_name, size_t a_size, CreateNew) : m_data(new SharedMemoryData) {
	m_data->m_size         = a_size;
	m_data->m_memoryHandle = CreateFileMapping(INVALID_HANDLE_VALUE, nullptr, PAGE_READWRITE, 0, (DWORD)a_size, a_name);
	Core::Log::Assert(m_data->m_memoryHandle != INVALID_HANDLE_VALUE,
	                  "Failed to create a shared memory resource, should be impossible");
	m_data->m_data = (uint8_t*)MapViewOfFile(m_data->m_memoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, m_data->m_size);
}

SharedMemory::SharedMemory(const char* a_name, size_t a_size, OpenExisting) : m_data(new SharedMemoryData) {
	m_data->m_size         = a_size;
	m_data->m_memoryHandle = OpenFileMapping(PAGE_READWRITE, FALSE, a_name);
	Core::Log::Require(m_data->m_memoryHandle != INVALID_HANDLE_VALUE,
	                   "Failed to open a shared memory resource, probably was never created");
	m_data->m_data = (uint8_t*)MapViewOfFile(m_data->m_memoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, m_data->m_size);
}

SharedMemory::SharedMemory(SharedMemory&& a_other) noexcept {
	*this = std::move(a_other);
}

SharedMemory& SharedMemory::operator=(SharedMemory&& a_other) noexcept {
	m_data = std::move(a_other.m_data);
	return *this;
}

SharedMemory::~SharedMemory() {
	if(!m_data) { return; }
	UnmapViewOfFile(m_data->m_data);
	CloseHandle(m_data->m_memoryHandle);
}

std::size_t SharedMemory::size() const {
	return m_data->m_size;
}

const uint8_t* SharedMemory::data() const {
	return m_data->m_data;
}

uint8_t* SharedMemory::data() {
	return m_data->m_data;
}

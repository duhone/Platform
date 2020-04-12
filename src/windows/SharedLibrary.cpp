#include "Platform/SharedLibrary.h"

#include "core/Log.h"

#include <windows.h>

namespace CR::Platform {
	struct SharedLibraryData {
		HMODULE m_libraryModule;
	};
}    // namespace CR::Platform

using namespace CR;
using namespace CR::Platform;

/*
#include <strsafe.h>
void ErrorExit(LPTSTR lpszFunction)
{
  // Retrieve the system error message for the last-error code

  LPVOID lpMsgBuf;
  LPVOID lpDisplayBuf;
  DWORD dw = GetLastError();

  FormatMessage(
    FORMAT_MESSAGE_ALLOCATE_BUFFER |
    FORMAT_MESSAGE_FROM_SYSTEM |
    FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL,
    dw,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (LPTSTR)&lpMsgBuf,
    0, NULL);

  // Display the error message and exit the process

  lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
                    (lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
  StringCchPrintf((LPTSTR)lpDisplayBuf,
          LocalSize(lpDisplayBuf) / sizeof(TCHAR),
          TEXT("%s failed with error %d: %s"),
          lpszFunction, dw, lpMsgBuf);
  MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

  LocalFree(lpMsgBuf);
  LocalFree(lpDisplayBuf);
  //ExitProcess(dw);
}
*/

SharedLibrary::SharedLibrary(const char* a_libraryName) : m_data(new SharedLibraryData) {
	m_data->m_libraryModule = LoadLibrary(a_libraryName);
	if(!m_data->m_libraryModule) {
		// ErrorExit("Failed to load library");
		Core::Log::Error("Failed to load library");
	}
}

SharedLibrary::~SharedLibrary() {
	if(!m_data) { return; }
	FreeLibrary(m_data->m_libraryModule);
}

void* SharedLibrary::GetFunction(const char* a_functionName) const {
	return GetProcAddress(m_data->m_libraryModule, a_functionName);
}

SharedLibrary::SharedLibrary(SharedLibrary&& a_other) noexcept {
	*this = std::move(a_other);
}

SharedLibrary& SharedLibrary::operator=(SharedLibrary&& a_other) noexcept {
	m_data = std::move(a_other.m_data);
	return *this;
}

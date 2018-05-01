#include "Platform/SharedLibrary.h"
#include <windows.h>

namespace CR::Platform {
  class SharedLibrary : public ISharedLibrary {
  public:
    SharedLibrary(HMODULE a_libraryModule);
    virtual ~SharedLibrary();
    void* GetFunction(const char* a_functionName) override;
  private:
    HMODULE m_libraryModule;
  };
}

using namespace CR::Platform;

SharedLibrary::SharedLibrary(HMODULE a_libraryModule) : m_libraryModule(a_libraryModule) {

}

SharedLibrary::~SharedLibrary() {
  FreeLibrary(m_libraryModule);
}

void* SharedLibrary::GetFunction(const char* a_functionName) {
  return GetProcAddress(m_libraryModule, a_functionName);
}
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
std::unique_ptr<ISharedLibrary> CR::Platform::LoadSharedLibrary(const char* a_libraryName) {
  auto library = LoadLibrary(a_libraryName);
  if (!library) {
    //ErrorExit("Failed to load library");
    return nullptr;
  }

  return std::make_unique<SharedLibrary>(library);
}
#include "Platform/PathUtils.h"
#include <Windows.h>

using namespace CR::Platform;
using namespace std::filesystem;

path CR::Platform::GetCurrentProcessPath() {
	wchar_t filenameWithPath[4096];    // 4096 should be enough for anyone
	GetModuleFileNameW(nullptr, filenameWithPath, sizeof(filenameWithPath));
	path pathOnly{filenameWithPath};
	pathOnly = pathOnly.parent_path();
	return pathOnly;
}

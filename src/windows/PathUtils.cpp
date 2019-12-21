#include "Platform/PathUtils.h"
#include <Windows.h>

using namespace CR::Platform;
using namespace std::filesystem;

path CR::Platform::GetCurrentProcessPath() {
	char filenameWithPath[_MAX_PATH];
	GetModuleFileName(nullptr, filenameWithPath, sizeof(filenameWithPath));
	path pathOnly{filenameWithPath};
	pathOnly = pathOnly.parent_path();
	return pathOnly;
}
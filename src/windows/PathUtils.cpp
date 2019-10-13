#include "Platform/PathUtils.h"
#include <Windows.h>
#include <filesystem>

using namespace CR::Platform;
using namespace std::filesystem;

std::string CR::Platform::GetCurrentProcessPath() {
	char filenameWithPath[_MAX_PATH];
	GetModuleFileName(nullptr, filenameWithPath, sizeof(filenameWithPath));
	path pathOnly{filenameWithPath};
	pathOnly = pathOnly.parent_path();
	return pathOnly.generic_string();
}
std::string CR::Platform::RelativeToAbsolute(const char* a_relativePath) {
	path result{GetCurrentProcessPath()};
	result.append(a_relativePath);
	return result.generic_string();
}
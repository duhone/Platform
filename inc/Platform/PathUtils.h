#pragma once
#include <string>

namespace CR
{
	namespace Platform
	{
		std::string GetCurrentProcessPath();
		std::string RelativeToAbsolute(const char* a_relativePath); //a_relativePath considered relative to process path
	}
}
#include "catch.hpp"

#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

TEST_CASE("memory mapped files", "") {
	auto testFilePath = CR::Platform::GetCurrentProcessPath();
	testFilePath.append("test.txt");
	auto mmapFile = CR::Platform::OpenMMapFile(testFilePath.c_str());
	REQUIRE(mmapFile);
	REQUIRE(mmapFile->size() == 2);
	auto data = mmapFile->data();
	REQUIRE((char)data[0] == '5');
	REQUIRE((char)data[1] == '8');
}
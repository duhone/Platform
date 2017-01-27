#include "catch.hpp"
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

TEST_CASE("memory mapped files", "")
{
	auto testFilePath = CR::Platform::RelativeToAbsolute("test.txt");
	auto mmapFile = CR::Platform::OpenMMapFile(testFilePath.c_str());
	REQUIRE(mmapFile);
	REQUIRE(mmapFile->size() == 2);
	auto data = mmapFile->data();
	REQUIRE(data[0] == '5');
	REQUIRE(data[1] == '8');
}
#include "Platform/MemoryMappedFile.h"
#include "Platform/PathUtils.h"

#include <3rdParty/doctest.h>

TEST_CASE("memory mapped files") {
	auto testFilePath = CR::Platform::GetCurrentProcessPath();
	testFilePath.append("test.txt");
	CR::Platform::MemoryMappedFile mmapFile{testFilePath.c_str()};
	REQUIRE(mmapFile.size() == 2);
	auto data = mmapFile.data();
	REQUIRE((char)data[0] == '5');
	REQUIRE((char)data[1] == '8');

	CR::Platform::MemoryMappedFile mmapFile2 = std::move(mmapFile);
	REQUIRE(mmapFile.size() == 0);
	REQUIRE(mmapFile2.size() == 2);
}

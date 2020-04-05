﻿#include "Platform/SharedLibrary.h"
#include <functional>

#include <3rdParty/doctest.h>

// test.dll is a dll that exports 2 functions, GetTestValue and SetTestValue. These functions get and set
// a global variable in the dll. Code for this dll not included but it is trivial to implement.

using namespace CR::Platform;

// By having 2 test cases we are also testing unloading the dll, then reloading it and its global variable going back to
// 0
TEST_CASE("shared library void* style") {
	auto library = LoadSharedLibrary("testdll");
	REQUIRE(library);
	auto GetTestValue = (int (*)())library->GetFunction("GetTestValue");
	REQUIRE(GetTestValue);
	auto SetTestValue = (void (*)(int))library->GetFunction("SetTestValue");
	REQUIRE(SetTestValue);
	REQUIRE(GetTestValue() == 0);
	SetTestValue(5);
	REQUIRE(GetTestValue() == 5);
}

TEST_CASE("shared library std::function style") {
	auto library = LoadSharedLibrary("testdll");
	REQUIRE(library);
	auto GetTestValue = library->GetStdFunction<int()>("GetTestValue");
	REQUIRE(GetTestValue);
	auto SetTestValue = library->GetStdFunction<void(int)>("SetTestValue");
	REQUIRE(SetTestValue);
	REQUIRE(GetTestValue() == 0);
	SetTestValue(5);
	REQUIRE(GetTestValue() == 5);
}

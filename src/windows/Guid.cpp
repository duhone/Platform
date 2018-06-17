#include "Platform/Guid.h"
#include <Platform/Window.h>
#include <objbase.h>

CR::Core::Guid CR::Platform::CreateGuid() {
	GUID guid;
	if(CoCreateGuid(&guid) != S_OK) return CR::Core::Guid::Null();
	return CR::Core::Guid{(uint)guid.Data1, *(uint*)(&guid.Data2), *(uint*)guid.Data4, *((uint*)&guid.Data4[4])};
}
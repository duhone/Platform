#include "Platform/Guid.h"
#include <objbase.h>

CR::Core::Guid CR::Platform::CreateGuid() {
	GUID guid;
	if(CoCreateGuid(&guid) != S_OK) return CR::Core::Guid::Null();
	return CR::Core::Guid{(uint32_t)guid.Data1, *(uint32_t*)(&guid.Data2), *(uint32_t*)guid.Data4,
	                      *((uint32_t*)&guid.Data4[4])};
}

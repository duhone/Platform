#include "Guid.h"
#include <Window.h>
#include <objbase.h>

CR::Core::Guid CR::Platform::CreateGuid()
{
	GUID guid;
	if(CoCreateGuid(&guid) != S_OK)
		return CR::Core::Guid::Null();
	return CR::Core::Guid{(unsigned int)guid.Data1, *(unsigned int*)(&guid.Data2), *(unsigned int*)guid.Data4, 
		*((unsigned int*)&guid.Data4[4])};
}
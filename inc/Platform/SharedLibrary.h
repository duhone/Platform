#pragma once
#include <memory>
#include <functional>
#include <core\Concepts.h>
#include <core\TypeTraits.h>

namespace CR
{
	namespace Platform
	{
		struct ISharedLibrary
		{
			virtual ~ISharedLibrary() = default;
			virtual void* GetFunction(const char* a_functionName) = 0;
			
			template<Callable CallableT>
			auto GetStdFunction(const char* a_functionName)
			{
				return std::function<CallableT>{static_cast<Core::GetFunctionPtrType_t<CallableT>>(GetFunction(a_functionName))};
			}
		};

		std::unique_ptr<ISharedLibrary> LoadSharedLibrary(const char* a_libraryName);
	}
}
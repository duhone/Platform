#pragma once
#include <core\Concepts.h>
#include <core\TypeTraits.h>
#include <functional>
#include <memory>

namespace CR::Platform {
	struct ISharedLibrary {
	  protected:
		ISharedLibrary() = default;

	  public:
		virtual ~ISharedLibrary()             = default;
		ISharedLibrary(const ISharedLibrary&) = delete;
		ISharedLibrary& operator=(const ISharedLibrary&) = delete;

		virtual void* GetFunction(const char* a_functionName) = 0;

		template<Callable CallableT>
		auto GetStdFunction(const char* a_functionName) {
			return std::function<CallableT>{
			    static_cast<Core::GetFunctionPtrType_t<CallableT>>(GetFunction(a_functionName))};
		}
	};

	std::unique_ptr<ISharedLibrary> LoadSharedLibrary(const char* a_libraryName);
}    // namespace CR::Platform
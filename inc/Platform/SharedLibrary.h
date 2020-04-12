#pragma once
#include <core\TypeTraits.h>

#include <3rdParty/function2.h>

#include <functional>
#include <memory>

namespace CR::Platform {
	struct SharedLibrary {
	  public:
		SharedLibrary() = default;
		SharedLibrary(const char* a_libraryName);
		~SharedLibrary() noexcept;
		SharedLibrary(const SharedLibrary&) = delete;
		SharedLibrary& operator=(const SharedLibrary&) = delete;
		SharedLibrary(SharedLibrary&& a_other) noexcept;
		SharedLibrary& operator=(SharedLibrary&& a_other) noexcept;

		void* GetFunction(const char* a_functionName) const;

		template<typename CallableT>
		auto GetUniqueFunction(const char* a_functionName) const {
			return fu2::unique_function<CallableT>{
			    static_cast<Core::GetFunctionPtrType_t<CallableT>>(GetFunction(a_functionName))};
		}

	  private:
		std::unique_ptr<struct SharedLibraryData> m_data;
	};
}    // namespace CR::Platform

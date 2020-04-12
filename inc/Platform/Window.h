#pragma once

#include <3rdParty/function2.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace CR::Platform {
	// For now their is an assumption that an application only ever creates one of these.
	// Some work needs to be done if that assumption isn't valid.
	class Window final {
	  public:
		using OnDestroy_t = fu2::unique_function<void()>;

		Window() = default;
		Window(const char* a_windowTitle, uint32_t a_width, uint32_t a_height, OnDestroy_t a_onDestroy);
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
		Window(Window&& a_other) noexcept;
		Window& operator=(Window&& a_other) noexcept;

		void Destroy();    // Will happen automatically on window destruction as well

		void OnDestroy();

	  private:
		std::unique_ptr<struct WindowData> m_data;
	};
}    // namespace CR::Platform

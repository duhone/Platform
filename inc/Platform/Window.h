#pragma once
#include "math/Types.h"
#include <functional>
#include <memory>

namespace CR::Platform {
	struct IWindow {
	  protected:
		IWindow() = default;

	  public:
		virtual ~IWindow()      = default;
		IWindow(const IWindow&) = delete;
		IWindow& operator=(const IWindow&) = delete;

		virtual void Destroy() = 0;    // Will happen automatically on window destruction as well
		using OnDestroyT       = std::function<void()>;
	};

	// For now their is an assumption that an application only ever creates one of these.
	// Some work needs to be done if that assumption isn't valid.
	std::unique_ptr<IWindow> CRCreateWindow(const char* a_windowTitle, uint a_width, uint a_height,
	                                        IWindow::OnDestroyT a_onDestroy);
}    // namespace CR::Platform
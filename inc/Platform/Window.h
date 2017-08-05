#pragma once
#include <memory>
#include <functional>
#include "math/Types.h"

namespace CR
{
	namespace Platform
	{
		struct IWindow
		{
			virtual ~IWindow() = default;
			virtual void Destroy() = 0; //Will happen automaticaly on window destruction as well
			using OnDestroyT = std::function<void()>;
		};

		//For now their is an assumption that an application only ever creates one of these.
		//Some work needs to be done if that assumption isn't valid.
		std::unique_ptr<IWindow> CRCreateWindow(const char* a_windowTitle, uint a_width, uint a_height,
												IWindow::OnDestroyT a_onDestroy);
	}
}
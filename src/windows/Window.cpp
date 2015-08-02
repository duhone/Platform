#include "Window.h"
#include <Windows.h>
#include <thread>
#include <string>

namespace CR
{
	namespace Platform
	{
		class Window : public IWindow
		{
		public:
			Window(const char* a_windowTitle, uint32_t a_width, uint32_t a_height);
			virtual ~Window(); 
			void Destroy() override;
		private:
			void MyCreateWindow(const char* a_windowTitle, uint32_t a_width, uint32_t a_height);
			void RunMsgLoop();

			HWND m_HWND{0};
			std::thread m_thread;
		};
	}
}

using namespace CR::Platform;

namespace
{
	LRESULT CALLBACK WinProc(HWND a_hWnd, UINT a_message, WPARAM a_wParam, LPARAM a_lParam)
	{
		switch(a_message)
		{
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		}

		// Handle any messages the switch statement didn't.
		return DefWindowProc(a_hWnd, a_message, a_wParam, a_lParam);
	}
}

Window::Window(const char* a_windowTitle, uint32_t a_width, uint32_t a_height)
{

	m_thread = std::thread([this, windowTitle = std::string{a_windowTitle}, a_width, a_height]() {
		this->MyCreateWindow(windowTitle.c_str(), a_width, a_height);
		this->RunMsgLoop(); 
	});
}

Window::~Window()
{
	Destroy();
	if(m_thread.joinable())
		m_thread.join();
}

void Window::MyCreateWindow(const char* a_windowTitle, uint32_t a_width, uint32_t a_height)
{
	// Initialize the window class.
	WNDCLASSEX windowClass = {0};
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = WinProc;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.lpszClassName = "WindowClass1";
	RegisterClassEx(&windowClass);

	RECT windowRect = {0, 0, static_cast<LONG>(a_width), static_cast<LONG>(a_height)};
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window and store a handle to it.
	m_HWND = CreateWindowEx(NULL,
							"WindowClass1",
							a_windowTitle,
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							windowRect.right - windowRect.left,
							windowRect.bottom - windowRect.top,
							NULL,
							NULL,
							GetModuleHandle(NULL),
							NULL);

	ShowWindow(m_HWND, TRUE);
}

void Window::RunMsgLoop()
{
	MSG msg = {0};
	BOOL GetMsgResult = TRUE;
	while((GetMsgResult = GetMessage(&msg, NULL, 0, 0)) != 0)
	{
		if(GetMsgResult < 0)
			PostQuitMessage(GetMsgResult);

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void Window::Destroy()
{
	PostMessage(m_HWND, WM_DESTROY, 0, 0);
}

std::unique_ptr<IWindow> CR::Platform::CRCreateWindow(const char* a_windowTitle, uint32_t a_width, uint32_t a_height)
{
	return std::make_unique<Window>(a_windowTitle, a_width, a_height);
}
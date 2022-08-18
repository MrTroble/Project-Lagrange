#include "../../public/graphics/WindowModule.hpp"
#include <iostream>
#include <string>
#include "../../public/Util.hpp"

#ifdef WIN32
#include <Windows.h>
#endif
#ifdef __linux__
#include <X11/X.h>
#include <X11/Xlib.h>
#endif

namespace tge::graphics
{

#ifdef WIN32
	LRESULT CALLBACK callback(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (Msg == WM_CLOSE)
		{
			util::requestExit();
			return 0;
		}
		return DefWindowProc(hWnd, Msg, wParam, lParam);
	}

	main::Error init(WindowModule *winModule)
	{
		HMODULE systemHandle = GetModuleHandle(nullptr);
		if (!systemHandle)
			return main::Error::NO_MODULE_HANDLE;
		winModule->hInstance = systemHandle;

		const auto windowProperties = winModule->getWindowProperties();

		WNDCLASSEX wndclass;
		FillMemory(&wndclass, sizeof(WNDCLASSEX), 0);
		wndclass.cbSize = sizeof(WNDCLASSEX);
		wndclass.style = CS_ENABLE | CS_OWNDC | CS_HREDRAW;
		wndclass.lpfnWndProc = callback;
		wndclass.hInstance = systemHandle;
		wndclass.lpszClassName = ENGINE_NAME;

		auto regWndClass = RegisterClassEx(&wndclass);
		if (!regWndClass)
		{
			if (GetLastError() != ERROR_CLASS_ALREADY_EXISTS)
				return main::Error::COULD_NOT_CREATE_WINDOW_CLASS;
		}

		auto window = CreateWindowEx(
			WS_EX_APPWINDOW, ENGINE_NAME, APPLICATION_NAME,
			WS_CLIPSIBLINGS | WS_CAPTION | WS_SIZEBOX | WS_SYSMENU,
			windowProperties.x, windowProperties.y, windowProperties.width,
			windowProperties.height, NULL, NULL, systemHandle, NULL);
		if (!window)
			return main::Error::COULD_NOT_CREATE_WINDOW;
		winModule->hWnd = window;
		ShowWindow(window, SW_SHOW);
		UpdateWindow(window);
		return main::Error::NONE;
	}

	void pool(WindowModule *winModule)
	{
		MSG msg;
		const HWND wnd = (HWND)winModule->hWnd;
		while (PeekMessage(&msg, wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_SIZING)
			{
				winModule->resizeMutex.lock();
			}
			else if (msg.message == WM_SIZE)
			{
				winModule->resizeMutex.unlock();
			}
			for (const auto fun : winModule->customFn)
				((WNDPROC)fun)(wnd, msg.message, msg.wParam, msg.lParam);
		}
	}

	void destroy(WindowModule *winModule)
	{
		DestroyWindow((HWND)winModule->hWnd);
	}

#endif // WIN32

#ifdef __linux__
	typedef int (*WNDPROC)(XEvent &ev);

	main::Error init(WindowModule *winModule)
	{
		winModule->hInstance = XOpenDisplay(NULL);
		if (!winModule->hInstance)
			return main::Error::NO_MODULE_HANDLE;
		Display *display = (Display *)winModule->hInstance;
		const auto root = DefaultRootWindow(display);
		if (!root)
			return main::Error::NO_MODULE_HANDLE;

		const auto windowProperties = winModule->getWindowProperties();

		winModule->hWnd = (void *)XCreateSimpleWindow(display, root, windowProperties.x, windowProperties.y, windowProperties.width,
													  windowProperties.height, 1, 1, 1);
		if (!winModule->hWnd)
			return main::Error::COULD_NOT_CREATE_WINDOW;

		XMapWindow(display, (Window)winModule->hWnd);
		XStoreName(display, (Window)winModule->hWnd, APPLICATION_NAME);
		XSelectInput(display, (Window)winModule->hWnd, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | FocusChangeMask);
		return tge::main::Error::NONE;
	}

	void pool(WindowModule *winModule)
	{
		const auto display = (Display *)winModule->hInstance;
		while (XPending(display))
		{
			XEvent xev;
			XNextEvent(display, &xev);
			for (const auto fun : winModule->customFn)
				((WNDPROC)fun)(xev);
		}
	}

	void destroy(WindowModule *winModule)
	{
		XDestroyWindow((Display *)winModule->hInstance, (Window)winModule->hWnd);
		XCloseDisplay((Display *)winModule->hInstance);
	}
#endif

	main::Error WindowModule::init()
	{
		return tge::graphics::init(this);
	}

	void WindowModule::tick(double deltatime)
	{
		tge::graphics::pool(this);
	}

	void WindowModule::destroy()
	{
		this->closing = true;
		tge::graphics::destroy(this);
	}

	WindowProperties WindowModule::getWindowProperties()
	{
		return WindowProperties();
	}

} // namespace tge::graphics

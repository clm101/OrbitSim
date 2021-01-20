#ifndef WINSTUFF_H
#define WINSTUFF_H

#include "WindowsInclude.h"
#include <string>
#include <memory>
#include <optional>
#include "../ExceptionBase.h"
#include "WMMap.h"

class Mouse;

class Window {
private:
	friend class Application;
	HINSTANCE hInst;
	HWND hwnd;
	const wchar_t* pWindowName;
	POINTS ptsWidth;

	static LRESULT CALLBACK MsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MsgRoutineHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK MsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
public:
	class WinExceptionBase : public ExceptionBase {
		using ExceptionBase::ExceptionBase;
	public:
		static std::string getWinErrorString(HRESULT hr) noexcept;
	};

	class HRException : public WinExceptionBase {
	private:
		HRESULT hrError;
		std::string strErrorMsg;
	public:
		HRException(int, const char*, HRESULT) noexcept;
		const char* getType() const noexcept override;
		const char* what() const noexcept override;
	};
private:
	// Used to enable the class to be unregistered if Window() throws
	class WindowClass {
		const wchar_t* pClassName;
		HINSTANCE hInst;
	public:
		WindowClass(HINSTANCE, const wchar_t*);
		~WindowClass();
		const wchar_t* getClassName() const noexcept;
	};
	std::unique_ptr<WindowClass> ptrWndClass;
public:
	Window(const wchar_t*, short, short);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	std::unique_ptr<Mouse> ptrMouse;

	static std::optional<int> ProcessMessages();

	const HINSTANCE& inst() const noexcept;
};

#endif
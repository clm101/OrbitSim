#include <windowsx.h>
#include "clmWinStuff.h"
#include "../DebugDefines/WinDebugDefines.h"
#include "Mouse.h"

/**********************************/
/***	EXCEPTION HANDLING		***/
/**********************************/

std::string Window::WinExceptionBase::getWinErrorString(HRESULT hr) noexcept {
	char* pMsg = nullptr;

	if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		, nullptr, hr, LocaleNameToLCID(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_ALLOW_NEUTRAL_NAMES),
		reinterpret_cast<LPSTR>(&pMsg), 0, nullptr)) {
		return "Unidentified error code";
	}

	std::string sReturn = pMsg;
	LocalFree(pMsg);
	return sReturn;
}

Window::HRException::HRException(int nLine, const char* pFile, HRESULT hrError) noexcept
	:
	WinExceptionBase(nLine, pFile), hrError(hrError) 
{
	strErrorMsg = WinExceptionBase::getWinErrorString(hrError);
	strWhatBuffer = "";
}

const char* Window::HRException::getType() const noexcept {
	return "Window exception";
}

const char* Window::HRException::what() const noexcept {
	std::ostringstream ossOut;
	ossOut << "File: " << getFile() << std::endl
		<< "Line: " << getLine() << std::endl
		<< "Type: " << getType() << std::endl
		<< "Desc: " << strErrorMsg << std::endl;
	strWhatBuffer = ossOut.str();
	return strWhatBuffer.c_str();
}

/**************************/
/***	WINDOWCLASS		***/
/**************************/

// Message handler declared below
Window::WindowClass::WindowClass(HINSTANCE hInst_in, const wchar_t* pClassName_in) 
	:
	hInst(hInst_in), pClassName(pClassName_in)
{
#if CONFIRM_CONSTRUCTOR
	OutputDebugStringW(L"WindowClass() called\n");
#endif

	WNDCLASSEXW wcex{};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MsgSetup;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInst;
	wcex.hIcon = nullptr;
	wcex.hCursor = nullptr;
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = pClassName;
	wcex.hIconSm = nullptr;

	CLM_EXCEPT_LAST(RegisterClassEx(&wcex));
}
Window::WindowClass::~WindowClass() {
#if CONFIRM_DESTRUCTOR
	OutputDebugStringW(L"~WindowClass() called\n");
#endif
	CLM_EXCEPT_DEBUG(UnregisterClass(pClassName, hInst), L"UnregisterClass failed\n");
}

const wchar_t* Window::WindowClass::getClassName() const noexcept {
	return pClassName;
}

/**********************/
/***	WINDOW		***/
/**********************/

// Constructor
Window::Window(const wchar_t* pWindowName, short sWidth, short sHeight)
	:
	hInst(GetModuleHandle(nullptr)), pWindowName(pWindowName),
	ptsWidth({ sWidth, sHeight })
{
	CONSTRUCTOR_CONFIRM(Window);
	CLM_EXCEPT_HR(CoInitialize(NULL));
	ptrWndClass = std::make_unique<WindowClass>(hInst, pWindowName);

	RECT rWindowDim{ 0, 0, sWidth, sHeight };
	AdjustWindowRect(&rWindowDim, WS_CAPTION | WS_SIZEBOX, FALSE);

	hwnd = CreateWindowExW(WS_EX_OVERLAPPEDWINDOW, ptrWndClass->getClassName(), pWindowName, WS_OVERLAPPED | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX,
		100 + rWindowDim.left, 100 + rWindowDim.top, rWindowDim.right - rWindowDim.left, rWindowDim.bottom - rWindowDim.top,
		nullptr, nullptr, hInst, this);
	if (hwnd == NULL) {
		CLM_EXCEPT_LAST_NARG();
	}
	ShowWindow(hwnd, SW_SHOW);

	ptrMouse = std::make_unique<Mouse>(*this);
	WINDOWINFO wiInfo{};
	wiInfo.cbSize = sizeof(WINDOWINFO);
	GetWindowInfo(hwnd, &wiInfo);
	//ptrUI = std::make_unique<UIManager>(UI::Rect{ 0, 0, (std::int32_t)(wiInfo.rcClient.right - wiInfo.rcClient.left), (std::int32_t)(wiInfo.rcClient.bottom - wiInfo.rcClient.top) });
}

// Destructor
Window::~Window() {
	DESTRUCTOR_CONFIRM(~Window);
	if (hwnd != NULL) {
		DestroyWindow(hwnd);
	}
	CoUninitialize();
}

// Initial callback function
LRESULT CALLBACK Window::MsgSetup(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_NCCREATE) {
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		Window* const ptrWnd = static_cast<Window*>(pCreate->lpCreateParams);
		SetWindowLongPtrW(hwnd, GWLP_USERDATA, (LONG_PTR)ptrWnd);
		SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)Window::MsgRoutineHandler);
		return ptrWnd->MsgHandler(hwnd, msg, wParam, lParam);
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
	
	//return DefWindowProc(hwnd, msg, wParam, lParam);
}

// Callback handler
LRESULT CALLBACK Window::MsgRoutineHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	Window* const ptrWnd = reinterpret_cast<Window*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
	return ptrWnd->MsgHandler(hwnd, msg, wParam, lParam);
}

// Instance-specific callback
LRESULT CALLBACK Window::MsgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		break;
	}
	case WM_LBUTTONDOWN:
	{
		POINTS coord = MAKEPOINTS(lParam);
		std::ostringstream ossTitle;
		ossTitle << "Mouse click: (" << coord.x
				<< ", " << coord.y << ")";
		CLM_EXCEPT_LAST(SetWindowTextA(hwnd, ossTitle.str().c_str()));
		ptrMouse->on_left_click({ (std::int32_t)GET_X_LPARAM(lParam), (std::int32_t)GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_LBUTTONUP:
	{
		ptrMouse->on_left_release({ (std::int32_t)GET_X_LPARAM(lParam), (std::int32_t)GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_RBUTTONDOWN:
	{
		ptrMouse->on_right_click({ (std::int32_t)GET_X_LPARAM(lParam), (std::int32_t)GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_RBUTTONUP:
	{
		ptrMouse->on_right_release({ (std::int32_t)GET_X_LPARAM(lParam), (std::int32_t)GET_Y_LPARAM(lParam) });
		break;
	}
	case WM_MOUSEMOVE:
	{
		ptrMouse->on_move({ (std::int32_t)GET_X_LPARAM(lParam), (std::int32_t)GET_Y_LPARAM(lParam) });
		break;
	}
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

std::optional<int> Window::ProcessMessages() {
	MSG msg{};
	static WindowsMessages wm;
	while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
		MESSAGE_PRINT(wm(msg.message, msg.wParam, msg.lParam).c_str());
		if (msg.message == WM_QUIT) {
			return (int)msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return {};
}

const HINSTANCE& Window::inst() const noexcept {
	return hInst;
}
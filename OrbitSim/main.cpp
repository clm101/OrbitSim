#include "Windows/WindowsInclude.h"
#include <d3d11.h>
#include "Windows/clmWinStuff.h"
#include "Application.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
	int nRet{ 0 };
	try {
		Application app(L"Test window", 1280, 720);
		nRet = app.Start();
	}
	catch (ExceptionBase& e) {
		MessageBoxA(nullptr, e.what(), e.getType(), MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
	}
	catch (std::exception& e) {
		MessageBoxA(nullptr, e.what(), "Standard exception", MB_ICONEXCLAMATION);
	}
	catch (...) {
		std::ostringstream ossCatchAll;
		ossCatchAll << "File: " << __FILE__ << std::endl
			<< "Line: " << __LINE__ << std::endl
			<< "Desc: N/A" << std::endl;
		MessageBoxA(nullptr, ossCatchAll.str().c_str(), "Unprocessed exception", MB_ICONEXCLAMATION);
	}

	//_CrtDumpMemoryLeaks();
	return nRet;
}
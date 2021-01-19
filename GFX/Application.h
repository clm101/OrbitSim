#ifndef APPLICATION_H
#define APPLICATION_H

#include "Windows/clmWinStuff.h"
#include "clmTimer.h"
#include <memory>
#include "DebugDefines/MemoryDebug.h"

class Application {
private:
	std::unique_ptr<Window> ptrWnd;
	clmTimer tmrTimer;
public:
	Application(const wchar_t*, short, short);
	Application(const Application&) = delete;
	Application& operator=(const Application) = delete;

	int Start();
	void process_input();
	void DoFrame(const float) const;
};

#endif
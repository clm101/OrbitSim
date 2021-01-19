#ifndef APPLICATION_H
#define APPLICATION_H

#include "Windows/clmWinStuff.h"
#include "clmTimer.h"
#include <memory>
#include "DebugDefines/MemoryDebug.h"
#include "Graphics/clmGraphics.h"

class Application {
private:
	std::unique_ptr<Window> ptrWnd;
	std::unique_ptr<Graphics> ptrGfx;
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
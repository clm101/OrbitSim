#ifndef APPLICATION_H
#define APPLICATION_H

#include "Windows/WinStuff.h"
#include "Timer.h"
#include <memory>
#include "DebugDefines/MemoryDebug.h"
#include "Graphics/Graphics.h"

namespace clm {
	class Application {
	protected:
		std::unique_ptr<Window> ptrWnd;
		std::unique_ptr<Graphics> ptrGfx;
		Timer tmrTimer;
	public:
		Application(const wchar_t*, short, short);
		Application(const Application&) = delete;
		Application& operator=(const Application) = delete;

		int Start();
		void process_input();
		void DoFrame(const float);

		virtual void run(const float);
	};
}

#endif
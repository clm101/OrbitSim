#include "Application.h"
#include "Windows/WinStuff.h"
#include "Windows/Mouse.h"

namespace clm {
	Application::Application(const wchar_t* wWindowName, short sWidth, short sHeight) {
		ptrWnd = std::make_unique<Window>(wWindowName, sWidth, sHeight);
		ptrGfx = std::make_unique<Graphics>(ptrWnd->hwnd);
		tmrTimer.set();
	}

	int Application::Start() {
		while (true) {
			if (const auto ret = Window::ProcessMessages()) {
				return *ret;
			}
			process_input();
			DoFrame((float)tmrTimer.peek());
		}
	}

	void Application::process_input() {
		while (const auto e = ptrWnd->ptrMouse->get_next_event()) {
			if (e->get_type() == Mouse::Event::Type::Move) {
				Mouse::Pos posCursor = e->get_cursor_pos();
				ptrWnd->ptrMouse->update_cursor_pos(posCursor);
			}
			else if (e->get_type() == Mouse::Event::Type::LClick) {
				int breakpoint = 3;
			}
		}
	}

	void Application::DoFrame(const float fTime) {
		ptrGfx->BeginFrame();

		run(fTime);

		ptrGfx->EndFrame();
	}

	void Application::run(const float fTime) {

	}
}
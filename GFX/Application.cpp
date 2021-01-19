#include "Application.h"
#include "Windows/clmWinStuff.h"
#include "Windows/Mouse.h"

Application::Application(const wchar_t* wWindowName, short sWidth, short sHeight) {
	ptrWnd = std::make_unique<Window>(wWindowName, sWidth, sHeight);
	tmrTimer.set();
}

int Application::Start() {
	using uia = UI::Axis;
	ptrWnd->ui().split_panel({ 100, 100 }, uia::Vertical);
	ptrWnd->ui().split_panel({ 100, 100 }, uia::Vertical);
	ptrWnd->ui().split_panel({ 100, 100 }, uia::Horizontal);
	//ptrWnd->update();
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
			//std::vector<UIBase::UIResize> vResize{};
			if (ptrWnd->ui().cursor_in_resize_region(posCursor)) {
				ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::ResizeEW);
			}
			else {
				ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::Normal);
			}
			//if (vResize.size() != 0) {
			//	ptrWnd->ui().enter_resize_mode();
			//	std::byte bResizeFlags{ 0 };
			//	for (UIBase::UIResize v : vResize) {
			//		switch (*v.oResizeDirection) {
			//		case UIBase::Side::Top://change
			//		{
			//			bResizeFlags |= std::byte{ 0b1 };
			//			break;
			//		}
			//		case UIBase::Side::Bottom://change
			//		{
			//			bResizeFlags |= std::byte{ 0b10 };
			//			break;
			//		}
			//		default:
			//		{
			//			bResizeFlags |= std::byte{ 0b0 };
			//			break;
			//		}
			//		}
			//	
			//	}
			//	switch (bResizeFlags) {
			//	case std::byte{ 0b1 }:
			//	{
			//		ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::ResizeEW);
			//		break;
			//	}
			//	case std::byte{ 0b10 }:
			//	{
			//		ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::ResizeNS);
			//		break;
			//	}
			//	case std::byte{ 0b11 }:
			//	{
			//		ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::ResizeNSEW);
			//		break;
			//	}
			//	default:
			//	{
			//		ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::Normal);
			//		break;
			//	}
			//	}
			//}
			//else {
			//	ptrWnd->ptrMouse->update_cursor(Mouse::CursorType::Normal);
			//}
		}
		else if (e->get_type() == Mouse::Event::Type::LClick) {
			int breakpoint = 3;
		}
	}
}

void Application::DoFrame(const float fTime) const {
	ptrWnd->gfx().BeginFrame(ptrWnd->ui());

	ptrWnd->gfx().EndFrame();
}
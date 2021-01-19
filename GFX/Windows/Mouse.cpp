#include <queue>
#include "Mouse.h"
#include "clmWinStuff.h"

Mouse::Mouse(const Window& win) noexcept
	:
	win(win)
{
	bIsLeftPressed = false;
	bIsRightPressed = false;
	posCursor = { 0, 0 };
	hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), 2u, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
	SetCursor(hCursor);
	ct = CursorType::Normal;
}

Mouse::Event::Event(Type t, Pos p) noexcept : t(t), posCursor(p) {}

const Mouse::Pos& Mouse::get_pos() const noexcept {
	return posCursor;
}

std::int32_t Mouse::get_pos_x() const noexcept {
	return posCursor.first;
}

std::int32_t Mouse::get_pos_y() const noexcept {
	return posCursor.second;
}

bool Mouse::get_lbstate() const noexcept {
	return bIsLeftPressed;
}

bool Mouse::get_rbstate() const noexcept {
	return bIsRightPressed;
}

void Mouse::on_left_click(Pos pos) noexcept {
	buffer.emplace(Event::Type::LClick, pos );
	trim_buffer();
	return;
}

void Mouse::on_left_release(Pos pos) noexcept {
	buffer.emplace(Event::Type::LRelease, pos);
	trim_buffer();
	return;
}

void Mouse::on_right_click(Pos pos) noexcept {
	buffer.emplace(Event::Type::RClick, pos);
	trim_buffer();
	return;
}

void Mouse::on_right_release(Pos pos) noexcept {
	buffer.emplace(Event::Type::RRelease, pos);
	trim_buffer();
	return;
}

void Mouse::on_move(Pos pos) noexcept {
	buffer.emplace(Event::Type::Move, pos);
	trim_buffer();
	return;
}

void Mouse::trim_buffer() noexcept {
	while (buffer.size() > nBufferSize) {
		buffer.pop();
	}
}

std::optional<Mouse::Event> Mouse::get_next_event() noexcept {
	if (buffer.size() != 0) {
		Mouse::Event e = buffer.front();
		buffer.pop();
		return { e };
	}
	return {};
}

void Mouse::update_cursor_pos(Pos pos) noexcept {
	posCursor = pos;
}

void Mouse::update_cursor(CursorType ct_in) noexcept {
	if (ct_in != ct) {
		ct = ct_in;
		switch (ct) {
		case CursorType::Normal:
		{
			hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_NORMAL), 2u, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
			break;
		}
		case CursorType::ResizeEW:
		{
			hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_SIZEWE), 2u, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
			break;
		}
		case CursorType::ResizeNS:
		{
			hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_SIZENS), 2u, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
			break;
		}
		case CursorType::ResizeNSEW:
		{
			hCursor = (HCURSOR)LoadImage(NULL, MAKEINTRESOURCE(OCR_SIZEALL), 2u, 0, 0, LR_SHARED | LR_DEFAULTSIZE);
			break;
		}
		}
		SetCursor(hCursor);
	}
}

Mouse::Event::Type Mouse::Event::get_type() const noexcept {
	return t;
}

const Mouse::Pos& Mouse::Event::get_cursor_pos() const noexcept {
	return posCursor;
}
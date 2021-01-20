#ifndef MOUSE_H
#define MOUSE_H

#include "WindowsInclude.h"
#include <queue>
#include <cstdint>
#include <optional>

class Window;

class Mouse {
public:
	typedef std::pair<std::int32_t, std::int32_t> Pos;
	Mouse(const Window& win) noexcept;
	~Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;

	const Pos& get_pos() const noexcept;
	std::int32_t get_pos_x() const noexcept;
	std::int32_t get_pos_y() const noexcept;
	bool get_lbstate() const noexcept;
	bool get_rbstate() const noexcept;

	void on_left_click(Pos pos) noexcept;
	void on_left_release(Pos pos) noexcept;
	void on_right_click(Pos pos) noexcept;
	void on_right_release(Pos pos) noexcept;
	void on_move(Pos pos) noexcept;

	enum class CursorType {
		Normal,
		ResizeEW,
		ResizeNS,
		ResizeNSEW
	};
private:
	static constexpr std::uint8_t nBufferSize = 16u;
	void trim_buffer() noexcept;
public:
	class Event {
	public:
		enum class Type {
			LClick,
			LRelease,
			RClick,
			RRelease,
			Move
		};

		Event(Type t, Pos p) noexcept;
		~Event() = default;
	private:
		Pos posCursor;
		Type t;
	public:
		Type get_type() const noexcept;
		const Pos& get_cursor_pos() const noexcept;
	};
private:
	bool bIsLeftPressed;
	bool bIsRightPressed;
	Pos posCursor;
	std::queue<Event> buffer;
	const Window& win;
	HCURSOR hCursor;
	CursorType ct;
public:
	std::optional<Event> get_next_event() noexcept;
	void update_cursor_pos(Pos pos) noexcept;
	void update_cursor(CursorType ct_in) noexcept;
};

#endif
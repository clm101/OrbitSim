#ifndef UI_BASE_H
#define UI_BASE_H

#define VECTOR_BORDER 0

#include <d2d1.h>
#include <d2d1_1.h>
#include <memory>
#include <optional>
#include <vector>
#include <array>
//#include <typeinfo>
#include <type_traits>
#include <utility>
#include <cassert>
#ifndef __cpp_lib_concepts
#define __cpp_lib_concepts
#endif
#include <concepts>
#include "../../clmRandom.h"
#include "../../DebugDefines/MemoryDebug.h"

namespace UI {
	template<typename T> concept SideType = std::same_as<T, std::int32_t> || std::same_as<T, std::shared_ptr<std::int32_t>>;
	struct Side {
		std::int32_t x;

		constexpr Side(std::int32_t y = 0) : x(y) {}

		constexpr operator std::int32_t() const { return x; }

		static const Side Left, Top, Right, Bottom;
	};
	/*enum class Side : std::int32_t {
		Left = 0,
		Top = 1,
		Right = 2,
		Bottom = 3
	};*/

	enum class Axis : std::int32_t {
		Horizontal = 0,
		Vertical = 1
	};

	typedef std::pair<std::int32_t, std::int32_t> Pos;
	class Rect {
	private:
		std::array<std::shared_ptr<std::int32_t>, 4> aBorders;
		void init_array_elem(Side s, std::int32_t n) noexcept;
		void init_array_elem(Side s, std::shared_ptr<std::int32_t> ptr) noexcept;
	public:
		Rect() noexcept;
		//~Rect() noexcept;
		template<SideType T1, SideType T2, SideType T3, SideType T4>
		Rect(T1 t1, T2 t2, T3 t3, T4 t4) {
			init_array_elem(Side::Left, t1);
			init_array_elem(Side::Top, t2);
			init_array_elem(Side::Right, t3);
			init_array_elem(Side::Bottom, t4);
		}
		Rect(const Rect&) noexcept;
		Rect& operator=(const Rect&) noexcept;
		Rect(Rect&&) noexcept;
		Rect& operator=(Rect&&) noexcept;

		std::int32_t& operator[](UI::Side s) const noexcept;
		const std::shared_ptr<std::int32_t>& get_ptr(UI::Side s) const noexcept;
		void set_ptr(UI::Side s, std::shared_ptr<std::int32_t>&& ptr) noexcept;
		void set_side(UI::Side s, std::int32_t n) noexcept;
		void set_side(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept;
		//void set_sides(UI::Rect r) noexcept;
		template<SideType T1, SideType T2, SideType T3, SideType T4>
		void set_sides(T1 t1, T2 t2, T3 t3, T4 t4) noexcept {
			set_side(UI::Side::Left, t1);
			set_side(UI::Side::Top, t2);
			set_side(UI::Side::Right, t3);
			set_side(UI::Side::Bottom, t4);
		}
	};
}
using uis = UI::Side;

class UIBaseElem {
private:
protected:
	UIBaseElem() {}
	UIBaseElem(UI::Rect r) { rBorder = std::move(r); }
public:
	UI::Rect rBorder;
	virtual void draw(ID2D1DeviceContext* ptrContext) = 0;
	bool contains_cursor(const UI::Pos&) const noexcept;
	void set_side(UI::Side s, std::int32_t n) noexcept;
	void set_side(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept;
	template<UI::SideType T1, UI::SideType T2, UI::SideType T3, UI::SideType T4>
	void set_sides(T1 t1, T2 t2, T3 t3, T4 t4) noexcept {
		set_side(uis::Left, t1);
		set_side(uis::Top, t2);
		set_side(uis::Right, t3);
		set_side(uis::Bottom, t4);
	}
	virtual ~UIBaseElem() = default;
	virtual bool cursor_in_resize_region(const UI::Pos& pos) const noexcept;
};

#if VECTOR_BORDER
class Panel {
private:

public:
	void draw(ID2D1DeviceContext* ptrContext) noexcept;
};
#else
class Panel : public UIBaseElem {
	friend class UIManager;
public:
	template<typename T1>
	Panel(T1 t1) noexcept {
		assert(typeid(t1) == typeid(UI::Rect));
		//static_assert(typeid(T2) == typeid(const UIBase*), "Invalid T2: Panel(2 args)");

		rBorder = std::move(t1);
		color = { getrandom(), getrandom(), getrandom(), 1.0f };
		ptrBrush = nullptr;
	}
	template<UI::SideType T1, UI::SideType T2, UI::SideType T3, UI::SideType T4>
	Panel(T1 t1, T2 t2, T3 t3, T4 t4) noexcept {
		set_dim(uis::Left, t1);
		set_dim(uis::Top, t2);
		set_dim(uis::Right, t3);
		set_dim(uis::Bottom, t4);
		color = { getrandom(), getrandom(), getrandom(), 1.0f };
		ptrBrush = nullptr;
	}

	~Panel() noexcept override;
	Panel(const Panel&) = delete;
	void draw(ID2D1DeviceContext* ptrContext) override;
private:
	struct {
		float r = 0.f;
		float g = 0.f;
		float b = 0.f;
		float a = 1.f;
	} color;
	//uib::Rect dim;
	ID2D1SolidColorBrush* ptrBrush;
	//bool contains_cursor(const Pos&) const noexcept;
	//void cursor_in_resize_region(const Pos& pos, std::vector<UIResize>& v) const noexcept;

	void set_dim(UI::Side s, std::int32_t n) noexcept;
	void set_dim(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept;
	void set_dims(UI::Rect&& r_in) noexcept;
	template<UI::SideType T1, UI::SideType T2, UI::SideType T3, UI::SideType T4>
	void set_dim(T1 t1, T2 t2, T3 t3, T4 t4) {
		set_dim(uis::Left, t1);
		set_dim(uis::Top, t2);
		set_dim(uis::Right, t3);
		set_dim(uis::Bottom, t4);
	}

	//void set_dim(Side s, std::int32_t n) noexcept;
	//void set_dim(Side s, std::shared_ptr<std::int32_t> ptr) noexcept;
};
#endif

class Zone : public UIBaseElem {
	friend class UIManager;
private:
	//uib::Rect dim;
	std::unique_ptr<UIBaseElem> ptrFirst;
	std::unique_ptr<UIBaseElem> ptrSecond;
	//bool contains_cursor(const Pos&) const noexcept override;
	void find_and_split_panel(const UI::Pos&, const UI::Axis&) noexcept;
	static void split_panel(std::unique_ptr<UIBaseElem>& ptr, const UI::Axis& a) noexcept;
public:
	Zone(const UI::Rect dim_in, std::unique_ptr<UIBaseElem>&& ptrFirst_in, UIBaseElem* ptrSecond_in = nullptr) noexcept;
	Zone(UI::Rect&& r_in) noexcept;
	~Zone() noexcept override = default;
	void draw(ID2D1DeviceContext* ptrContext) override;
	bool cursor_in_resize_region(const UI::Pos& p) const noexcept override;
	//void cursor_in_resize_region(const Pos& pos, std::vector<UIBase::UIResize>& v) const noexcept override;
};

#endif
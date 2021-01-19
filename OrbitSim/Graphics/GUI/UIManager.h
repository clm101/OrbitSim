#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <memory>
#include <vector>
#include "UIBaseElem.h"
#include "../../DebugDefines/MemoryDebug.h"

struct Dimensions {
	Dimensions(std::int32_t w, std::int32_t h) : nWidth(w), nHeight(h) {}
	std::int32_t nWidth;
	std::int32_t nHeight;
};

struct Border {
	std::shared_ptr<std::int32_t> ptrStart;
	std::shared_ptr<std::int32_t> ptrEnd;
	std::shared_ptr<std::int32_t> ptrPosition;	// Position refers to the veritcal position for a horizontal border or horizontal position for a vertical border
};

//class UIManagerRect : public UIBase::Rect {
//	//void set_side(UIBase::Side s, std::int32_t n) noexcept override { return; }
//	//void set_side(UIBase::Side s, std::shared_ptr<std::int32_t> ptr) noexcept override { return; }
//};

class UIManager {
public:
	UIManager(UI::Rect r) noexcept;
	~UIManager() = default;
	UIManager(const UIManager&) = delete;
	UIManager& operator=(const UIManager&) = delete;
	void draw(ID2D1DeviceContext* ptrContext) noexcept;
	void split_panel(UI::Pos p, UI::Axis a) noexcept;
	//static void cursor_in_resize_region_util(const std::unique_ptr<UIBase>& ptr, const Pos& pos, std::vector<UIResize>& v) noexcept;
	enum class UIBESelect : std::int32_t {
		First = 0,
		Second = 1
	};
private:
#if VECTOR_BORDER
	std::vector<Border> vHorizontalBorders;
	std::vector<Border> vVerticalBorders;
	std::vector<Panel> vPanels;

	void create_panel(const UI::Pos& pos, const UI::Axis& a) noexcept;
	//void create_panel_util(const UI::Pos& pos, std::vector<Border>& vTarget) noexcept;
#else
	UI::Rect rBorder;
	//UIManagerRect dim;
	std::unique_ptr<UIBaseElem> ptrFirst;
	std::unique_ptr<UIBaseElem> ptrSecond;

	template<typename T>
	static void find_and_split_panel_util(T* ptr, const UI::Pos& pos, const UI::Axis& a) noexcept;
	void create_zone_split_panel(const UIBESelect s, const UI::Axis& a) noexcept;
	void find_and_split_panel(const UI::Pos& pos, const UI::Axis& st) noexcept;
#endif

public:
	//void cursor_in_resize_region(const UI::Pos& pos, std::vector<UIResize>& v) const noexcept override;

	std::byte bFlags;
	static constexpr std::byte RESIZE_MODE = std::byte{ 0b1 };
	static constexpr std::byte RESIZE_ACTIVE = std::byte{ 0b10 };
	struct ResizingElement {
		UIBaseElem* ptrElement;
		UI::Side s;
	};
	void enter_resize_mode() noexcept;
	void exit_resize_mode() noexcept;
	void update_dimensions(const UI::Pos& posCursor) noexcept;
	bool cursor_in_resize_region(const UI::Pos& p) noexcept;
};

//template<typename T>
//void UIManager::find_and_split_panel_util(T* ptr, const UI::Pos& pos, const UI::Axis& st) noexcept {
//	if (ptr->ptrFirst->contains_cursor(pos)) {
//		switch ((ElementType) * (ptr->ptrFirst.get())) {
//		case ElementType::Panel:
//		{
//			create_zone_split_panel(ptr->ptrFirst, st);
//			break;
//		}
//		case ElementType::Zone:
//		{
//			find_and_split_panel(ptr->ptrFirst.get(), pos, st);
//			break;
//		}
//		}
//	}
//	else {
//		switch ((ElementType) * (ptr->ptrSecond.get())) {
//		case ElementType::Panel:
//		{
//			create_zone_split_panel(ptr->ptrSecond, st);
//			break;
//		}
//		case ElementType::Zone:
//		{
//			find_and_split_panel(ptr->ptrSecond.get(), pos, st);
//			break;
//		}
//		}
//	}
//}

#endif
#include "UIBaseElem.h"
#include "UIManager.h"

Zone::Zone(const UI::Rect dim_in, std::unique_ptr<UIBaseElem>&& ptrFirst_in, UIBaseElem* ptrSecond_in) noexcept {
	rBorder = dim_in;
	ptrFirst.reset(ptrFirst_in.release());
	ptrSecond.reset(ptrSecond_in);
}

Zone::Zone(UI::Rect&& r_in) noexcept {
	rBorder = r_in;
}

void Zone::draw(ID2D1DeviceContext* ptrContext) {
	if (ptrFirst != nullptr) {
		ptrFirst->draw(ptrContext);
	}
	if (ptrSecond != nullptr) {
		ptrSecond->draw(ptrContext);
	}
}

void Zone::split_panel(std::unique_ptr<UIBaseElem>& ptr, const UI::Axis& a) noexcept {
	using uis = UI::Side;
	using uia = UI::Axis;
	UI::Rect rZoneDim = std::move(ptr->rBorder);
	UI::Rect rFirstDim{};
	UI::Rect rSecondDim{};
	if (a == uia::Horizontal) {
		rFirstDim.set_sides(rZoneDim.get_ptr(uis::Left), rZoneDim.get_ptr(uis::Top),
			rZoneDim.get_ptr(uis::Right), (rZoneDim[uis::Top] + rZoneDim[uis::Bottom]) / 2);
		rSecondDim.set_sides(rZoneDim.get_ptr(uis::Left), rFirstDim.get_ptr(uis::Bottom),
			rZoneDim.get_ptr(uis::Right), rZoneDim.get_ptr(uis::Bottom));
	}
	else {
		rFirstDim.set_sides(rZoneDim.get_ptr(uis::Left), rZoneDim.get_ptr(uis::Top),
			(rZoneDim[uis::Left] + rZoneDim[uis::Right]) / 2, rZoneDim.get_ptr(uis::Bottom));
		rSecondDim.set_sides(rFirstDim.get_ptr(uis::Right), rZoneDim.get_ptr(uis::Top),
			rZoneDim.get_ptr(uis::Right), rZoneDim.get_ptr(uis::Bottom));
	}

	std::unique_ptr<Zone> ptrZone = std::make_unique<Zone>(std::move(rZoneDim));
	ptr->rBorder = std::move(rFirstDim);
	ptrZone->ptrFirst = std::move(ptr);
	ptrZone->ptrSecond = std::make_unique<Panel>(rSecondDim);
	ptr = std::move(ptrZone);
}

void Zone::find_and_split_panel(const UI::Pos& pos, const UI::Axis& a) noexcept {
	if (ptrFirst->contains_cursor(pos)) {
		if (Zone* ptrZone = dynamic_cast<Zone*>(ptrFirst.get())) {
			ptrZone->find_and_split_panel(pos, a);
			ptrZone = nullptr;
		}
		else {
			split_panel(ptrFirst, a);
		}
	}
	else {
		if (Zone* ptrZone = dynamic_cast<Zone*>(ptrSecond.get())) {
			ptrZone->find_and_split_panel(pos, a);
			ptrZone = nullptr;
		}
		else {
			split_panel(ptrFirst, a);
		}
	}
}

bool Zone::cursor_in_resize_region(const UI::Pos& p) const noexcept {
	return ptrFirst->cursor_in_resize_region(p) || ptrSecond->cursor_in_resize_region(p);
}
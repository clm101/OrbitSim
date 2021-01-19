#include "UIBaseElem.h"
#include "../clmGraphics.h"
#include "../DxgiInfoManager.h"
#include <array>
#include <cassert>
#include "../../clmRandom.h"

//Panel::Panel(const uib::Rect dim_in, const UIBase* parent) noexcept {
//	dim.set_sides_single(dim_in);
//	color = { getrandom(), getrandom(), getrandom(), 1.0f };
//	ptrBrush = nullptr;
//};
#if VERTEX_BORDER

#else
Panel::~Panel() noexcept {
	SafeRelease(ptrBrush);
}

void Panel::draw(ID2D1DeviceContext* ptrContext) {
	if (ptrBrush == nullptr) {
		static HRESULT hr = S_OK;
		CLM_EXCEPT_GFX_HR(ptrContext->CreateSolidColorBrush(D2D1::ColorF(color.r, color.g,
			color.b, color.a), &ptrBrush));
	}
	assert(rBorder.get_ptr(uis::Left) != nullptr);
	assert(rBorder.get_ptr(uis::Top) != nullptr);
	assert(rBorder.get_ptr(uis::Right) != nullptr);
	assert(rBorder.get_ptr(uis::Bottom) != nullptr);
	D2D1_RECT_F rect = { (float)rBorder[uis::Left], (float)rBorder[uis::Top],
		(float)rBorder[uis::Right], (float)rBorder[uis::Bottom] };

	ptrContext->FillRectangle(rect, ptrBrush);
	return;
}

//bool Panel::contains_cursor(const Pos& pos) const noexcept {
//	return ((pos.first > (*ptrDim)[Side::Left]) && (pos.first < (*ptrDim)[Side::Right])) &&
//		((pos.second > (*ptrDim)[Side::Top]) && (pos.second < (*ptrDim)[Side::Bottom]));
//}

//void Panel::set_dim(Side s, std::int32_t n) noexcept {
//	dim.set_side_single(s, n);
//}
//
//void Panel::set_dim(Side s, std::shared_ptr<std::int32_t> ptr) noexcept {
//	dim.set_side_global(s, ptr);
//}

void Panel::set_dim(UI::Side side, std::int32_t n) noexcept {
	rBorder.set_ptr(side, std::make_shared<std::int32_t>(n));
}

void Panel::set_dim(UI::Side side, std::shared_ptr<std::int32_t> ptr) noexcept {
	rBorder.set_ptr(side, std::move(ptr));
}

void Panel::set_dims(UI::Rect&& r_in) noexcept {
	rBorder = r_in;
}
#endif
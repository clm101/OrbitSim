#include <immintrin.h>
#include "UIBaseElem.h"
#include "../../clmRandom.h"

UI::Rect::Rect() noexcept {
	for (size_t i = 0; i < 4; i++) {
		aBorders[i] = nullptr;
	}
}
UI::Rect::Rect(const Rect& r_in) noexcept {
	if (this != &r_in) {
		for (size_t i = 0; i < 4; i++) {
			aBorders[i] = r_in.aBorders[i];
		}
	}
}

UI::Rect& UI::Rect::operator=(const UI::Rect& r_in) noexcept {
	if (this != &r_in) {
		for (size_t i = 0; i < 4; i++) {
			if (aBorders[i] != r_in.aBorders[i]) {
				aBorders[i] = r_in.aBorders[i];
			}
		}
	}
	return *this;
}

UI::Rect::Rect(Rect&& r_in) noexcept {
	if (this != &r_in) {
		for (size_t i = 0; i < 4; i++) {
			if (aBorders[i] != r_in.aBorders[i]) {
				aBorders[i] = std::move(r_in.aBorders[i]);
				/*aBorders[i].reset(r_in.aBorders[i].get());
				r_in.aBorders[i].reset();*/
			}
		}
	}
}

UI::Rect& UI::Rect::operator=(Rect&& r_in) noexcept {
	if (this != &r_in) {
		for (size_t i = 0; i < 4; i++) {
			if (aBorders[i] != r_in.aBorders[i]) {
				aBorders[i] = std::move(r_in.aBorders[i]);
				/*aBorders[i].reset(r_in.aBorders[i].get());
				r_in.aBorders[i].reset();*/
			}
		}
	}
	return *this;
}

void UI::Rect::init_array_elem(UI::Side s, std::int32_t n) noexcept {
	aBorders[s] = std::make_shared<std::int32_t>(n);
	return;
}

void UI::Rect::init_array_elem(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept {
	aBorders[s] = ptr;
	return;
}

std::int32_t& UI::Rect::operator[](UI::Side s) const noexcept {
	return *(aBorders[s]);
}

const std::shared_ptr<std::int32_t>& UI::Rect::get_ptr(UI::Side s) const noexcept {
	return aBorders[s];
}

void UI::Rect::set_ptr(UI::Side s, std::shared_ptr<std::int32_t>&& ptr) noexcept {
	aBorders[s] = ptr;
}

void UI::Rect::set_side(UI::Side s, std::int32_t n) noexcept {
	aBorders[s] = std::make_shared<std::int32_t>(n);
}

void UI::Rect::set_side(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept {
	aBorders[s] = ptr;
}

/*********************************/
/*********** UIBaseElem **********/
/*********************************/

constexpr UI::Side uis::Left{ 0 }, uis::Top{ 1 }, uis::Right{ 2 }, uis::Bottom{ 3 };

bool UIBaseElem::contains_cursor(const UI::Pos& pos) const noexcept {
	using uis = UI::Side;
	return ((pos.first > rBorder[uis::Left]) && (pos.first < rBorder[uis::Right])) &&
		((pos.second > rBorder[uis::Top]) && (pos.second < rBorder[uis::Bottom]));
}

void UIBaseElem::set_side(UI::Side s, std::int32_t n) noexcept {
	rBorder[s] = n;
}

void UIBaseElem::set_side(UI::Side s, std::shared_ptr<std::int32_t> ptr) noexcept {
	rBorder.set_ptr(s, std::move(ptr));
}

// Over-engineered getting whether or not cursor is in the resize region
// This can be extended to determining whether or not the cursor is in a panel, the only thing that changes is the nThreshold comparison
bool UIBaseElem::cursor_in_resize_region(const UI::Pos& pos) const noexcept {
	constexpr std::int32_t nThreshold = 5;

	const __m128i mPos = _mm_set_epi32(pos.first, pos.first, pos.second, pos.second);
	const __m128i mBorders = _mm_set_epi32(rBorder[uis::Left], rBorder[uis::Right], rBorder[uis::Top], rBorder[uis::Bottom]);
	__m128i mDist = _mm_sub_epi32(mPos, mBorders);
	mDist = _mm_abs_epi32(mDist);
	std::array<std::int32_t, 4> aDists = { _mm_extract_epi32(mDist, uis::Left), _mm_extract_epi32(mDist, uis::Top),
											_mm_extract_epi32(mDist, uis::Right), _mm_extract_epi32(mDist, uis::Bottom) };
	std::int32_t nMin = aDists[0];
	size_t nIndex = 0;
	for (size_t i = 1; i < 4; i++) {
		if (aDists[i] < nMin) {
			nMin = aDists[i];
			nIndex = i;
		}
	}
	
	return nMin < nThreshold;
}
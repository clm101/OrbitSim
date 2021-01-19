#include "UIManager.h"

UIManager::UIManager(UI::Rect r) noexcept
{
#if VECTOR_BORDER
#else
	rBorder = r;
	ptrFirst = std::make_unique<Panel>(rBorder);
	ptrSecond = nullptr;
	bFlags = std::byte{ 0 };
#endif
}

void UIManager::draw(ID2D1DeviceContext* ptrContext) noexcept {
#if VECTOR_BORDER
	for (Panel& p : vPanels) {
		p.draw(ptrContext);
	}
#else
	if (ptrFirst != nullptr) {
		ptrFirst->draw(ptrContext);
	}
	if (ptrSecond != nullptr) {
		ptrSecond->draw(ptrContext);
	}
#endif
}

#if VECTOR_BORDER

void UIManager::create_panel(const UI::Pos& pos, const UI::Axis& a) noexcept {
	size_t nStart = 0;
	size_t nEnd = vVerticalBorders.size();
	size_t nVecPos = (nStart + nEnd) / 2;
	while (true) {
		if (pos.first < vVerticalBorders[nVecPos].nPosition) {
			nEnd = nVecPos;
		}
		else {
			nStart = nVecPos;
		}
		assert(nEnd == nStart);
		assert((std::int32_t)nEnd - (std::int32_t)nStart > 0);
		if ((nEnd - nStart) == 1) {
			break;
		}
		nVecPos = (nStart + nEnd) / 2;
	}
	auto iVerticalInsert = vVerticalBorders.begin() + nEnd;

	nStart = 0;
	nEnd = vHorizontalBorders.size();
	nVecPos = (nStart + nEnd) / 2;
	while (true) {
		if (pos.second < *(vHorizontalBorders[nVecPos].ptrPosition)) {
			nEnd = nVecPos;
		}
		else {
			nStart = nVecPos;
		}
		assert(nEnd == nStart);
		assert((std::int32_t)nEnd - (std::int32_t)nStart > 0);
		if ((nEnd - nStart) == 1) {
			break;
		}
		nVecPos = (nStart + nEnd) / 2;
	}
	auto iHorizontalInsert = vHorizontalBorders.begin() + nEnd;

	Border newBorder{};
	if (a == UI::Axis::Horizontal) {
		newBorder.ptrStart = (iVerticalInsert - 1)->ptrPosition;
		newBorder.ptrEnd = iVerticalInsert->ptrPosition;
		newBorder.ptrPosition = std::make_shared<std::int32_t>((*((iHorizontalInsert - 1)->ptrPosition) + *(iHorizontalInsert->ptrPosition)) / 2);
		iHorizontalInsert = vHorizontalBorders.insert(iHorizontalInsert, std::move(newBorder));
	}
	else {
		newBorder.ptrStart = (iHorizontalInsert - 1)->ptrPosition;
		newBorder.ptrEnd = iHorizontalInsert->ptrPosition;
		newBorder.ptrPosition = std::make_shared<std::int32_t>((*((iVerticalInsert - 1)->ptrPosition) + *(iVerticalInsert->ptrPosition)) / 2);
		iVerticalInsert = vVerticalBorders.insert(iVerticalInsert, std::move(newBorder));
	}


}

#else
void UIManager::create_zone_split_panel(const UIBESelect s, const UI::Axis& a) noexcept {
	using uia = UI::Axis;
	using uis = UI::Side;
	UI::Rect rZoneDim{};
	UI::Rect rFirstDim{};
	UI::Rect rSecondDim{};

	if (s == UIBESelect::First) {
		rZoneDim = std::move(ptrFirst->rBorder);
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
		ptrFirst->rBorder = std::move(rFirstDim);
		ptrZone->ptrFirst = std::move(ptrFirst);
		ptrZone->ptrSecond = std::make_unique<Panel>(rSecondDim);
		ptrFirst = std::move(ptrZone);
	}
	else {
		rZoneDim = std::move(ptrSecond->rBorder);
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
		ptrSecond->rBorder = std::move(rSecondDim);
		ptrZone->ptrSecond = std::move(ptrSecond);
		ptrZone->ptrFirst = std::make_unique<Panel>(rFirstDim);
		ptrSecond = std::move(ptrZone);
	}
}

void UIManager::find_and_split_panel(const UI::Pos& pos, const UI::Axis& a) noexcept {
	
}

void UIManager::split_panel(UI::Pos p, UI::Axis a) noexcept {
	using uis = UI::Side;
	using uia = UI::Axis;
	if (ptrSecond == nullptr) {
		Panel* ptrFirstTmp = dynamic_cast<Panel*>(ptrFirst.get());
		assert(ptrFirst != nullptr);
		const UI::Rect rOldDim = ptrFirstTmp->rBorder;
		switch (a) {
		case uia::Vertical:
		{
			/*ptrFirstTmp->set_dim({
				rOldDim.nLeft, rOldDim.nTop,
				(rOldDim.nRight + rOldDim.nLeft) / 2, rOldDim.nBottom });*/

			/*ptrFirstTmp->set_dim(0, rOldDim.get_ptr(Side::Left));
			ptrFirstTmp->set_dim(1, rOldDim.get_ptr(Side::Top));
			ptrFirstTmp->set_dim(2, (rOldDim[Side::Right] + rOldDim[Side::Left]) / 2);
			ptrFirstTmp->set_dim(3, rOldDim.get_ptr(Side::Bottom));*/

			/*ptrFirstTmp->set_dim(0, rOldDim.get_ptr(Side::Left), rOldDim.get_ptr(Side::Top),
				(rOldDim[Side::Right] + rOldDim[Side::Left]) / 2, rOldDim.get_ptr(Side::Bottom));*/
			ptrFirstTmp->set_dim(rOldDim.get_ptr(uis::Left), rOldDim.get_ptr(uis::Top),
				(rOldDim[uis::Right] + rOldDim[uis::Left]) / 2, rOldDim.get_ptr(uis::Bottom));

			ptrSecond = std::make_unique<Panel>(ptrFirstTmp->rBorder.get_ptr(uis::Right), rOldDim.get_ptr(uis::Top),
				rOldDim.get_ptr(uis::Right), rOldDim.get_ptr(uis::Bottom));

			break;
		}
		case uia::Horizontal:
		{
			//ptrFirstTmp->set_dim({
			//	rOldDim.nLeft, rOldDim.nTop,
			//	rOldDim.nRight, (rOldDim.nBottom + rOldDim.nTop) / 2});
			ptrFirstTmp->set_dim(rOldDim.get_ptr(uis::Left), rOldDim.get_ptr(uis::Top),
				rOldDim.get_ptr(uis::Right), (rOldDim[uis::Bottom] + rOldDim[uis::Top]) / 2);
			ptrSecond = std::make_unique<Panel>(rOldDim.get_ptr(uis::Left), ptrFirstTmp->rBorder.get_ptr(uis::Bottom),
				rOldDim.get_ptr(uis::Right), rOldDim.get_ptr(uis::Bottom));
			break;
		}
		}
	}
	else {
		if (ptrFirst->contains_cursor(p)) {
			if (Zone* ptrZone = dynamic_cast<Zone*>(ptrFirst.get())) {
				ptrZone->find_and_split_panel(p, a);
			}
			else {
				create_zone_split_panel(UIBESelect::First, a);
			}
		}
		else {
			if (Zone* ptrZone = dynamic_cast<Zone*>(ptrSecond.get())) {
				ptrZone->find_and_split_panel(p, a);
			}
			else {
				create_zone_split_panel(UIBESelect::Second, a);
			}
		}
	}
	return;
}
#endif

//void UIManager::cursor_in_resize_region(const UI::Pos& pos, std::vector<UIResize>& v) const noexcept {
//	if (ptrFirst != nullptr) {
//		ptrFirst->cursor_in_resize_region(pos, v);
//	}
//	if (ptrSecond != nullptr) {
//		ptrSecond->cursor_in_resize_region(pos, v);
//	}
//}

void UIManager::enter_resize_mode() noexcept {
	bFlags |= RESIZE_MODE;
}

void UIManager::exit_resize_mode() noexcept {
	bFlags &= ~RESIZE_MODE;
}

void UIManager::update_dimensions(const UI::Pos& posCursor) noexcept {
	if ((bFlags & RESIZE_ACTIVE) == RESIZE_ACTIVE) {

	}
}

bool UIManager::cursor_in_resize_region(const UI::Pos& p) noexcept {
#if VECTOR_BORDER
#else
	bool bReturn = false;

	if (ptrFirst != nullptr) {
		bReturn |= ptrFirst->cursor_in_resize_region(p);
	}
	if (ptrSecond != nullptr) {
		bReturn |= ptrSecond->cursor_in_resize_region(p);
	}

	return bReturn;
#endif
}

//void UIManager::cursor_in_resize_region_util(const std::unique_ptr<UIBase>& ptr, const Pos& pos, std::vector<UIResize>& v) noexcept {
//	if (ptr != nullptr) {
//		return ptr->cursor_in_resize_region(pos);
//	}
//	return { false };
//}
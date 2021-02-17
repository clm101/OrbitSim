#include "Graph.h"

namespace clm {
	void Graph::set_x_min(float x) noexcept
	{
		mf_XMin = x;
	}

	void Graph::set_x_max(float x) noexcept
	{
		mf_XMax = x;
	}

	void Graph::set_x_res(size_t x) noexcept
	{
		mn_XRes = x;
	}

	void Graph::set_y_min(float y) noexcept
	{
		mf_YMin = y;
	}

	void Graph::set_y_max(float y) noexcept
	{
		mf_YMax = y;
	}

	void Graph::set_y_res(size_t y) noexcept
	{
		mn_YRes = y;
	}

	void Graph::set_graph_title(const std::wstring& str) noexcept
	{
		strTitle = str;
	}

	void Graph::draw(const Graphics& gfx) const
	{
		Geometry::Rect graphArea = { m_bounds.fLeft + fXSpacing, m_bounds.fTop + fYSpacing, m_bounds.fRight - fXSpacing, m_bounds.fBottom - fYSpacing };
		gfx.draw_rect(graphArea, GFXUtil::Color::White);
	}
}
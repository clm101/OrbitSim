#include <vector>
#include <string>
#include "Graphics/Graphics.h"

#ifndef GRAPH_H
#define GRAPH_H
namespace clm {
	class Graph {
	public:
		Graph() = default;
		Graph(const Geometry::Rect& rect) : m_bounds(rect) {}
		~Graph() = default;

		void set_x_min(float) noexcept;
		void set_x_max(float) noexcept;
		void set_x_res(size_t) noexcept;

		void set_y_min(float) noexcept;
		void set_y_max(float) noexcept;
		void set_y_res(size_t) noexcept;

		void set_graph_title(const std::wstring&) noexcept;

		void set_boundary(const Geometry::Rect& r) noexcept { m_bounds = r; }

		void draw(const Graphics&) const;
	private:
		//inline Geometry::Rect convert_rect(const Geometry::Rect&) const noexcept;

		Geometry::Rect m_bounds;

		std::vector<std::pair<float, float>> mv_DataPoints;

		float mf_XMin;
		float mf_XMax;
		size_t mn_XRes;

		float mf_YMin;
		float mf_YMax;
		size_t mn_YRes;

		std::wstring strTitle;
		std::wstring strXAxisTitle;
		std::wstring strYAxisTitle;

		static constexpr float mf_MinWidth = 50.0f;
		static constexpr float fXSpacing = 10.0f;
		static constexpr float fYSpacing = 10.0f;
		static constexpr float fTicLen = 3.0f;
	};
}
#endif
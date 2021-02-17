#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "../Windows/WindowsInclude.h"
#include <d3d11.h>
#include <windows.foundation.h>
#include <wrl\wrappers\corewrappers.h>
#include <wrl\client.h>
#include <dxgi1_3.h>
#include <Dxgi1_6.h>
#include <vector>
#include <d2d1.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include "DxgiInfoManager.h"
#include "../ExceptionBase.h"
#include "../DebugDefines/GraphicsDebugDefines.h"
#include "../DebugDefines/MemoryDebug.h"
#include "../MathVector.h"

namespace mswrl = Microsoft::WRL;

namespace clm {
	namespace Geometry {
		struct Circle {
			float x, y;
			float r;

			Circle() = default;
			Circle(math::Point2D_F pt_in, float r_in) : x(pt_in.get_x()), y(pt_in.get_y()), r(r_in) {}
			~Circle() = default;
		};

		struct Rect {
			float fLeft, fTop;
			float fRight, fBottom;
		};
	}

	namespace GFXUtil {
		enum class Color {
			White,
			Red,
			Green,
			Blue
		};
	}
	class Graphics {
	private:
		// Set up device
		// D3D11
		mswrl::ComPtr<IDXGIFactory> ptrFactory;
		mswrl::ComPtr<IDXGIAdapter> ptrAdapter;
		mswrl::ComPtr<ID3D11Device> ptrDevice;
		mswrl::ComPtr<IDXGISwapChain> ptrSwapChain;
		mswrl::ComPtr<ID3D11DeviceContext> ptrDeviceContext;
		mswrl::ComPtr<ID3D11RenderTargetView> ptrTarget;

		// Direct2D
		mswrl::ComPtr<IDXGIDevice> ptrDXGIDevice;
		mswrl::ComPtr<ID2D1Device> ptrD2DDevice;
		mswrl::ComPtr<ID2D1DeviceContext> ptrD2DDeviceContext;
		//IDXGISurface* ptrDXGISurface;
		mswrl::ComPtr<ID2D1Bitmap1> ptrBitmap;

		// Pixel Shader
		mswrl::ComPtr<ID3D11PixelShader> ptrPixelShader;
		mswrl::ComPtr<ID3DBlob> ptrPSBlob;
		mswrl::ComPtr<ID3DBlob> ptrPSErrorBlob;
		mswrl::ComPtr<ID3D11Buffer> ptrPSBuffer;

		// Vertex Shader
		mswrl::ComPtr<ID3D11VertexShader> ptrVertexShader;
		mswrl::ComPtr<ID3DBlob> ptrVSBlob;
		mswrl::ComPtr<ID3DBlob> ptrVSErrorBlob;
		mswrl::ComPtr<ID3D11Buffer> ptrVBuffer;
		mswrl::ComPtr<ID3D11InputLayout> ptrInputLayout;
		DB_INFO_MANAGER_DECL();

		// Text Rendering
		mswrl::ComPtr<IDWriteFactory> ptrWriteFactory;

		D2D1::ColorF get_color(const GFXUtil::Color) const noexcept;
		void draw_circle_impl(const Geometry::Circle&, const GFXUtil::Color = GFXUtil::Color::White) const;
		void draw_rect_impl(const Geometry::Rect&, const GFXUtil::Color = GFXUtil::Color::Green) const;
	public:
		class GFXExceptionBase : public ExceptionBase {
			using ExceptionBase::ExceptionBase;
		public:
			static std::string getGfxErrorString(HRESULT) noexcept;
		};
		class GFXException : public GFXExceptionBase {
		private:
			HRESULT hrError;
			std::string strErrorMsg;
			std::string strInfoMsgs;
		public:
			GFXException(int, const char*, HRESULT, std::vector<std::string> = {}) noexcept;
			const char* getType() const noexcept override;
			const std::string& getErrorMsg() const noexcept;
			const std::string& getInfoMsgs() const noexcept;
			const char* what() const noexcept override;
		};

		Graphics(HWND);
		~Graphics();
		Graphics(const Graphics&) = delete;
		Graphics& operator=(const Graphics&) = delete;

		void BeginFrame();
		void EndFrame();
		void ClearBuffer(const double);

		//void conv_points(float&, float&) noexcept;

		math::Vec2D_F get_screen_size() const;

		void draw_circle(const Geometry::Circle, const GFXUtil::Color = GFXUtil::Color::White) const;

		void draw_rect(const Geometry::Rect&, const GFXUtil::Color = GFXUtil::Color::Green) const;
		void draw_bounding_square(const Geometry::Circle, const GFXUtil::Color = GFXUtil::Color::Green);

		//void draw_grid(const std::vector<math::Vec2D_F>&, const float, const GFXUtil::Color = GFXUtil::Color::Green);

		//void draw_circle_with_grid(const Geometry::Circle, const std::vector<math::Vec2D_F>&, const float, const GFXUtil::Color = GFXUtil::Color::White, const GFXUtil::Color = GFXUtil::Color::Green);

		void draw_line(float, float, float, float, const GFXUtil::Color = GFXUtil::Color::Red) const;

		void write_text(const std::wstring& str, Geometry::Rect) const noexcept;
	};

	template<class T>
	inline void SafeRelease(T* ptr) {
		if (ptr != nullptr) {
			ptr->Release();

		}
		return;
	}

}
#endif
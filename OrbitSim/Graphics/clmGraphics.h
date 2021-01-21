#ifndef CLM_GRAPHICS_H
#define CLM_GRAPHICS_H

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
#include "DxgiInfoManager.h"
#include "../ExceptionBase.h"
#include "../DebugDefines/GraphicsDebugDefines.h"
#include "../DebugDefines/MemoryDebug.h"
#include "../VectorMath.h"

namespace mswrl = Microsoft::WRL;

namespace GFX {
	struct Circle {
		float x, y;
		float r;
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
	ID2D1Device* ptrD2DDevice;
	ID2D1DeviceContext* ptrD2DDeviceContext;
	//IDXGISurface* ptrDXGISurface;
	ID2D1Bitmap1* ptrBitmap;

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

	void draw_circle(const float, const float, const float);
	void draw_circle(const GFX::Circle);

	void draw_square(const float, const float, const float);
	void draw_square(const GFX::Circle);

	void draw_circle_with_grid(const GFX::Circle, size_t);

	void draw_line(float, float, float, float);
};

template<class T>
inline void SafeRelease(T* ptr) {
	if (ptr != nullptr) {
		ptr->Release();
		
	}
	return;
}
#endif
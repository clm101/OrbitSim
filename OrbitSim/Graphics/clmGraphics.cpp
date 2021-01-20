#include <combaseapi.h>
#include <d3dcompiler.h>
#include <numbers>
#include "../clmRandom.h"
#include "clmGraphics.h"
#include "../DebugDefines/GraphicsDebugDefines.h"
#include "DxgiInfoManager.h"

std::string Graphics::GFXExceptionBase::getGfxErrorString(HRESULT hr) noexcept {
	char* pMsg;

	if (!FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS
		, nullptr, hr, LocaleNameToLCID(LOCALE_NAME_SYSTEM_DEFAULT, LOCALE_ALLOW_NEUTRAL_NAMES),
		reinterpret_cast<LPSTR>(&pMsg), 0, nullptr)) {
		return "Unidentified error code";
	}

	std::string sReturn = pMsg;
	LocalFree(pMsg);
	return sReturn;
}

Graphics::GFXException::GFXException(int nLine, const char* pFile, HRESULT hr, std::vector<std::string> v_strMsgs) noexcept
	:
	GFXExceptionBase(nLine, pFile), hrError(hr) {
	strErrorMsg = GFXExceptionBase::getGfxErrorString(hrError);
	strWhatBuffer = "";
	strInfoMsgs = "";
	if (!v_strMsgs.empty()) {
		for (const std::string& m : v_strMsgs) {
			strInfoMsgs += m;
			strInfoMsgs.push_back('\n');
		}
		strInfoMsgs.pop_back();
	}
}

const char* Graphics::GFXException::getType() const noexcept {
	return "Graphics exception";
}

const std::string& Graphics::GFXException::getErrorMsg() const noexcept {
	return strErrorMsg;
}

const std::string& Graphics::GFXException::getInfoMsgs() const noexcept {
	return strInfoMsgs;
}

const char* Graphics::GFXException::what() const noexcept {
	std::ostringstream ossOut;
	ossOut << "File: " << getFile() << std::endl
		<< "Line: " << getLine() << std::endl
		<< "Type: " << getType() << std::endl
		<< "Desc: " << getErrorMsg() << std::endl;
	if (!strInfoMsgs.empty()) {
		ossOut << "Error Info: " << getInfoMsgs() << std::endl;
	}
	strWhatBuffer = ossOut.str();
	return strWhatBuffer.c_str();
}

Graphics::Graphics(HWND hwnd) {
	CONSTRUCTOR_CONFIRM(Graphics);
	DB_INFO_MANAGER_SETUP();
	HRESULT hr;
	CLM_EXCEPT_GFX_HR_INFO(CreateDXGIFactory(__uuidof(IDXGIFactory), &ptrFactory));

#if ENUMERATE_ADAPTERS
	UINT i = 0;
	while (true) {
		if (ptrFactory->EnumAdapters(i, &ptrAdapter) == DXGI_ERROR_NOT_FOUND) {
			break;
		}
		DXGI_ADAPTER_DESC desc{};
		ptrAdapter->GetDesc(&desc);
		OutputDebugStringW(desc.Description);
		OutputDebugStringW(L"\n");
		i++;
	}
#endif
	// Device 1 is the nVidia card
	CLM_EXCEPT_GFX_HR_INFO(ptrFactory->EnumAdapters(1, &ptrAdapter));
	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	//CLM_EXCEPT_GFX_HR_INFO(D3D11CreateDevice(ptrAdapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG, featureLevels,
	//	sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL), D3D11_SDK_VERSION, &ptrDevice, NULL, &ptrDeviceContext));
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc{};
	dxgiSwapChainDesc.BufferDesc.Width = 0;
	dxgiSwapChainDesc.BufferDesc.Height = 0;
	dxgiSwapChainDesc.BufferDesc.RefreshRate = { 60, 1 };
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainDesc.SampleDesc = { 1, 0 };
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = 2;
	dxgiSwapChainDesc.OutputWindow = hwnd;
	dxgiSwapChainDesc.Windowed = TRUE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.Flags = 0;
	CLM_EXCEPT_GFX_HR_INFO(D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_BIND_RENDER_TARGET,
		featureLevels,
		sizeof(featureLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&dxgiSwapChainDesc,
		&ptrSwapChain,
		&ptrDevice,
		nullptr,
		&ptrDeviceContext));

	// Create DXGI Device and get Direct2D device
	CLM_EXCEPT_GFX_HR_INFO(ptrDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&ptrDXGIDevice));
	D2D1_CREATION_PROPERTIES d2dCreationProp{ D2D1_THREADING_MODE_SINGLE_THREADED, D2D1_DEBUG_LEVEL_INFORMATION, D2D1_DEVICE_CONTEXT_OPTIONS_NONE };
	CLM_EXCEPT_GFX_HR_INFO(D2D1CreateDevice(ptrDXGIDevice.Get(), d2dCreationProp, &ptrD2DDevice));
	CLM_EXCEPT_GFX_HR_INFO(ptrD2DDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ptrD2DDeviceContext));
	mswrl::ComPtr<IDXGISurface> ptrDXGISurface;
	ptrSwapChain->GetBuffer(0, IID_PPV_ARGS(&ptrDXGISurface));
	const D2D1_BITMAP_PROPERTIES1 dbpBitmapProp{ {DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_IGNORE},
		0.f, 0.f,
		{D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW},
		NULL };
	CLM_EXCEPT_GFX_HR_INFO(ptrD2DDeviceContext->CreateBitmapFromDxgiSurface(ptrDXGISurface.Get(), dbpBitmapProp, &ptrBitmap));
	ptrD2DDeviceContext->SetTarget(ptrBitmap);
	
	// Create render target view for D3D11
	ID3D11Resource* pBackBuffer = nullptr;
	CLM_EXCEPT_GFX_HR_INFO(ptrSwapChain->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer)));
	ptrDevice->CreateRenderTargetView(pBackBuffer, nullptr, &ptrTarget);
	pBackBuffer->Release();

	//
	//// Compile vertex shader
	//std::wstring strShaderFileName = L"GFX/Graphics/Shaders/VSmain.hlsl";
	//std::string strShaderEntryPoint = "main";
	//CLM_EXCEPT_GFX_HR_INFO(D3DCompileFromFile(strShaderFileName.c_str(), NULL, NULL, strShaderEntryPoint.c_str(), "vs_5_0", D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &ptrVSBlob, &ptrVSErrorBlob));

	//// Compile pixel shader
	//strShaderFileName = L"GFX/Graphics/Shaders/PSmain.hlsl";
	//strShaderEntryPoint = "main";
	//CLM_EXCEPT_GFX_HR_INFO(D3DCompileFromFile(strShaderFileName.c_str(), NULL, NULL, strShaderEntryPoint.c_str(), "ps_5_0", D3DCOMPILE_WARNINGS_ARE_ERRORS, 0, &ptrPSBlob, &ptrPSErrorBlob));

	//// Create and load vertex shader
	//CLM_EXCEPT_GFX_HR_INFO(ptrDevice->CreateVertexShader(ptrVSBlob->GetBufferPointer(), ptrVSBlob->GetBufferSize(), NULL, &ptrVertexShader));
	//CLM_EXCEPT_GFX_INFO_ONLY(ptrDeviceContext->VSSetShader(ptrVertexShader.Get(), NULL, 0));

	//// Create and load pixel shader
	//CLM_EXCEPT_GFX_HR_INFO(ptrDevice->CreatePixelShader(ptrPSBlob->GetBufferPointer(), ptrPSBlob->GetBufferSize(), NULL, &ptrPixelShader));
	//CLM_EXCEPT_GFX_INFO_ONLY(ptrDeviceContext->PSSetShader(ptrPixelShader.Get(), NULL, 0));

}

Graphics::~Graphics() {
	DESTRUCTOR_CONFIRM(~Graphics);
	SafeRelease(ptrBitmap);
	SafeRelease(ptrD2DDeviceContext);
	SafeRelease(ptrD2DDevice);
}

void Graphics::ClearBuffer(const double fTime = 0.0f) {
	/*float fRed = 0.5 * (float)(1 + std::sin(2 * std::numbers::pi * 1.5 * fTime));
	float fGreen = 0.5 * (float)(1 + std::sin(2 * std::numbers::pi * 1.0 * fTime));
	float fBlue = 0.5 * (float)(1 + std::sin(2 * std::numbers::pi * 0.1 * fTime));*/
	const float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	ptrDeviceContext->ClearRenderTargetView(ptrTarget.Get(), color);
}

void Graphics::BeginFrame() {
	ClearBuffer();
	static HRESULT hr;

	ptrD2DDeviceContext->BeginDraw();

	return;
}
void Graphics::EndFrame() {
	static HRESULT hr;
	CLM_EXCEPT_GFX_HR_INFO(ptrD2DDeviceContext->EndDraw());
	CLM_EXCEPT_GFX_HR_INFO(ptrSwapChain->Present(1u, 0));
	return;
}


void Graphics::draw_circle(const float x, const float y, const float r) {
	ID2D1SolidColorBrush* ptrBrush = nullptr;
	ptrD2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &ptrBrush);

	D2D1_ELLIPSE circle = D2D1::Ellipse({ x, y }, r, r);

	ptrD2DDeviceContext->FillEllipse(circle, ptrBrush);

	ptrBrush->Release();
	return;
}

void Graphics::draw_circle(const GFX::Circle c) {
	ID2D1SolidColorBrush* ptrBrush = nullptr;
	ptrD2DDeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &ptrBrush);

	D2D1_ELLIPSE circle = D2D1::Ellipse({ c.x, c.y }, c.r, c.r);

	ptrD2DDeviceContext->FillEllipse(circle, ptrBrush);

	ptrBrush->Release();
	return;
}
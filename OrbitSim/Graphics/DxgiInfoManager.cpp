#include "DxgiInfoManager.h"
#include "../Windows/WinStuff.h"
#include "../DebugDefines/WinDebugDefines.h"

namespace clm {
	DxgiInfoManager::DxgiInfoManager() {
		typedef HRESULT(*DXGIGetDebugInterface)(REFIID, void**);
		nNext = 0;

		HMODULE hDxgiIMModule = LoadLibraryExW(L"dxgidebug.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (hDxgiIMModule == nullptr) {
			CLM_EXCEPT_LAST_NARG();
		}
		DXGIGetDebugInterface fptrGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hDxgiIMModule, "DXGIGetDebugInterface")));
		if (fptrGetDebugInterface == nullptr) {
			CLM_EXCEPT_LAST_NARG();
		}

		HRESULT hr = fptrGetDebugInterface(__uuidof(IDXGIInfoQueue), &ptrInfoQueue);
		CLM_EXCEPT_LAST(hr);
		hr = fptrGetDebugInterface(__uuidof(IDXGIDebug1), &ptrDebug);
		CLM_EXCEPT_LAST(hr);
	}

	void DxgiInfoManager::set() noexcept {
		nNext = ptrInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	}

	UINT64 DxgiInfoManager::check() const noexcept {
		return ptrInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	}

	std::vector<std::string> DxgiInfoManager::getMessages() const {
		std::vector<std::string> vstrReturn{};
		//HRESULT hr;
		size_t nEnd{ (size_t)ptrInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL) };
		SIZE_T nLength{ 0 };
		for (size_t i = nNext; i < nEnd; i++) {
			CLM_EXCEPT_LAST(ptrInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &nLength));
			std::unique_ptr<byte[]> ptrMsgData = std::make_unique<byte[]>(nLength);
			DXGI_INFO_QUEUE_MESSAGE* ptrMsg = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(ptrMsgData.get());
			CLM_EXCEPT_LAST(ptrInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, ptrMsg, &nLength));
			vstrReturn.emplace_back(ptrMsg->pDescription);
		}

		return vstrReturn;
	}
}
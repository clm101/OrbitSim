#include <windows.foundation.h>
#include "wrl\wrappers\corewrappers.h"
#include "wrl\client.h"
#include <dxgidebug.h>
#include <vector>
#include <string>
#include "../DebugDefines/MemoryDebug.h"

#ifndef DXGI_INFO_MANAGER_H
#define DXGI_INFO_MANAGER_H

class DxgiInfoManager {
private:
	size_t nNext;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> ptrInfoQueue;
	Microsoft::WRL::ComPtr<IDXGIDebug1> ptrDebug;
public:
	DxgiInfoManager();
	~DxgiInfoManager() = default;
	DxgiInfoManager(const DxgiInfoManager&) = delete;
	DxgiInfoManager& operator=(const DxgiInfoManager&) = delete;

	void set() noexcept; // Set nNext to keep track of position in info queue
	UINT64 check() const noexcept;
	std::vector<std::string> getMessages() const;	// Get messages from the info queue and return them in a vector
};

#endif
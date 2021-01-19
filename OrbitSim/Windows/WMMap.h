#ifndef WMMAP_H
#define WMMAP_H

#include "../DebugDefines/MemoryDebug.h"
#include "WindowsInclude.h"
#include <string>
#include <unordered_map>

#define REGISTER_MESSAGE(msg) {msg, L#msg}

class WindowsMessages {
private:
	std::unordered_map<DWORD, std::wstring> umMessages;
public:
	WindowsMessages() noexcept;
	~WindowsMessages() noexcept;

	std::wstring operator()(DWORD, WPARAM, LPARAM) const noexcept;
};

#endif
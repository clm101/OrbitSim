#include "BaseDebugDefines.h"

#ifndef GRAPHICS_DEBUG_DEFINES_H
#define GRPAHICS_DEBUG_DEFINES_H

#ifdef _DEBUG

#define DB_INFO_MANAGER_DECL() std::unique_ptr<DxgiInfoManager> ptrInfoManager;
#define DB_INFO_MANAGER_SETUP() ptrInfoManager = std::make_unique<DxgiInfoManager>();

// Requires HRESULT hr be declared
#define CLM_EXCEPT_GFX_HR(hrcall) if((hr = (hrcall)) < 0) {throw Graphics::GFXException(__LINE__, __FILE__, hr);}
#define CLM_EXCEPT_GFX_HR_INFO(hrcall) ptrInfoManager->set(); if((hr = (hrcall)) < 0) {throw Graphics::GFXException(__LINE__, __FILE__, hr, ptrInfoManager->getMessages());}
#define CLM_EXCEPT_GFX_INFO_ONLY(call) ptrInfoManager->set(); call; if(ptrInfoManager->check() > 0) {throw Graphics::GFXException(__LINE__, __FILE__, S_OK, ptrInfoManager->getMessages());}

#else
#define DB_INFO_MANAGER_DECL()
#define DB_INFO_MANAGER_SETUP()
#define CLM_EXCEPT_GFX_HR(hr)
#define CLM_EXCEPT_GFX_HR_INFO(hrcall) hrcall;
#define CLM_EXCEPT_GFX_INFO_ONLY(call) call;
#endif

#endif
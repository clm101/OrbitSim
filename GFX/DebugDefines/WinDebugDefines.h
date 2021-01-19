#include "BaseDebugDefines.h"

#ifndef WIN_DEBUG_DEFINES_H
#define WIN_DEBUG_DEFINES_H

/**********************************/
/***	PREPROCESSOR MACROS		***/
/**********************************/

#define CLM_EXCEPT_HR(hr) if(hr < 0) {throw Window::HRException(__LINE__, __FILE__, hr);}
#define CLM_EXCEPT_LAST(ret) if(ret < 0) {throw Window::HRException(__LINE__, __FILE__, GetLastError());}
#define CLM_EXCEPT_LAST_NARG() throw Window::HRException(__LINE__, __FILE__, GetLastError())

#ifdef _DEBUG
#define CLM_EXCEPT_DEBUG(ret, strOut) if(ret == 0){OutputDebugStringW(strOut);}
#else
#define CLM_EXCEPT_DEBUG(ret, strOut)
#endif

#if PRINT_MESSAGES
#define MESSAGE_PRINT(var) OutputDebugStringW(var)
#else
#define MESSAGE_PRINT(var)
#endif

#endif
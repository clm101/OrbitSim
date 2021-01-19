#ifndef BASE_DEBUG_DEFINES_H
#define BASE_DEBUG_DEFINES_H

/**************************/
/***	DEBUG ONLY		***/
/**************************/
#ifdef _DEBUG

#define CONFIRM_CONSTRUCTOR 1
#define CONFIRM_DESTRUCTOR 1
#define PRINT_MESSAGES 0
#define ENUMERATE_ADAPTERS 1

#endif

#define PRINT_FUNC_CALL(func)\
	OutputDebugStringW(L#func);\
	OutputDebugStringW(L"() called\n");

#if CONFIRM_CONSTRUCTOR
#define CONSTRUCTOR_CONFIRM(name) PRINT_FUNC_CALL(name)
#else
#define CONSTRUCTOR_CONFIRM(name)
#endif

#if CONFIRM_DESTRUCTOR
#define DESTRUCTOR_CONFIRM(name) PRINT_FUNC_CALL(name)
#else
#define DESTRUCTOR_CONFIRM(name)
#endif

#endif
////////////////////////////////////////////////////////////////////////////
//	Module 		: os_extensions.h
//	Created 	: 24.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : operating system extensions
////////////////////////////////////////////////////////////////////////////

#ifndef OS_EXTENSIONS_H_INCLUDED
#define OS_EXTENSIONS_H_INCLUDED

#define LPVOID						pvoid
#define HANDLE						pvoid
#define INVALID_HANDLE_VALUE		((HANDLE)((u64)(-1)))

#define BOOL						int
#define FALSE						0
#define TRUE						1

#define DLL_PROCESS_DETACH			0
#define DLL_PROCESS_ATTACH			1

#define APIENTRY					__stdcall

#define HMODULE						pvoid

#include <cs/core/os_functions.h>

#endif // #ifndef OS_EXTENSIONS_H_INCLUDED
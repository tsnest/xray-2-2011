////////////////////////////////////////////////////////////////////////////
//	Module 		: extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_EXTENSIONS_H_INCLUDED
#define CS_CORE_EXTENSIONS_H_INCLUDED

#ifdef SN_TARGET_PS3
	#include <assert.h>
	namespace std {
		void _SCE_Assert(const char *first, const char *second);
		void _Assert(const char *first, const char *second);
	} // namespace std
#endif // #ifdef SN_TARGET_PS3

#include <cs/config.h>
#include <cs/core/memory.h>
#include <cs/core/macro_extensions.h>
#include <cs/core/optimization_extensions.h>
#include <cs/core/type_extensions.h>
#include <cs/core/log_extensions.h>
#include <cs/core/debug_extensions.h>
#include <cs/core/stdlib_extensions.h>
#include <cs/core/memory_extensions.h>
#include <cs/core/std_extensions.h>
#include <cs/core/os_extensions.h>
#include <cs/core/threading_extensions.h>
#include <cs/core/platform_extensions.h>
#include <cs/core/timing_extensions.h>
#include <cs/core/string_extensions.h>
#include <cs/core/math_extensions.h>
#include <cs/core/fs_extensions.h>
#include <cs/core/build_extensions.h>
#include <cs/core/core_extensions.h>

#endif // #ifndef CS_CORE_EXTENSIONS_H_INCLUDED
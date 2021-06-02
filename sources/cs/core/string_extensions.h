////////////////////////////////////////////////////////////////////////////
//	Module 		: string_extensions.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : string extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_STRING_EXTENSIONS_H_INCLUDED
#define CS_CORE_STRING_EXTENSIONS_H_INCLUDED

#include <string>

class shared_string;
typedef shared_string str_shared;

inline	u32		sz_len			(pcstr string);
inline	pstr	sz_dup			(pcstr string);

namespace string {
	extern void initialize		();
	extern void uninitialize	();
}

#include <cs/core/shared_string_manager.h>
#include <cs/core/shared_string.h>
#include <cs/core/string_functions.h>

#endif // #ifndef CS_CORE_STRING_EXTENSIONS_H_INCLUDED
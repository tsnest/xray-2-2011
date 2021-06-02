////////////////////////////////////////////////////////////////////////////
//	Module 		: core_extensions.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : core extensions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_CORE_EXTENSIONS_H_INCLUDED
#define CS_CORE_CORE_EXTENSIONS_H_INCLUDED

#include "core.h"

inline u32 color_rgba	(u32 const r, u32 const g, u32 const b, u32 const a)
{
	return		((a&  0xff) << 24) | ((r&  0xff) << 16) | ((g&  0xff) << 8) | (b&  0xff);
}

inline u32 color_xrgb	(u32 const r, u32 const g, u32 const b)
{
	return		color_rgba(r,g,b,0xff);
}

struct lua_State;

namespace cs { namespace core {
	extern CS_CORE_API void script_export(lua_State* state);
}}

#endif // #ifndef CS_CORE_CORE_EXTENSIONS_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created 	: 20.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_INPUT_API_H_INCLUDED
#define XRAY_INPUT_API_H_INCLUDED

#ifndef XRAY_INPUT_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_INPUT_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_INPUT_BUILDING
#			define XRAY_INPUT_API			XRAY_DLL_EXPORT
#		else // #ifdef XRAY_INPUT_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_INPUT_API		XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_INPUT_API		XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_INPUT_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_INPUT_API

#ifndef _MSC_VER
	typedef pvoid HWND;
#endif // #ifndef _MSC_VER

namespace xray {
namespace input {

struct engine;
struct world;

typedef memory::doug_lea_allocator_type		allocator_type;

XRAY_INPUT_API	world*	create_world		( engine& engine, HWND window_handle );
XRAY_INPUT_API	void	destroy_world		( world*& world );
XRAY_INPUT_API	void	set_memory_allocator( allocator_type& allocator );

} // namespace input
} // namespace xray

#endif // #ifndef XRAY_INPUT_API_H_INCLUDED
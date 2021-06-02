////////////////////////////////////////////////////////////////////////////
//	Created 	: 31.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_EDITOR_API_H_INCLUDED
#define XRAY_EDITOR_API_H_INCLUDED

#include <xray/memory_pthreads3_allocator.h>
#include <xray/memory_allocator_helper.h>

#ifndef XRAY_EDITOR_API

#	ifndef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_EDITOR_BUILDING
#			define XRAY_EDITOR_API	XRAY_DLL_EXPORT
#		else // #ifdef XRAY_EDITOR_BUILDING
#			define XRAY_EDITOR_API	XRAY_DLL_IMPORT
#		endif // #ifdef XRAY_EDITOR_BUILDING
#	else // #ifndef XRAY_STATIC_LIBRARIES
#		define XRAY_EDITOR_API
#	endif // #ifndef XRAY_STATIC_LIBRARIES

#	define XRAY_EDITOR_API_C		extern "C" XRAY_EDITOR_API
#endif // #ifndef XRAY_EDITOR_API

namespace xray {
namespace editor {

	struct engine;
	struct world;

	typedef memory::doug_lea_allocator				allocator_type;

	typedef world* ( *create_world_ptr )			( engine& engine );
	XRAY_EDITOR_API_C	world*	create_world		( engine& engine );

	typedef void ( *destroy_world_ptr )				( world*& world );
	XRAY_EDITOR_API_C	void	destroy_world		( world*& world );

	typedef void ( *memory_allocator_ptr )			( allocator_type& allocator );
	XRAY_EDITOR_API_C	void	set_memory_allocator( allocator_type& allocator );

} // namespace editor
} // namespace xray

#endif // #ifndef XRAY_EDITOR_API_H_INCLUDED
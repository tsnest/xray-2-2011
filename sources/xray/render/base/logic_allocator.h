////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_LOGIC_ALLOCATOR_H_INCLUDED
#define XRAY_RENDER_BASE_LOGIC_ALLOCATOR_H_INCLUDED

#include <xray/render/base/api.h>

namespace xray {
namespace render {
namespace logic {

XRAY_RENDER_BASE_API extern logic_allocator_type*	g_allocator;

#define USER_ALLOCATOR							*::xray::render::logic::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace logic
} // namespace render
} // namespace xray

#define L_NEW( type )							XRAY_NEW_IMPL(		*::xray::render::logic::g_allocator, type )
#define L_DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::render::logic::g_allocator, pointer )
#define L_MALLOC( size, description )			XRAY_MALLOC_IMPL(	*::xray::render::logic::g_allocator, size, description )
#define L_REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::render::logic::g_allocator, pointer, size, description )
#define L_FREE( pointer )						XRAY_FREE_IMPL(		*::xray::render::logic::g_allocator, pointer )
#define L_ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::render::logic::g_allocator, type, count )

#endif // #ifndef XRAY_RENDER_BASE_LOGIC_ALLOCATOR_H_INCLUDED
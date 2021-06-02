////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_RENDER_ALLOCATOR_H_INCLUDED
#define XRAY_RENDER_BASE_RENDER_ALLOCATOR_H_INCLUDED

#include <xray/render/base/api.h>

namespace xray {
namespace render {

XRAY_RENDER_BASE_API extern render_allocator_type*	g_allocator;

#define USER_ALLOCATOR							*::xray::render::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace render
} // namespace xray

#define NEW( type )								XRAY_NEW_IMPL(		*::xray::render::g_allocator, type )
#define NEW_ARRAY(type, count)					XRAY_NEW_ARRAY_IMPL(*::xray::render::g_allocator, type, count)
#define DELETE_ARRAY(pointer)					XRAY_DELETE_ARRAY_IMPL(*::xray::render::g_allocator, pointer )
#define DELETE( pointer, ... )					XRAY_DELETE_IMPL(	*::xray::render::g_allocator, pointer, __VA_ARGS__ )
#define DELETET( pointer )						XRAY_DELETE_IMPL(	*::xray::render::g_allocator, pointer )
#define MALLOC( size, description )				XRAY_MALLOC_IMPL(	*::xray::render::g_allocator, size, description )
#define REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::render::g_allocator, pointer, size, description )
#define FREE( pointer )							XRAY_FREE_IMPL(		*::xray::render::g_allocator, pointer )
#define ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::render::g_allocator, type, count )

#endif // #ifndef XRAY_RENDER_BASE_RENDER_ALLOCATOR_H_INCLUDED
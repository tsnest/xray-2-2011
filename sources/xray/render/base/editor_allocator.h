////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_EDITOR_ALLOCATOR_H_INCLUDED
#define XRAY_RENDER_BASE_EDITOR_ALLOCATOR_H_INCLUDED

#include <xray/render/base/api.h>

namespace xray {
namespace render {
namespace editor {

XRAY_RENDER_BASE_API extern editor_allocator_type*				g_allocator;

#define USER_ALLOCATOR							*::xray::render::editor::g_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

} // namespace editor
} // namespace render
} // namespace xray

#define E_NEW( type )							XRAY_NEW_IMPL(		*::xray::render::editor::g_allocator, type )
#define E_DELETE( pointer )						XRAY_DELETE_IMPL(	*::xray::render::editor::g_allocator, pointer )
#define E_MALLOC( size, description )			XRAY_MALLOC_IMPL(	*::xray::render::editor::g_allocator, size, description )
#define E_REALLOC( pointer, size, description )	XRAY_REALLOC_IMPL(	*::xray::render::editor::g_allocator, pointer, size, description )
#define E_FREE( pointer )						XRAY_FREE_IMPL(		*::xray::render::editor::g_allocator, pointer )
#define E_ALLOC( type, count )					XRAY_ALLOC_IMPL(	*::xray::render::editor::g_allocator, type, count )

#endif // #ifndef XRAY_RENDER_BASE_EDITOR_ALLOCATOR_H_INCLUDED
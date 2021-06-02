////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_creator.h"
#include "model_editor.h"

namespace xray {
namespace model_editor {

xray::editor_base::tool_window^ editor_creator::create_editor( render::world& render_world,
															  xray::editor_base::tool_window_holder^ holder,
															  pcstr resources_path )
{
	model_editor^ ed	= gcnew model_editor( render_world, holder, resources_path );
	return				ed;
}

void editor_creator::set_memory_allocator(xray::memory::doug_lea_allocator_type* allocator)
{
	ASSERT			( !g_allocator || g_allocator==allocator );
	g_allocator		= allocator;
}

} //namespace model_editor
} //namespace xray


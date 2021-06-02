////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_creator.h"
#include "particle_editor.h"
#include <xray/render/world.h>

namespace xray {
namespace particle_editor {

xray::editor_base::tool_window^ editor_creator::create_editor( System::String^ resources_path, 
																render::world& render_world, 
																xray::ui::world& ui_world,
																xray::editor_base::tool_window_holder^ h )
{
	particle_editor^ ed				= gcnew particle_editor(resources_path, render_world, ui_world, h);
	return			ed;
}

void editor_creator::set_memory_allocator(xray::particle_editor::allocator_type* allocator)
{
	g_allocator = allocator;
}

} //namespace particle_editor
} //namespace xray
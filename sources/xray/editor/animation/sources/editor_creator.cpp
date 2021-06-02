////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_creator.h"
#include "animation_editor.h"

namespace xray {
namespace animation_editor {

xray::editor_base::tool_window^ editor_creator::create_editor( System::String^ resources_path, 
															  render::world& render_world,
															  ui::world& ui_wrld,
															  xray::editor_base::tool_window_holder^ h )
{
	animation_editor^ ed	= gcnew animation_editor(resources_path, render_world, ui_wrld, h);
	return					ed;
}

void editor_creator::set_memory_allocator(xray::animation_editor::allocator_type* allocator)
{
	g_allocator = allocator;
}

} //namespace animation_editor
} //namespace xray


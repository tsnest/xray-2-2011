////////////////////////////////////////////////////////////////////////////
//	Created		: 10.06.2010
//	Author		: Sergey Pryshchepa
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "editor_creator.h"
#include "sound_editor.h"

namespace xray {
namespace sound_editor {
xray::editor_base::tool_window^ editor_creator::create_editor(System::String^ resources_path, 
															  sound::world& sound_world,
															  render::world& render_world,
															  xray::editor_base::tool_window_holder^ h )
{
	sound_editor^ ed	= gcnew sound_editor(resources_path, sound_world, render_world, h );
	return				ed;
}

void editor_creator::set_memory_allocator(xray::sound_editor::allocator_type* allocator)
{
	g_allocator = allocator;
}
}// namespace sound_editor
}// namespace xray

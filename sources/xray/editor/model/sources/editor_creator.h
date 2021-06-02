////////////////////////////////////////////////////////////////////////////
//	Created		: 21.01.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_CREATOR_H_INCLUDED
#define EDITOR_CREATOR_H_INCLUDED

namespace xray {

namespace render{ class world; }

namespace model_editor {

public ref class editor_creator
{
public:
	static xray::editor_base::tool_window^	create_editor			( render::world& render_world, xray::editor_base::tool_window_holder^ h, pcstr resources_path );
	static void								set_memory_allocator	( memory::doug_lea_allocator_type* allocator );
}; // ref class editor_creator

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDITOR_CREATOR_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_smooth.h"
#include "terrain_object.h"
#include "tool_terrain.h"
#include "level_editor.h"
#include "terrain_modifier_command.h"

namespace xray {
namespace editor {


terrain_modifier_smooth::terrain_modifier_smooth(level_editor^ le, tool_terrain^ tool)
:super	( le, tool)
{
	m_control_id		= "terrain_modifier_smooth";
	m_min_power			= 0.01f;
	m_max_power			= 1.0f;
}

terrain_modifier_smooth::~terrain_modifier_smooth( )
{
}

void terrain_modifier_smooth::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	refresh_active_working_set	( shape_type );

	m_last_screen_xy.X			= -10000;// force first update
	m_last_screen_xy.Y			= -10000;
	init_smooth					( );

	super::start_input			( mode, button, coll, pos );
}


void terrain_modifier_smooth::execute_input( )
{
	System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();
	if(screen_xy != m_last_screen_xy)
		refresh_active_working_set	(shape_type);

	m_last_screen_xy				= screen_xy;

	m_level_editor->get_command_engine()->preview();
}

void terrain_modifier_smooth::do_work( )
{
	do_smooth		( );
}


} // namespace editor
} // namespace xray

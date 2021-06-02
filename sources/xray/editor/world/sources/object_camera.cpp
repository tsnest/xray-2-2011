////////////////////////////////////////////////////////////////////////////
//	Created		: 24.04.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_camera.h"
#include "tool_misc.h"
#include "project_items.h"

namespace xray{
namespace editor{

object_camera::object_camera( tool_misc^ tool )
:super			( tool )
{
	image_index	= xray::editor_base::misc_tree_icon;
}

void object_camera::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "camera";
//	t["registry_name"]		= unmanaged_string( m_project_item->get_full_name()).c_str();
}



object_timer::object_timer( tool_misc^ tool )
:super(tool)
{}

void object_timer::save( configs::lua_config_value t )
{
	super::save				( t );
	t["game_object_type"]	= "timer";
//	t["registry_name"]		= unmanaged_string( m_project_item->get_full_name()).c_str();
}

} // namespace editor
} // namespace xray




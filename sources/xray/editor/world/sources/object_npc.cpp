////////////////////////////////////////////////////////////////////////////
//	Created		: 26.11.2010
//	Author		: Plichko Alexander
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_npc.h"
#include "object_scene.h"
#include "tool_scripting.h"
#include "project_items.h"
#include "level_editor.h"
#include "logic_editor_panel.h"
#include "editor_world.h"

#pragma managed(push,off)
#include <xray/render/base/debug_renderer.h>
#pragma managed(pop)

namespace xray{
namespace editor{

object_npc::object_npc( tool_scripting^ t )
	:super(t)
{
	image_index		= xray::editor_base::npc_tree_icon;
}

void object_npc::load_props( configs::lua_config_value const& t )
{
	super::load_props		(t);	
}


void object_npc::save( configs::lua_config_value t )
{
	super::save				( t );
}

void object_npc::render( )
{
	super::render	();

 	if( m_project_item->get_selected() )
 	{
		color clr(0u,0u,255u,255u);
		float tmp = 0.5f;
		float3 position = m_transform->c.xyz();
		position.y += 1;
		get_debug_renderer().draw_sphere( get_editor_world().scene(), position, tmp, clr);
	}

	get_debug_renderer().draw_cross( get_editor_world().scene(), m_transform->c.xyz(), 1.f, math::color(0xffffff00) );
}

void object_npc::setup_job( object_job^ job )
{
	m_job = job;
}


void object_npc::on_selected				( bool bselected )
{
	super::on_selected( bselected );

// 	logic_editor_panel^ logic_panel = m_tool_scripting->get_level_editor()->get_logic_editor_panel( );
// 
// 	this->tree_node->Selected				= bselected;
// 
// 	if ( logic_panel == nullptr )
// 		return;
// 
// 	if ( bselected )
// 	{
// 		logic_panel->show_object_logic( m_parent_scene );
// 	}
// 	else
// 	{
// 		logic_panel->show_object_logic( nullptr );
// 	}
}

}
}



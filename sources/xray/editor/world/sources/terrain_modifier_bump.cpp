////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_bump.h"
#include "terrain_object.h"
#include "level_editor.h"
#include "tool_terrain.h"
#include "terrain_modifier_command.h"
#include "window_ide.h"

namespace xray {
namespace editor {

terrain_modifier_bump::terrain_modifier_bump(xray::editor::level_editor^ le, tool_terrain^ tool)
:super(le, tool)
{
	m_control_id			= "terrain_modifier_bump";
	grab_mode				= false;
	m_timer					= NEW( timing::timer );
	m_min_power				= -20.0f;
	m_max_power				= 20.0f;
}

terrain_modifier_bump::~terrain_modifier_bump( )
{
	DELETE					( m_timer );
}

void terrain_modifier_bump::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	super::start_input		( mode, button, coll, pos );

	m_last_screen_xy		= m_level_editor->view_window()->get_mouse_position();

	if(!grab_mode)
		m_timer->start		();
	
	refresh_active_working_set( shape_type );
}

void terrain_modifier_bump::execute_input( )
{
	if(!grab_mode)
	{
		System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();

		if(screen_xy != m_last_screen_xy)
			refresh_active_working_set	(shape_type);

		m_last_screen_xy			= screen_xy;
	}

	m_level_editor->get_command_engine()->preview();
}

void terrain_modifier_bump::do_work( )
{
	if(m_smooth_mode)
	{
		do_smooth	();
		return;
	}

	float global_influence			= power;
	if(!grab_mode)
	{
		float const time			= m_timer->get_elapsed_sec();

		global_influence			*= time; // strength = value per 1 second
		
		
		if(m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ControlKey))
			global_influence		*= -1.0f;

		m_timer->start				();
	}else
	{
		System::Drawing::Point screen_xy		= m_level_editor->view_window()->get_mouse_position();
		if(screen_xy.Y != m_last_screen_xy.Y)
		{
			global_influence		*= (m_last_screen_xy.Y - screen_xy.Y)/100.0f ; // strength = value per 100 pixels

			m_last_screen_xy.Y		= screen_xy.Y;
		}else
			return;
	}
	float4x4						inv_transform;

	for each (terrain_node_key key in m_active_id_list.Keys)
	{

		height_diff_dict^ curr_processed_values_list	= m_processed_values_list[key];
		vert_id_list^ curr_processed_id_list			= m_processed_id_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];

		inv_transform.try_invert			(curr_terrain->get_transform());
		float3 center_position_local		= inv_transform.transform_position(*m_center_position_global);

		for each (u16 vert_idx in m_active_id_list[key])
		{
			terrain_vertex v				= curr_terrain->m_vertices.get(vert_idx);
			float const distance			= curr_terrain->distance_xz(vert_idx, center_position_local);

			float influence					= calc_influence_factor(distance);
			influence						*= global_influence;

			if(!curr_processed_id_list->Contains(vert_idx))
			{
				curr_processed_id_list->Add				( vert_idx );
				curr_processed_values_list[vert_idx]	= influence;
			}else
				curr_processed_values_list[vert_idx]	+= influence;

			v.height							+= influence;

			curr_terrain->m_vertices.set(v, vert_idx);
		}

		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key]);
	}
}

void terrain_modifier_bump::load_settings(RegistryKey^ key)
{
	super::load_settings		(key);

	RegistryKey^ self_key		= get_sub_key(key, name);

	grab_mode					= System::Convert::ToBoolean(self_key->GetValue("grab_mode", false));

	self_key->Close				();
}

void terrain_modifier_bump::save_settings(RegistryKey^ key)
{
	super::save_settings		(key);

	RegistryKey^ self_key		= get_sub_key(key, name);

	self_key->SetValue			("grab_mode", grab_mode);

	self_key->Close				();
}

} // namespace editor
} // namespace xray

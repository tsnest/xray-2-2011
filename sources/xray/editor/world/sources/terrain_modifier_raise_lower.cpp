////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_raise_lower.h"
#include "terrain_object.h"
#include "level_editor.h"
#include "tool_terrain.h"
#include "terrain_modifier_command.h"
#include "window_ide.h"

namespace xray {
namespace editor {

terrain_modifier_raise_lower::terrain_modifier_raise_lower(level_editor^ le, tool_terrain^ tool)
:super(le, tool)
{
	m_control_id		= "terrain_modifier_raise_lower";
	m_min_power			= -20.0f;
	m_max_power			= 20.0f;
}

terrain_modifier_raise_lower::~terrain_modifier_raise_lower()
{}

void terrain_modifier_raise_lower::execute_input( )
{
	System::Drawing::Point screen_xy	= m_level_editor->view_window()->get_mouse_position();
	if(screen_xy==m_last_screen_xy)
		return;

	m_last_screen_xy = screen_xy;
	
	m_level_editor->get_command_engine()->preview();
}

void terrain_modifier_raise_lower::do_work( )
{
	// do work
	refresh_active_working_set			(shape_type);

	if(m_smooth_mode)
	{
		do_smooth	();
		return;
	}

	float global_influence				= 1.0f;

	if(m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ControlKey))
			global_influence			*= -1.0f;

	float4x4							inv_transform;

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

			ASSERT							(distance<radius+math::epsilon_3);
			float vertex_influence			= calc_influence_factor(distance) * power;
			vertex_influence				*= global_influence;


			if(!curr_processed_id_list->Contains(vert_idx))
			{
				curr_processed_id_list->Add			(vert_idx);
				curr_processed_values_list[vert_idx] = vertex_influence;
				v.height							+= vertex_influence;
				curr_terrain->m_vertices.set( v, vert_idx );
			}else
			{
				float val = curr_processed_values_list[vert_idx];
				if(math::abs(val) < math::abs(vertex_influence) )
				{
					curr_processed_values_list[vert_idx]= vertex_influence;
					v.height						-= val;
					v.height						+= vertex_influence;
					curr_terrain->m_vertices.set	( v, vert_idx );
				}
			}
		} // verts
		
		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key]);
	} // keys

	// create update-command from vertex set m_active_list
}

} // namespace editor
} // namespace xray

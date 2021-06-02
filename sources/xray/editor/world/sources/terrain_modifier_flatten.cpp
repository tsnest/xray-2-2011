////////////////////////////////////////////////////////////////////////////
//	Created		: 26.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_flatten.h"
#include "terrain_object.h"
#include "level_editor.h"
#include "tool_terrain.h"
#include "terrain_modifier_command.h"

namespace xray {
namespace editor {

terrain_modifier_flatten::terrain_modifier_flatten( level_editor^ le, tool_terrain^ tool )
:super(le, tool)
{
	m_control_id		= "terrain_modifier_flatten";
	m_min_power			= -150.0f;
	m_max_power			= 150.0f;
	m_timer				= NEW( timing::timer );

}

terrain_modifier_flatten::~terrain_modifier_flatten( )
{
	DELETE					( m_timer );
}

void terrain_modifier_flatten::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	m_timer->start			( );
	super::start_input		( mode, button, coll, pos );
}

void terrain_modifier_flatten::execute_input()
{
	System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();
	
	if(screen_xy != m_last_screen_xy)
		refresh_active_working_set		( shape_type );

	m_last_screen_xy					= screen_xy;
	
	m_level_editor->get_command_engine()->preview();
}

void terrain_modifier_flatten::do_work( )
{
	if( m_smooth_mode )
	{
		do_smooth	( );
		return;
	}

	float const time		= m_timer->get_elapsed_sec();
	m_timer->start			( );


	float4x4 inv_transform;

	for each( terrain_node_key key in m_active_id_list.Keys )
	{
		height_diff_dict^ curr_processed_values_list	= m_processed_values_list[key];
		height_diff_dict^ curr_stored_values_list		= m_stored_values_list[key];
		vert_id_list^ curr_processed_id_list			= m_processed_id_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];

		inv_transform.try_invert			(curr_terrain->get_transform());
		float3 center_position_local		= inv_transform.transform_position(*m_center_position_global);

		for each( u16 vert_idx in m_active_id_list[key] )
		{
			terrain_vertex v				= curr_terrain->m_vertices.get(vert_idx);
			
			// curr_processed_values_list[vert_idx] - accumulated influence (0..1)
			if(	!curr_processed_values_list->ContainsKey(vert_idx) )
			{
				curr_processed_id_list->Add				( vert_idx );
				curr_processed_values_list[vert_idx]	= 0.0f; 
			}

			float const distance				= curr_terrain->distance_xz(vert_idx, center_position_local);

			ASSERT								( distance<radius+math::epsilon_3 );

			float const original_height			= curr_stored_values_list[vert_idx];

			float const vertex_influence		= calc_influence_factor(distance) * time;

			float value = curr_processed_values_list[vert_idx] + vertex_influence;
			math::clamp							( value, 0.0f, 1.0f );
			curr_processed_values_list[vert_idx] = value;

			v.height							= original_height + value * ( power - original_height);
			curr_terrain->m_vertices.set		( v, vert_idx );
		}// verts
		
		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key]);
	} // keys
}

void terrain_modifier_flatten::change_property( System::String^ prop_name, float const dx, float const dy )
{
	if(prop_name=="dropper")
	{
		math::float3			pick_position;
		collision::object const* picked_collision = NULL;
	
		if(m_level_editor->view_window()->ray_query( acceptable_collision_type(), &picked_collision, &pick_position ) )
			power	= pick_position.y;
	}

	super::change_property(prop_name, dx, dy);
}

} // namespace editor
} // namespace xray

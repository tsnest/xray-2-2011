////////////////////////////////////////////////////////////////////////////
//	Created		: 23.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_uv_relax.h"
#include "terrain_object.h"
#include "tool_terrain.h"
#include "level_editor.h"
#include "terrain_modifier_command.h"
#include "window_ide.h"

namespace xray {
namespace editor {

terrain_modifier_uv_relax::terrain_modifier_uv_relax( level_editor^ le, tool_terrain^ tool )
:super(le, tool)
{
	m_control_id		= "terrain_modifier_uv_realx";
	m_min_power			= 0.01f;
	m_max_power			= 0.9f;
}

terrain_modifier_uv_relax::~terrain_modifier_uv_relax( )
{
}

void terrain_modifier_uv_relax::on_working_set_changed( )
{
	super::on_working_set_changed();

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		if( !m_pull_vectors.ContainsKey(key))
			m_pull_vectors.Add( key, gcnew pull_vector_dict);
	}
}

void terrain_modifier_uv_relax::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	refresh_active_working_set	( shape_type );

	m_last_screen_xy			= m_level_editor->view_window()->get_mouse_position();

	m_timer->start				();
	m_last_screen_xy.X			= -10000;// force first update
	m_last_screen_xy.Y			= -10000;

	super::start_input			( mode, button, coll, pos );
}


void terrain_modifier_uv_relax::execute_input( )
{
	System::Drawing::Point screen_xy = m_level_editor->view_window()->get_mouse_position();
	if(screen_xy != m_last_screen_xy)
		refresh_active_working_set	( shape_type );

	m_last_screen_xy				= screen_xy;

	m_level_editor->get_command_engine()->preview();
}

void terrain_modifier_uv_relax::end_input( )
{
	super::end_input				();
	m_pull_vectors.Clear			();
}

void terrain_modifier_uv_relax::do_work( )
{
	if(!m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ControlKey))
	{
		// prev ver
		do_work1();
		return;
	}

	float const time			= m_timer->get_elapsed_sec();

	if(time<0.01f)
		return;

	m_timer->start				();

	float4x4					inv_transform;

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		ASSERT(m_stored_values_list.ContainsKey(key));
		ASSERT(m_pull_vectors.ContainsKey(key));

		pull_vector_dict^ curr_processed_values_list	= m_pull_vectors[key];
		vert_id_list^	curr_processed_id_list			= m_processed_id_list[key];
		vertex_dict^	curr_stored_values_list			= m_stored_values_list[key];
		terrain_node^	curr_terrain					= m_tool_terrain->get_terrain_core()->m_nodes[key];

		inv_transform.try_invert			(curr_terrain->get_transform());
		float3 center_position_local		= inv_transform.transform_position(*m_center_position_global);

		for each (u16 curr_vert_idx in m_active_id_list[key])
		{
			int curr_idx_x, curr_idx_z;	
			curr_terrain->vertex_xz( curr_vert_idx, curr_idx_x, curr_idx_z );

			terrain_vertex curr_vertex	= curr_terrain->m_vertices.get(curr_vert_idx);
			math::float3 vert_pos		= curr_terrain->position_g( curr_vert_idx );
			math::float2 vert_uv_pos	= math::float2(vert_pos.x, vert_pos.z);
			vert_uv_pos.x				+= curr_vertex.tc_shift_x;
			vert_uv_pos.y				+= curr_vertex.tc_shift_z;

			//float const distance		= curr_terrain->distance_xz(curr_vert_idx, center_position_local);

			//ASSERT						( distance<radius+math::epsilon_3 );
			//float influence				= calc_influence_factor(distance);
			//math::clamp					(influence, 0.0f, 1.0f);

			pull_vector	pull_vector;
			pull_vector.x		= 0.0f;
			pull_vector.z		= 0.0f;

			const int quad_half_size = 1;
			math::float2 neighbours_weight(0,0);
			float neighbours_count = 0.0f;

			for( int ix = -quad_half_size; ix <= quad_half_size; ++ix )
			{
				for( int iz = -quad_half_size; iz <= quad_half_size; ++iz)
				{
					if( ix==0 && iz==0)
						continue;

					terrain_vertex^ neighbour_vert;
					math::float3 neighbour_vert_pos;


					terrain_node^ neighbour_node	= nullptr;
					u16 neighbour_vert_idx			= 0;
					get_neighbour_result res = get_neighbour(	curr_terrain,
																	curr_vert_idx, 
																	ix, 
																	iz,
																	neighbour_node,
																	neighbour_vert_idx );

					if(get_neighbour_result::missing == res)
						continue;
					
					neighbours_count			+= 1.0f;
					neighbour_vert				= neighbour_node->m_vertices.get(neighbour_vert_idx);
					neighbour_vert_pos			= neighbour_node->position_g(neighbour_vert_idx);
					math::float2 neighb_uv_pos	= math::float2(neighbour_vert_pos.x, neighbour_vert_pos.z);
					neighb_uv_pos.x				+= neighbour_vert->tc_shift_x;
					neighb_uv_pos.y				+= neighbour_vert->tc_shift_z;


					float distance_norm		= (neighbour_vert_pos-vert_pos).length();
					math::float2 uv_dir		= vert_uv_pos-neighb_uv_pos;// direction to neighbour
					float distance_uv		= uv_dir.length();

					float uv_diff			= distance_norm - distance_uv;

					R_ASSERT(math::valid(uv_diff));
					uv_diff					/= 5.0f;
					pull_vector.x			= uv_diff * uv_dir.x;
					pull_vector.z			= uv_diff * uv_dir.y;
					R_ASSERT(math::valid(pull_vector.x));
					R_ASSERT(math::valid(pull_vector.z));
					
					neighbours_weight.x		+= uv_diff;
					neighbours_weight.y		+= uv_diff;
				}
			}

			pull_vector.x	/= neighbours_count;
			pull_vector.z	/= neighbours_count;
					R_ASSERT(math::valid(pull_vector.x));
					R_ASSERT(math::valid(pull_vector.z));
						
			curr_processed_values_list[curr_vert_idx]	= pull_vector;
	
			
			if(!curr_processed_id_list->Contains(curr_vert_idx))
			{
				curr_processed_id_list->Add		(curr_vert_idx);
				curr_stored_values_list->Add	(curr_vert_idx, curr_vertex);
			}
		} // all active set verts
	}// all active set keys

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		pull_vector_dict^	curr_processed_values_list	= m_pull_vectors[key];
		terrain_node^		curr_terrain				= m_tool_terrain->get_terrain_core()->m_nodes[key];
		
		for each (u16 vert_idx in m_active_id_list[key])
		{
			terrain_vertex v	= curr_terrain->m_vertices.get(vert_idx);
			pull_vector% add	= curr_processed_values_list[vert_idx];
			#ifndef MASTER_GOLD

			float x = add.x;
			float z = add.z;

			R_ASSERT( math::valid( x ) );
			R_ASSERT( math::valid( z ) );

			#endif
			v.tc_shift_x		+= add.x;
			v.tc_shift_z		+= add.z;
			curr_terrain->m_vertices.set(v, vert_idx);
		}

		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key] );
	}

}

} // namespace editor
} // namespace xray


namespace xray {
namespace editor {

inline float SQR ( float const a) { return a*a; }

void terrain_modifier_uv_relax::do_work1()
{
	float const time			= m_timer->get_elapsed_sec();

	if(time<0.01f)
		return;

	m_timer->start				();

	float4x4					inv_transform;

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		if( !m_pull_vectors.ContainsKey(key))
		{
			m_pull_vectors.Add( key, gcnew pull_vector_dict);
			m_stored_values_list.Add( key, gcnew vertex_dict);
		}

		pull_vector_dict^ curr_processed_values_list	= m_pull_vectors[key];
		vert_id_list^	curr_processed_id_list			= m_processed_id_list[key];
		vertex_dict^	curr_stored_values_list			= m_stored_values_list[key];
		terrain_node^	curr_terrain					= m_tool_terrain->get_terrain_core()->m_nodes[key];

		inv_transform.try_invert			(curr_terrain->get_transform());
		float3 center_position_local		= inv_transform.transform_position(*m_center_position_global);

		for each (u16 vert_idx in m_active_id_list[key])
		{
			int left, top;	
			curr_terrain->vertex_xz( vert_idx, left, top);

			math::float3 pos_xz = curr_terrain->position_g(vert_idx);

			terrain_vertex v				= curr_terrain->m_vertices.get(vert_idx);

			float const distance			= curr_terrain->distance_xz(vert_idx, center_position_local);

			ASSERT							(distance<radius+math::epsilon_3);
			float influence					= calc_influence_factor(distance);


			pull_vector	pull_vector;
			pull_vector.x = 0;
			pull_vector.z = 0;

			const int quad_size = 3;
			for( int i = 0; i < quad_size; ++i)
			{
				for( int j = 0; j < quad_size; ++j)
				{
					if( i == quad_size/2 && j == quad_size/2)
						continue;

					terrain_node^ nnode = nullptr;
					u16 uidx = 0;
					get_neighbour_result res = get_neighbour(	curr_terrain,
																vert_idx, 
																i - quad_size/2, 
																j - quad_size/2,
																nnode,
																uidx );
					if(res==get_neighbour_result::missing)
						continue;

					terrain_vertex^ neighbour_vert = nnode->m_vertices.get(uidx);
					math::float3 neighbour_vert_pos_xz = nnode->position_g(uidx);

					float distance_norm = math::sqrt( SQR(neighbour_vert_pos_xz.x - pos_xz.x) + SQR(neighbour_vert_pos_xz.z - pos_xz.z) );

					float distance_flat = math::sqrt( SQR( neighbour_vert_pos_xz.x+neighbour_vert->tc_shift_x - pos_xz.x+v.tc_shift_x ) 
												+ SQR( neighbour_vert_pos_xz.z+neighbour_vert->tc_shift_z - pos_xz.z+v.tc_shift_z) );

					float distance_3d	= math::sqrt( SQR(distance_flat) + 1000 * SQR( v.height - neighbour_vert->height ) );

					float pull_factor =  (distance_3d - distance_norm)*0.01f;


					pull_factor = (pull_factor>0) ? pull_factor : 0;

					pull_vector.x += influence*pull_factor* ((neighbour_vert_pos_xz.x+neighbour_vert->tc_shift_x) - (pos_xz.x+v.tc_shift_x));
					pull_vector.z += influence*pull_factor* ((neighbour_vert_pos_xz.z+neighbour_vert->tc_shift_z) - (pos_xz.z+v.tc_shift_z));
				}

			}

			float module = math::sqrt( pull_vector.x*pull_vector.x + pull_vector.z*pull_vector.z);

			float clamp_module = (module <= 0.3f) ? module : 0.3f;

			if( module > 0.001)
			{
				pull_vector.x *= clamp_module/module;
				pull_vector.z *= clamp_module/module;
			}
						
			curr_processed_values_list[vert_idx]	= pull_vector;
	
			
			if(!curr_processed_id_list->Contains(vert_idx))
			{
				curr_processed_id_list->Add		(vert_idx);
				curr_stored_values_list->Add	(vert_idx, v);
			}

		} // verts in node
 
	}// keys

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		pull_vector_dict^	curr_processed_values_list	= m_pull_vectors[key];
		terrain_node^		curr_terrain				= m_tool_terrain->get_terrain_core()->m_nodes[key];
		
		for each (u16 vert_idx in m_active_id_list[key])
		{
			terrain_vertex v	= curr_terrain->m_vertices.get(vert_idx);
			v.tc_shift_x		+= curr_processed_values_list[vert_idx].x;
			v.tc_shift_z		+= curr_processed_values_list[vert_idx].z;
			curr_terrain->m_vertices.set(v, vert_idx);
		}

		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key] );
	}

}

} // namespace editor
} // namespace xray

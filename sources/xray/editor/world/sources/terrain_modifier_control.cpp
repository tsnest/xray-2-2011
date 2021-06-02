////////////////////////////////////////////////////////////////////////////
//	Created		: 14.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_modifier_control.h"
#include "terrain_object.h"
#include "terrain_modifier_command.h"
#include "terrain_modifier_smooth.h"
#include "terrain_control_cursor.h"
#include "level_editor.h"
#include "tool_terrain.h"
#include <xray/editor/base/collision_object_types.h>
#include "editor_world.h"
#include "window_ide.h"

#pragma managed( push, off )
#include <xray/render/facade/editor_renderer.h>
#include <xray/collision/collision_object.h>
#include <xray/render/facade/scene_renderer.h>
#pragma managed( pop )

namespace xray {
namespace editor {

terrain_modifier_control_base::terrain_modifier_control_base( level_editor^ le, tool_terrain^ tool )
:super		( le, tool )
{
	hardness			= 0.5f; //0..1
	power				= 0.5f;
	affect_objects_snap	= true;
}

terrain_modifier_control_base::~terrain_modifier_control_base( )
{}

float terrain_modifier_control_base::power_value()
{
	return ((power-m_min_power) / (m_max_power-m_min_power)) * 5.0f;
}

void terrain_modifier_control_base::end_input()
{
	super::end_input				();
	for each (terrain_node_key key in m_processed_id_list.Keys)
	{
		terrain_node^ curr_terrain			= m_tool_terrain->get_terrain_core()->m_nodes[key];
		curr_terrain->initialize_collision	();
		curr_terrain->get_editor_renderer().terrain_update_cell_aabb( m_level_editor->get_editor_world().scene(), 
																		curr_terrain->get_model()->m_render_model, 
																		curr_terrain->get_local_aabb());
	}

	m_processed_id_list.Clear		();
}

// 0..1
float terrain_modifier_control_base::calc_influence_factor(float const & distance)
{
	float hardness_bound	= radius * hardness;
	float res				= 0.0f;

	if(distance<hardness_bound)
	{
		res			= 1.0f;
	}else
	{
		float t		= 1.0f - (radius-distance) / (radius-hardness_bound);
		res			= (1+2*t)*(1-t)*(1-t);
		ASSERT			(res>=0.0f && res<=1.0f);
	}
	return			res;
}

void terrain_modifier_control_base::load_settings(RegistryKey^ key)
{
	super::load_settings		(key);
	RegistryKey^ self_key		= get_sub_key(key, name);

	hardness					= convert_to_single(self_key->GetValue("hardness", "0.5"));
	power						= convert_to_single(self_key->GetValue("strength",	"0.5"));

	self_key->Close				( );
}

void terrain_modifier_control_base::save_settings( RegistryKey^ key )
{
	super::save_settings		( key );
	RegistryKey^ self_key		= get_sub_key( key, name );

	self_key->SetValue			( "hardness", convert_to_string(hardness) );
	self_key->SetValue			( "strength", convert_to_string(power) );

	self_key->Close				( );
}

void terrain_modifier_control_base::change_property( System::String^ prop_name, float const dx, float const dy )
{
	if(prop_name=="radius_and_power")
	{
		if( math::abs(dy) > math::abs(dx) )
		{
			float const power_value		= (m_max_power-m_min_power)*dy/500.0f;
			power						+= power_value;
		}
	}

	super::change_property(prop_name, dx, dy);
}

terrain_control_base::terrain_control_base(level_editor^ le, tool_terrain^ tool)
:super				( ),
m_level_editor		( le ),
m_last_screen_xy	( -10000, -10000 ),
m_center_position_global(NEW (float3)(0,0,0)),
m_apply_command		( nullptr ),
m_visible			( false ),
m_terrain_cursor	( nullptr ),
m_tool_terrain		( tool )
{
	shape_type		= modifier_shape::circle;
	radius			= 5.0f; //1..50
}

terrain_control_base::~terrain_control_base( )
{
	DELETE			( m_center_position_global );
	delete			( m_terrain_cursor );
}

u32 terrain_control_base::acceptable_collision_type( )
{
	return editor_base::collision_object_type_terrain;
}

void terrain_control_base::initialize( )
{}

void terrain_control_base::destroy( )
{}

void terrain_control_base::activate( bool b_activate )
{
	super::activate				(b_activate);
	m_visible					= b_activate;

	if(m_terrain_cursor==nullptr)
		create_cursor			( m_level_editor->get_editor_world().scene() );
	
	m_terrain_cursor->show		(b_activate);
}

void terrain_control_base::update()
{
	ASSERT(m_terrain_cursor);
	m_terrain_cursor->update();

}

void terrain_control_base::pin(bool bpin)
{
	m_terrain_cursor->pin(bpin);
}

void terrain_control_base::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	super::start_input				( mode, button, coll, pos );

	ASSERT							( m_apply_command == nullptr );
	create_command					( );
	if(m_apply_command)
		m_level_editor->get_command_engine()->run_preview( m_apply_command );
}

void terrain_control_base::end_input()
{
	super::end_input				();

	if(m_apply_command)
	{
		m_level_editor->get_command_engine()->end_preview();
		m_apply_command					= nullptr;
	}
	m_active_id_list.Clear			();
}

void terrain_control_base::draw( render::scene_view_pointer scene_view, xray::render::render_output_window_pointer /*output_window*/ )
{
	XRAY_UNREFERENCED_PARAMETER	( scene_view );
}

float3 terrain_control_base::get_picked_point()
{
	return *m_terrain_cursor->m_last_picked_position;
}


void terrain_modifier_control_base::refresh_active_working_set(modifier_shape t)
{
	m_active_id_list.Clear				();

	collision::object const* picked_collision = NULL;
	
	if(m_level_editor->view_window()->ray_query( acceptable_collision_type(), &picked_collision, m_center_position_global ) )
	{
		ASSERT( picked_collision->get_type() & acceptable_collision_type() );
	
		m_tool_terrain->get_terrain_core()->select_vertices(*m_center_position_global, radius, t, %m_active_id_list);

		for each (terrain_node_key key in m_active_id_list.Keys)
			if(!m_processed_id_list.ContainsKey(key))
				m_processed_id_list.Add			(key, gcnew vert_id_list);
	}

	on_working_set_changed				();
}


//math::int2 terrain_control_base::get_mouse_position()
//{
//	return m_level_editor->ide()->get_mouse_position_int();
//}

void terrain_control_base::load_settings(RegistryKey^ key)
{
	super::load_settings		(key);
	RegistryKey^ self_key		= get_sub_key(key, name);

	shape_type					= (modifier_shape)(System::Enum::Parse(modifier_shape::typeid, self_key->GetValue("shape_type", modifier_shape::circle)->ToString()));
	radius						= convert_to_single(self_key->GetValue("radius", "3.0"));

	self_key->Close				();
}

void terrain_control_base::save_settings(RegistryKey^ key)
{
	super::save_settings		(key);
	RegistryKey^ self_key		= get_sub_key(key, name);

	self_key->SetValue			("shape_type", shape_type);
	self_key->SetValue			("radius", convert_to_string(radius) );

	self_key->Close				();
}

void terrain_control_base::change_property( System::String^ prop_name, float const dx, float const dy )
{
	if(prop_name=="radius_and_power")
	{
		if( math::abs(dx) > math::abs(dy) )
		{
			float const radius_value	= dx;
			radius						+= radius_value;
		}
	}
	
	super::change_property	( prop_name, dx, dy );
}


terrain_modifier_height::terrain_modifier_height(xray::editor::level_editor^ le, tool_terrain^ tool)
:super			( le, tool ),
m_smooth_mode	( false )
{
	m_smooth_matrix_dim		= 7;
	m_smooth_timer			= NEW( timing::timer );
}

terrain_modifier_height::~terrain_modifier_height()
{
	DELETE					( m_smooth_timer );
}

void terrain_modifier_height::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	if(m_level_editor->ide()->get_held_keys()->Contains(System::Windows::Forms::Keys::ShiftKey))
	{
		m_smooth_mode	= true;
		init_smooth		( );
	}

	super::start_input		( mode, button, coll, pos );
}

void terrain_modifier_height::end_input( )
{
	for each (terrain_node_key key in m_processed_id_list.Keys)
	{
		height_diff_dict^ curr_processed_values_list	= m_processed_values_list[key];
		height_diff_dict^ curr_stored_values_list		= m_stored_values_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];

		for each (u16 vert_idx in m_processed_id_list[key])
		{
			float old_h							= curr_stored_values_list[vert_idx];
			float new_h							= curr_terrain->m_vertices.get(vert_idx).height;
			curr_processed_values_list[vert_idx] = new_h - old_h;
		}

		(safe_cast<terrain_modifier_height_command^>(m_apply_command))->set_data(key, m_processed_values_list[key]);
	}

	super::end_input				( );
	m_processed_values_list.Clear	( );
	m_stored_values_list.Clear		( );
	m_smooth_mode					= false;
}

void terrain_modifier_height::on_working_set_changed()
{
	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		if(!m_stored_values_list.ContainsKey(key))
		{
			m_stored_values_list.Add	(key, gcnew height_diff_dict);
			m_processed_values_list.Add	(key, gcnew height_diff_dict);
		}

		height_diff_dict^ curr_stored_values_list		= m_stored_values_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];

		for each (u16 vert_idx in m_active_id_list[key])
		{
			if(!curr_stored_values_list->ContainsKey(vert_idx))
				curr_stored_values_list[vert_idx] = curr_terrain->m_vertices.get(vert_idx).height;
		}
	}
}

void terrain_modifier_height::create_command()
{
	m_apply_command			= gcnew terrain_modifier_height_command(this, m_tool_terrain->get_terrain_core());
}

void terrain_modifier_height::create_cursor( render::scene_ptr const& scene )
{
	m_terrain_cursor		= gcnew terrain_painter_cursor(this, scene);
}

enum class get_height_result{ok, missing};

get_height_result get_height(	terrain_node^ terrain,
								u16 origin_vert_idx, 
								int const& offset_x_idx, 
								int const& offset_z_idx, 
								float& result_height )
{
	terrain_node^ result_node	= nullptr;
	u16 result_vert_idx			= 0;
	get_neighbour_result result =  get_neighbour( terrain,
													origin_vert_idx, 
													offset_x_idx, 
													offset_z_idx,
													result_node,
													result_vert_idx );

	if(result==get_neighbour_result::ok)
	{
		result_height = result_node->m_vertices.get(result_vert_idx).height;
		return get_height_result::ok;
	}else
		return get_height_result::missing;
}

// smooth support
void terrain_modifier_height::do_smooth( )
{

	float const time			= m_smooth_timer->get_elapsed_sec();

	if(time<0.01f)
		return;

	m_smooth_timer->start				();

	float4x4					inv_transform;


	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		height_diff_dict^ curr_processed_values_list	= m_processed_values_list[key];
		vert_id_list^ curr_processed_id_list			= m_processed_id_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];
		inv_transform.try_invert			(curr_terrain->get_transform());
		float3 center_position_local		= inv_transform.transform_position(*m_center_position_global);

		for each (u16 vert_idx in m_active_id_list[key])
		{
			float	influenced_verts_weight	= 0.0;
			float	current_vert_sum		= 0.0;

			for(int ix=0; ix<m_smooth_matrix_dim; ++ix)
			{
				int		_x					= ix - (m_smooth_matrix_dim>>1);
				for (int iy=0; iy<m_smooth_matrix_dim; ++iy)
				{
					int _y					= iy - (m_smooth_matrix_dim>>1);
					float _h				= math::float_min;
					float infl				= m_gaussian_matrix[ix, iy];

					if(get_height_result::ok == get_height(curr_terrain, vert_idx, _x, _y, _h) )
					{
						ASSERT(math::valid(_h));
						influenced_verts_weight	+= infl;
						current_vert_sum		+= _h * infl;
					}
					// else out of terrain bound
				}
			}// in matrix
			ASSERT(influenced_verts_weight>0.0);

			terrain_vertex v				= curr_terrain->m_vertices.get(vert_idx);
			float const distance			= curr_terrain->distance_xz(vert_idx, center_position_local);

			ASSERT							(distance<radius+math::epsilon_3);
			float influence					= calc_influence_factor(distance);

			float vert_height_new			= current_vert_sum / influenced_verts_weight;
			float vert_height_old			= v.height;
			float vert_height				= vert_height_old + influence * (vert_height_new - vert_height_old); // lerp

			if(math::is_zero(vert_height))
				vert_height = 0.0f;

			curr_processed_values_list[vert_idx]	= (float)vert_height;
			
			if(!curr_processed_id_list->Contains(vert_idx))
				curr_processed_id_list->Add		(vert_idx);

		} // verts in node

	}// keys

	for each (terrain_node_key key in m_active_id_list.Keys)
	{
		height_diff_dict^ curr_processed_values_list	= m_processed_values_list[key];
		terrain_node^ curr_terrain						= m_tool_terrain->get_terrain_core()->m_nodes[key];
		
		for each (u16 vert_idx in m_active_id_list[key])
		{
			terrain_vertex v	= curr_terrain->m_vertices.get(vert_idx);
			v.height			= curr_processed_values_list[vert_idx];
			curr_terrain->m_vertices.set(v, vert_idx);
		}

		m_tool_terrain->get_terrain_core()->sync_visual_vertices(curr_terrain, m_active_id_list[key] );
	}

}

void terrain_modifier_height::init_smooth( )
{
	terrain_modifier_smooth^ smooth_control = safe_cast<terrain_modifier_smooth^>(m_level_editor->get_editor_control("terrain_modifier_smooth"));

	m_gaussian_matrix			= gcnew cli::array<float, 2>(m_smooth_matrix_dim, m_smooth_matrix_dim);
	
	static float const min_strength	= 0.25f;
	static float const max_strength	= 0.7f;
	
	float const smooth_strength		= min_strength + (max_strength-min_strength) * smooth_control->power;
	float g0						= 1/(2 * math::pi * smooth_strength * smooth_strength);

	for (int ix=0; ix<m_smooth_matrix_dim; ++ix)
	{
		int _x						= ix - (m_smooth_matrix_dim>>1);
		for (int iy=0; iy<m_smooth_matrix_dim; ++iy)
		{
			int _y					= iy - (m_smooth_matrix_dim>>1);
			float g1				= exp(-(_x*_x + _y*_y)/ (2 * smooth_strength * smooth_strength));
			float g					= g0*g1;

			m_gaussian_matrix[ix, iy]= g;
		}
	}
	m_smooth_timer->start				( );
}

void terrain_modifier_height::change_property( System::String^ prop_name, float const dx, float const dy )
{
	if(prop_name == "smooth_power")
	{
		terrain_modifier_smooth^ smooth_control = safe_cast<terrain_modifier_smooth^>(m_level_editor->get_editor_control("terrain_modifier_smooth"));
		smooth_control->change_property( "radius_and_power", 0.0f, dy );// filter only POWER
	}

	super::change_property( prop_name, dx, dy );
}

} // namespace editor
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_base.h"
#include "collision_object.h"

#include "level_editor.h"
#include "project.h"
#include "object_base.h"

#pragma managed(push,off)
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/api.h>
#pragma managed(pop)

using xray::collision::space_partitioning_tree;

namespace xray {
namespace editor {

void editor_control_base::activate( bool b_activate )
{
	m_active = b_activate;

	if(b_activate)
		on_activated(this);
	else
		on_deactivated(this);
}

void editor_control_base::subscribe_on_activated( activate_control_event^ d)
{
	on_activated += d;
}

void editor_control_base::subscribe_on_deactivated( deactivate_control_event^ d)
{
	on_deactivated += d;
}

void editor_control_base::subscribe_on_property_changed(property_changed^ d)
{
	on_property_changed	+= d;
}

void editor_control_base::change_property(System::String^ prop_name, float const value)
{
	if(prop_name=="sensitivity")
	{
		sensitivity	+= value;
		sensitivity	= math::clamp_r(sensitivity, 0.1f, 20.0f);
	}
	on_property_changed(this);
}

void editor_control_base::load_settings(RegistryKey^ key)
{
	RegistryKey^ self_key	= get_sub_key(key, name);
	sensitivity				= System::Convert::ToSingle(self_key->GetValue("sensitivity", "1") );
	self_key->Close			();
}

void editor_control_base::save_settings(RegistryKey^ key)
{
	RegistryKey^ self_key	= get_sub_key(key, name);
	self_key->SetValue		("sensitivity", sensitivity);
	self_key->Close			();
}

void editor_control_base::start_input( )
{
	m_ide->set_mouse_sensivity(sensitivity);
}

void editor_control_base::end_input( )
{
	m_ide->set_mouse_sensivity(1.0f);
}

struct transform_control_base_internal : private boost::noncopyable
{
	transform_control_base_internal()
	:m_transform		( float4x4().identity() ),
	m_view_transform	( float4x4().identity() ),
	m_selected_color	( 255u,	255u, 0u, 255u ),
	m_last_position_lines_color		( 100u, 100u, 100u, 255u )
	{
		m_axes_vectors.resize( enum_transform_axis_COUNT );
		m_axes_vectors[enum_transform_axis_cam]		= float3( 0, 0, 0 );
		m_axes_vectors[enum_transform_axis_x]		= float3( 1, 0, 0 );
		m_axes_vectors[enum_transform_axis_y]		= float3( 0, 1, 0 );
		m_axes_vectors[enum_transform_axis_z]		= float3( 0, 0, 1 );
		m_axes_vectors[enum_transform_axis_all]		= float3( 1, 1, 1 ).normalize();
		m_axes_vectors[enum_transform_axis_none]	= float3( 0, 0, 0 );

		m_colors_default.resize( enum_transform_axis_COUNT );
		m_colors_default[enum_transform_axis_cam]	= math::color_xrgb(	0,		255,	255	);
		m_colors_default[enum_transform_axis_x]		= math::color_xrgb(	255,	0,		0	);
		m_colors_default[enum_transform_axis_y]		= math::color_xrgb(	0,		255,	0	);
		m_colors_default[enum_transform_axis_z]		= math::color_xrgb(	0,		0,		255 );
		m_colors_default[enum_transform_axis_all]	= math::color_xrgb( 70,		70,		70	);
		m_colors_default[enum_transform_axis_none]	= math::color_xrgb( 0,		0,		0	);

		m_axes_visibilities.resize		( enum_transform_axis_COUNT, true ); 
	}
	collision_objects_vec				m_collision_objects;
	geometries_vec						m_geometries;


	float4x4							m_transform;
	float4x4							m_view_transform;


	vector < float3 >					m_axes_vectors;
	vector < color >					m_colors_default;
	vector < color >					m_colors;
	vector < bool >						m_axes_visibilities;
	color const							m_selected_color;
	color const							m_last_position_lines_color;

};

transform_control_base::transform_control_base( level_editor^ le)
:super				(le->ide()),
m_level_editor		( le ),
m_data				( NEW (transform_control_base_internal) ),
m_control_visible	( false ),
m_draw_collision_geomtery ( false ),
m_dragging			( false ),
m_preview_mode		( false ),
m_cursor_thickness	( 0.2f ),
m_last_position_vertex_radius	( 5.f ),
m_distance_from_cam				( 20.f ),
m_transparent_line_pattern		( 0x55555555 ),
m_active_axis					( enum_transform_axis_cam ),
m_apply_command					( nullptr ),
m_aim_object					( nullptr )
{
	sensitivity				= 1.0f;
	reset_colors			();
	m_data->m_colors[m_active_axis]	= m_data->m_selected_color;
}

transform_control_base::~transform_control_base()
{
	DELETE (m_data);
}

void transform_control_base::activate(bool bactivate)
{
	super::activate(bactivate);

	if(bactivate)
		m_data->m_view_transform	= m_level_editor->get_inverted_view_matrix();
}

void transform_control_base::initialize( )
{
	set_transform		( create_scale( float3( 0.5f, 0.7f, 2.f ) )
							* math::create_rotation_x	( -math::pi_d4 )
							* create_translation	( float3( 0.f, 0.f, 50.f ) ) );
}

void transform_control_base::destroy( )
{
	for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
		DELETE (m_data->m_collision_objects[i]);

	m_data->m_collision_objects.clear();

	for( u32 i = 0; i < m_data->m_geometries.size(); ++i)
		collision::destroy	( m_data->m_geometries[i] );

	m_data->m_geometries.clear();
}

void transform_control_base::set_transform		( float4x4 const& transform )
{
	m_data->m_transform = transform;

	collision_objects_vec::const_iterator	i = m_data->m_collision_objects.begin();
	collision_objects_vec::const_iterator	e = m_data->m_collision_objects.end();
	for( ; i != e; ++i )
		(*i)->set_matrix	( calculate_fixed_size_transform( m_data->m_transform, m_distance_from_cam, m_data->m_view_transform ) );

	update_colision_tree	( );
}

void transform_control_base::update			( )
{
	// make it event-based !!!
	object_base^ o = m_level_editor->get_project()->aim_object();
	
	if( o && !m_control_visible )
		show(true);
	else
	if( !o && m_control_visible )
		show(false);

	if ( !m_control_visible )
		return;

	m_data->m_view_transform	= m_level_editor->get_inverted_view_matrix();

	if(m_aim_object != o || !m_data->m_transform.c.similar(o->get_transform().c) )
	{
		m_aim_object = o;
		set_transform( m_aim_object->get_transform() );
	}
}

float transform_control_base::calculate_fixed_size_scale( float3 const& pos, float distance, float4x4 const& inv_view )
{
	ASSERT( distance != 0 );
	//float3 const start_point = transform.transform_position( float3( 0, 0, 0 ) );
	float3 const distance_vector		= inv_view.c.xyz() - pos;
	float const distance_magnitude		= distance_vector.magnitude();

	if( math::is_zero( distance_magnitude ) )
		return 0;

	return (distance_magnitude / distance) * -math::sign(distance_vector|inv_view.k.xyz());
}

float4x4 transform_control_base::calculate_fixed_size_transform	( float4x4 const& transform, float distance, float4x4 const& view )
{
	ASSERT( distance != 0 );
	float scale = calculate_fixed_size_scale	( transform.c.xyz(), distance, view );

	return	create_scale( float3( scale, scale, scale ) ) * transform;
}

bool	transform_control_base::rey_nearest_point	(  float3 const& origin1, float3 direction1, float3 const& origin2, float3 direction2, float3& intersect )
{
	direction1.normalize();
	direction2.normalize();

	float3 perp_direction = direction1^direction2; // cross product

	if( fabs( direction1.dot_product(direction2)) > (1 - math::epsilon_3) )
		return false;

	return plane_ray_intersect	( origin2, perp_direction^direction2, origin1, direction1, intersect );
}

bool transform_control_base::plane_ray_intersect	( float3 const& plane_origin, float3 const& plane_normal, float3 const& ray_origin, float3 const& ray_direction, float3& intersect )
{
	if( fabs( plane_normal.dot_product(ray_direction)) < math::epsilon_3 )
		return false;

	float d = -plane_normal.dot_product(plane_origin);

	float t = -( d + (plane_normal.dot_product(ray_origin))) / plane_normal.dot_product(ray_direction);

	intersect = ray_origin+t*ray_direction;

	return true;
}

void transform_control_base::show			( bool show )
{
	if( m_control_visible != show )
	{
		m_control_visible = show;

		space_partitioning_tree* tree = m_level_editor->get_collision_tree( );

		if( m_control_visible )
		{
			for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
				tree->insert( m_data->m_collision_objects[i], m_data->m_transform.transform_position( m_data->m_collision_objects[i]->get_center() ), m_data->m_collision_objects[i]->get_extents() );
		}
		else
		{
			for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
				tree->remove( m_data->m_collision_objects[i] );
		}
	}

	if( m_control_visible )
	{
		m_aim_object		= m_level_editor->get_project()->aim_object();
		set_transform		( m_aim_object->get_transform() );
	}
}

void transform_control_base::update_colision_tree()
{
	space_partitioning_tree* const tree = m_level_editor->get_collision_tree( );

	for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
		tree->move		( m_data->m_collision_objects[i], m_data->m_collision_objects[i]->get_center(), m_data->m_collision_objects[i]->get_extents());
}

float3 transform_control_base::get_axis_vector( enum_transform_axis mode )
{
	if( (u32) mode >= m_data->m_axes_vectors.size() )
	{
		ASSERT ( "Unknown axis id !" );
		return m_data->m_axes_vectors[enum_transform_axis_none];
	}

	if( mode == enum_transform_axis_cam )
		return -m_data->m_view_transform.k.xyz();

	return m_data->m_axes_vectors[mode];
}

void transform_control_base::set_draw_geomtry	( bool draw )
{
	m_draw_collision_geomtery = draw;
}

void transform_control_base::reset_colors		( )
{
	m_data->m_colors.resize( m_data->m_colors_default.size() );

	const u32 count = m_data->m_colors.size();
	for ( u32 i = 0; i < count; ++i )
		m_data->m_colors[i] = m_data->m_colors_default[i];
}

bool transform_control_base::one_covers_another	(	float3 position1, float radius1, 
													float3 position2, float radius2,  float4x4 const& view )
{
	float3 distance_vector1 = position1 - view.c.xyz();
	float3 distance_vector2 = position2 - view.c.xyz();
	float distance1 = distance_vector1.magnitude();
	float distance2 = distance_vector2.magnitude();

	if( distance1 < distance2 )
	{	
		if( math::is_zero( distance1 ) )
			return true;

		float scale_factor = distance2/distance1;
		radius1	*= scale_factor;
		distance_vector1 *= scale_factor;
		position1 = view.c.xyz() + distance_vector1;
	}
	else
		return false;

	if( (position2-position1).magnitude() < radius2+radius1)
		return true;

	return false;
}

void transform_control_base::set_mode_modfiers			( bool plane_mode, bool free_mode )
{
	m_plane_mode_modifier	= plane_mode;
	m_free_mode_modifier	= free_mode;
}

u32 transform_control_base::acceptable_collision_type()
{
	return collision_type_control;
}


float4x4 const&	transform_control_base::get_view_transform()
{
	return m_data->m_view_transform;
}

float4x4 const&	transform_control_base::get_transform()
{
	return m_data->m_transform;
}

geometries_vec& transform_control_base::get_geometries()
{
	return m_data->m_geometries;
}

collision_objects_vec& transform_control_base::get_collision_objects()
{
	return m_data->m_collision_objects;
}

color& transform_control_base::get_color(int idx)
{
	return m_data->m_colors[idx];
}

bool transform_control_base::is_axis_visible(int idx)
{
	return m_data->m_axes_visibilities[idx];
}

void transform_control_base::set_axis_visible(bool val, int idx)
{
	m_data->m_axes_visibilities[idx]=val;
}

color const& transform_control_base::selection_color()
{
	return m_data->m_selected_color;
}

color const& transform_control_base::last_position_line_color()
{
	return m_data->m_last_position_lines_color;
}

} // editor
} // xray

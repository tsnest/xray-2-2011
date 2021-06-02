////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_rotation.h"
#include <xray/editor/base/collision_object_control.h>
#include "transform_control_object.h"

#pragma managed( push, off )
#include <xray/geometry_utils.h>
#pragma managed( pop )

namespace xray{
namespace editor_base{

transform_control_rotation::transform_control_rotation( transform_control_helper^ helper )
:super						( helper ),
m_radius					( 4.5f ),
m_outer_radius_ratio		( 1.1f ),
m_inner_radius_ratio		( 0.95f ),
m_outer_radius				( m_outer_radius_ratio*m_radius ),
m_inner_radius				( m_inner_radius_ratio*m_radius ),
m_last_pos_segment_step		( math::pi/16 ),
m_outer_ring				( NULL ),
m_current_angle				( 0 ),
m_rolling_mode				( false ),
m_rolling_first_step		( false ),
m_data						( NEW (transform_control_rotation_internal) )
{
	m_cursor_thickness		= 0.12f;
	m_control_id			= "rotation";
}

transform_control_rotation::~transform_control_rotation()
{
	destroy	( );
	DELETE (m_data);
}

void transform_control_rotation::initialize( )
{
	super::initialize							();
	float4x4 tmp_transform;

	vectora< math::float3 >						vertices		(&debug::g_mt_allocator);
	vectora< u16 >								render_indices	(&debug::g_mt_allocator);

	tmp_transform.identity		( );
	geometry_utils::create_torus( 
									vertices, 
									render_indices, 
									tmp_transform, 
									m_radius, 
									m_cursor_thickness, 32, 8 );


	vectora< u32 >			collision_indices		(*g_allocator);
	vectora< u16 >::const_iterator it				= render_indices.begin();
	vectora< u16 >::const_iterator it_e				= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	xray::non_null<collision::geometry>::ptr geom	= collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	get_geometries().push_back						( &*geom );

	collision_object_transform_control* tem_obj;

	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_y
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_x
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_z
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );


	// Create outer torus mesh
	geometry_utils::create_torus( 
																vertices, 
																render_indices, 
																tmp_transform.identity(), 
																m_outer_radius, 
																m_cursor_thickness, 32, 8 );

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	// Create outer torus geometry
	geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_cam
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	m_outer_ring = tem_obj;

	// Create inner sphere mesh
	geometry_utils::create_ellipsoid( 
		vertices, 
		render_indices, 
		tmp_transform.identity(), 
		float3( m_inner_radius, m_inner_radius, m_inner_radius ) );

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	// Create inner sphere geometry
	geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_all
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );
}

void transform_control_rotation::update( )
{
	super::update				();

	if( m_shown )
	{
		float3 vector_to_object = get_inv_view().c.xyz() - get_matrix().c.xyz();

		if (vector_to_object.length()>math::epsilon_3)
			vector_to_object.normalize();
		else
			vector_to_object = get_inv_view().k.xyz();

		float4x4 trans			= create_rotation( (vector_to_object^get_inv_view().i.xyz()).normalize(), vector_to_object );

		// This is needed to compensate global tranformation rotations.
		float4x4 trans_inv		= invert4x3( create_rotation( get_matrix().get_angles_xyz() ));

		m_outer_ring->set_matrix_original( trans*trans_inv );
	}
}

void transform_control_rotation::set_matrix( float4x4 const& m )
{
	// Rotation control ignores the object scaling 
	m_data->m_translation_transform	= create_translation( m.c.xyz() );
	float4x4  rotation_transform	= create_rotation	( m.get_angles_xyz() );
	float4x4 tmp_tranform			= rotation_transform *	m_data->m_translation_transform;
	super::set_matrix				( tmp_tranform );
	m_data->m_draw_transform		= tmp_tranform;
}

void transform_control_rotation::one_step_execute( enum_transform_axis axis, float value )
{
	m_active_axis						= axis;
	m_helper->m_object->start_modify	( this );
	m_current_angle						= step_value * -value;
	m_helper->m_object->execute_preview	( this );
	m_helper->m_object->end_modify		( this );
	m_active_axis						= enum_transform_axis_none;
}

void transform_control_rotation::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	super::start_input	( mode, button, coll, pos );
	m_dragging = true;
	m_preview_mode = true;

	if( m_plane_mode_modifier || m_free_mode_modifier )
		m_rolling_mode = true;
	else
		m_rolling_mode = false;

	float3 mouse_origin;
	float3 mouse_direction;
	m_current_angle = 0.f;

	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

	float3 saved_origin = get_matrix().c.xyz();
	m_data->m_curr_axis_vector = create_rotation_axis( get_matrix() );

	float4x4 control_tranform = calculate_fixed_size_transform( get_matrix() );
	float3 control_position = control_tranform.c.xyz();

	if( !m_rolling_mode )
	{
		float radius = (m_active_axis != enum_transform_axis_all) ? m_radius : m_inner_radius;

		if( m_active_axis == enum_transform_axis_cam )
			radius = m_outer_radius;

		radius		*= calculate_fixed_size_scale( get_matrix().c.xyz() );

		math::sphere axes_sphere( control_position, radius );

		float	intersec_points[2];
		u32		intersec_points_count;
		math::intersection intersec_result = axes_sphere.intersect_ray( mouse_origin, mouse_direction, 1000.f, intersec_points_count, intersec_points );

		if( intersec_result == math::intersection_outside )
		{
			const float3 intersect_point = intersec_points[0]*mouse_direction+mouse_origin;
			m_data->m_start_radius_vector = (intersect_point-control_position);

		}else
		{
			float3 saved_origin = get_matrix().c.xyz();
			float3 inter_point;
			bool res = plane_ray_intersect( saved_origin, mouse_direction, mouse_origin, mouse_direction, inter_point );
			ASSERT_U( res, "The plane must have intersection with the ray." );

			inter_point = (inter_point - saved_origin ).normalize();
			m_data->m_start_radius_vector = inter_point*radius;
		}


		if( m_active_axis != enum_transform_axis_all )
		{
			float dot = m_data->m_curr_axis_vector.dot_product( m_data->m_start_radius_vector );
			m_data->m_start_plane_vector = m_data->m_start_radius_vector - dot* m_data->m_curr_axis_vector;
			m_data->m_start_plane_vector.normalize();
			float dot1 = m_data->m_curr_axis_vector.dot_product( m_data->m_start_plane_vector );
			dot1 = dot1 ;
		}else
		{
			m_result->identity();
		}
	}else
	{
		bool res = plane_ray_intersect( control_position, get_inv_view().k.xyz(), mouse_origin, mouse_direction, m_data->m_drag_start );
		ASSERT_U( res, "The plane must have intersection with the ray." );

		m_rolling_first_step = true;
	}
}

void transform_control_rotation::execute_input( )
{
	float3 saved_origin = get_matrix().c.xyz();

	float3 mouse_origin;
	float3 mouse_direction;
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

	float4x4 control_tranform = calculate_fixed_size_transform( get_matrix() );
	
	float3 control_position = control_tranform.c.xyz();

	float radius = (m_active_axis != enum_transform_axis_all) ? m_radius : m_inner_radius;

	if( m_active_axis == enum_transform_axis_cam )
		radius = m_outer_radius;

	radius *= calculate_fixed_size_scale	( get_matrix().c.xyz() );


	float3 radius_vector;

	if( !m_rolling_mode )
	{
		math::sphere axes_sphere( control_position, radius );

		float	intersec_points[2];
		u32		intersec_points_count;
		math::intersection intersec_result = axes_sphere.intersect_ray( mouse_origin, mouse_direction, 1000.f, intersec_points_count, intersec_points );

		if( intersec_result != math::intersection_none )
		{
			const float3 intersect_point	= intersec_points[0]*mouse_direction+mouse_origin;
			radius_vector		= (intersect_point-control_position);
		}
		else
		{
			float3 inter_point;
			bool res = plane_ray_intersect( saved_origin, mouse_direction, mouse_origin, mouse_direction, inter_point );
			ASSERT_U( res, "The plane must have intersection with the ray." );

			inter_point			= (inter_point - saved_origin ).normalize();
			radius_vector		= inter_point*radius;
		}

		if( m_active_axis == enum_transform_axis_all )
		{
			m_data->m_curr_axis_vector = radius_vector^m_data->m_start_radius_vector.normalize();

			float curr_axis_length = m_data->m_curr_axis_vector.length();
			if( math::is_zero( curr_axis_length ) )
				return;

			m_data->m_curr_axis_vector /= curr_axis_length;

			m_data->m_start_plane_vector = m_data->m_start_radius_vector - m_data->m_curr_axis_vector.dot_product( m_data->m_start_radius_vector )* m_data->m_curr_axis_vector;
			m_data->m_start_plane_vector.normalize();
		}

		float3 curr_plane_vector = radius_vector - m_data->m_curr_axis_vector.dot_product( radius_vector ) * m_data->m_curr_axis_vector;
		curr_plane_vector.normalize();

		float3 normal_vector = m_data->m_start_plane_vector^curr_plane_vector;
		float normal_vector_length = normal_vector.length();

		if( math::is_zero( normal_vector_length ) )
			return;

		normal_vector		/= normal_vector_length;

		float direction		= -normal_vector|m_data->m_curr_axis_vector;

		float acosine = m_data->m_start_plane_vector | curr_plane_vector;

		ASSERT( math::valid( acosine ) );

		math::clamp( acosine, -1.f, 1.f );
		m_current_angle = direction*math::acos( acosine );
	}else // if(!rolling_mode)
	{
		float3 curr_pos;
		bool res = plane_ray_intersect( control_position, get_inv_view().k.xyz(), mouse_origin, mouse_direction, curr_pos );
		ASSERT_U( res, "The plane must have intersection with the ray." );

		float3 drag_vector = curr_pos - m_data->m_drag_start;
	
		float drag_vector_length = drag_vector.length();
		if( math::is_zero( drag_vector_length ) )
			return;
		
		float3 projected_vector;
		if( m_rolling_first_step )
		{
			m_rolling_first_step	= false;
			m_data->m_rolling_axis	= drag_vector/drag_vector_length;
			projected_vector		= drag_vector; 
		}else
		{
			projected_vector		= m_data->m_rolling_axis*( m_data->m_rolling_axis | drag_vector );
		}
		

		m_current_angle = ( projected_vector | m_data->m_rolling_axis ) / radius*math::pi_d2;
	}

	float4x4 self_rotation	= create_rotation( get_matrix().get_angles_xyz() );
	float3 local_vector		= math::invert4x3( self_rotation ).transform_direction( m_data->m_curr_axis_vector );
	float4x4 rotation		= create_rotation( local_vector,  m_current_angle );
	
	if( m_active_axis == enum_transform_axis_all && !m_rolling_mode )
	{
		m_data->m_start_radius_vector	= radius_vector;
		*m_result						= *m_result * rotation;
	}
	else
		*m_result						= rotation;

	m_data->m_draw_transform			= *m_result * get_matrix();

	super::execute_input				( );
}

void transform_control_rotation::end_input( )
{
	m_dragging					= false;

	set_matrix					( m_data->m_draw_transform );
	m_data->m_draw_transform	= get_matrix( );

	super::end_input			( );

	m_result->identity			( );
	m_current_angle				= 0.f;
	m_preview_mode				= false;
}

float4x4 transform_control_rotation::calculate( float4x4 const& m )
{
	float4x4 translation	= create_translation( m.c.xyz() );

	float4x4 rotation_to_apply;
	if( m_active_axis != enum_transform_axis_all )
	{
		float3 axis			= create_rotation_axis( m );
		rotation_to_apply	= create_rotation( axis, m_current_angle );
		return				( m * math::invert4x3(translation) * rotation_to_apply * translation );
	}else
	{
		float4x4 rotation	= create_rotation( m.get_angles_xyz() );
		return				( m * math::invert4x3(rotation*translation) * (*m_result) * rotation * translation );
	}
}

float3 transform_control_rotation::create_rotation_axis( float4x4 transform )
{
	float3 rotation_vector;

	if( m_active_axis == enum_transform_axis_cam )
	{
		rotation_vector = (get_inv_view().c.xyz() - transform.c.xyz() ).normalize();
	}
	else if( m_active_axis != enum_transform_axis_all || m_rolling_mode )
	{
		rotation_vector = get_axis_vector( m_active_axis );
		rotation_vector = transform.transform_direction( rotation_vector );
		rotation_vector.normalize();
	}

	return rotation_vector;
}

} // namespace editor
} // namespace xray

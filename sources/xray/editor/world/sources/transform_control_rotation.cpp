////////////////////////////////////////////////////////////////////////////
//	Created		: 26.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_rotation.h"

#include "collision_object_control.h"
#include "level_editor.h"
#include "command_apply_control_transform.h"

#pragma unmanaged
#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/world.h>
#include <xray/render/base/editor_renderer.h>
#include <xray/editor/world/engine.h>
#include <xray/geometry_utils.h>
#pragma managed

using xray::math::float3;
using xray::math::float4x4;
using namespace xray::memory;


using xray::editor::editor_control_base;
using xray::editor::transform_control_rotation;
using xray::editor::transform_control_base;

using xray::editor::collision_object_transform_control;
using xray::editor::collision_object;
using xray::editor::enum_transform_axis;
using xray::editor::enum_transform_mode;
using xray::collision::loose_oct_tree;


struct transform_control_rotation_internal : private boost::noncopyable
{
	transform_control_rotation_internal()
	:m_resulting_transform		( float4x4().identity() )
	{
	}

	float4x4							m_translation_transform;
	float4x4							m_resulting_transform;
	float4x4							m_draw_transform;
	float4x4							m_scale_transform;
	float3								m_rolling_axis;
	float3								m_saved_origin;
	float3								m_drag_start;
	float3								m_curr_axis_vector;
	float3								m_start_radius_vector;
	float3								m_start_plane_vector;
//	float3								m_curr_plane_vector;

};

/////////////////////////////  transform_control_rotation /////////////////////////////////////////////
transform_control_rotation::transform_control_rotation( xray::editor::level_editor^ le)
:super						( le ),
m_radius					( 4.5f ),
m_outer_radius_ratio		( 1.1f ),
m_inner_radius_ratio		( 0.95f ),
m_outer_radius				( m_outer_radius_ratio*m_radius ),
m_inner_radius				( m_inner_radius_ratio*m_radius ),
m_last_pos_segment_step		( xray::math::pi/16 ),
m_outer_ring				( NULL ),
m_current_angle				( 0 ),
m_initialized				( false ),
m_rolling_mode				( false ),
m_rolling_first_step		( false ),
m_data						(NEW (transform_control_rotation_internal) )
{
	m_control_id				= "rotation";
}

transform_control_rotation::~transform_control_rotation()
{
	destroy	( );
	DELETE (m_data);
}

void transform_control_rotation::initialize		( )
{
	if( m_initialized )
	{
		ASSERT( !m_initialized );
		return;
	}
	super::initialize							();
	float4x4 tmp_transform;
	tmp_transform.identity();

	vectora< math::float3 >						vertices		(g_allocator);
	vectora< u16 >								render_indices	(g_allocator);

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

	xray::non_null<collision::geometry>::ptr geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	get_geometries().push_back( &*geom );

	collision_object_transform_control* tem_obj;

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_y );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_x );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_z );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls );
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
	geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)( &*geom, this, enum_transform_axis_cam );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls );
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
	geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)( &*geom, this, enum_transform_axis_all );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls );
	get_collision_objects().push_back( tem_obj );

	m_initialized = true;
}

void transform_control_rotation::destroy		( )
{
	if( !m_initialized )
		return;
	
	super::destroy		();

	m_initialized = false;
}

void transform_control_rotation::resize		( float size )
{
	ASSERT( size >=  0, "Size must be greater or equal to 0" );

	bool visible = m_control_visible;
	if( visible )
		show		( false );

	bool initialized = m_initialized;

	if( initialized )
		destroy		( );

	m_radius			= size;
	m_outer_radius		= m_outer_radius_ratio * m_radius;
	m_inner_radius		= m_inner_radius_ratio * m_radius;
	
	if( initialized )
		initialize	( );

	update		();

	if( visible )
		show		( true );
}

void transform_control_rotation::update			( )
{
	super::update				();

	if( m_initialized && m_control_visible )
	{
		float3 vector_to_object = ( get_view_transform().c.xyz() - get_transform().c.xyz() ).normalize( );
		float4x4 trans			= create_rotation( (vector_to_object^get_view_transform().i.xyz()).normalize(), vector_to_object );

		// This is needed to compensate global tranformation rotations.
		float4x4 trans_inv		= invert4x3( create_rotation_INCORRECT( get_transform().get_angles_xyz_INCORRECT() ));

		m_outer_ring->set_matrix_original( trans*trans_inv );
	}
}

void transform_control_rotation::set_transform		( float4x4 const& transform )
{
	// Rotation control ignores the object scaling 
	m_data->m_translation_transform	= create_translation	( transform.c.xyz() );
	float4x4  rotation_transform	= create_rotation_INCORRECT	( transform.get_angles_xyz_INCORRECT() );
	m_data->m_scale_transform		= transform * invert4x3	( rotation_transform* m_data->m_translation_transform );
	float4x4 tmp_tranform			= rotation_transform *	m_data->m_translation_transform;
	super::set_transform			( tmp_tranform );
	m_data->m_draw_transform		= tmp_tranform;
}

void transform_control_rotation::start_input	( )
{
	super::start_input	();
	m_dragging = true;
	m_preview_mode = true;

	if( m_plane_mode_modifier || m_free_mode_modifier )
		m_rolling_mode = true;
	else
		m_rolling_mode = false;

	reset_colors();
	get_color(m_active_axis) = selection_color();

	float3 mouse_origin;
	float3 mouse_direction;
	m_current_angle = 0.f;

	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );

	m_data->m_saved_origin = get_transform().c.xyz();

	m_data->m_curr_axis_vector = create_rotation_axis( get_transform() );

	float4x4 control_tranform = calculate_fixed_size_transform( get_transform(), m_distance_from_cam, get_view_transform());
	float3 control_position = control_tranform.c.xyz();

	if( !m_rolling_mode )
	{
		float radius = (m_active_axis != enum_transform_axis_all) ? m_radius : m_inner_radius;

		if( m_active_axis == enum_transform_axis_cam )
			radius = m_outer_radius;

		radius *= calculate_fixed_size_scale	( get_transform().c.xyz(), m_distance_from_cam, get_view_transform() );

		math::sphere axes_sphere( control_position, radius );

		float	intersec_points[2];
		u32		intersec_points_count;
		math::intersection intersec_result = axes_sphere.intersect_ray( mouse_origin, mouse_direction, 1000.f, intersec_points_count, intersec_points );

		if( intersec_result == math::intersection_outside )
		{
			const float3 intersect_point = intersec_points[0]*mouse_direction+mouse_origin;
			m_data->m_start_radius_vector = (intersect_point-control_position);

		}
		else
		{
			float3 inter_point;
			bool res = plane_ray_intersect( m_data->m_saved_origin, mouse_direction, mouse_origin, mouse_direction, inter_point );
			ASSERT_U( res, "The plane must have intersection with the ray." );

			inter_point = (inter_point - m_data->m_saved_origin ).normalize();
			m_data->m_start_radius_vector = inter_point*radius;
		}


		if( m_active_axis != enum_transform_axis_all )
		{
			float dot = m_data->m_curr_axis_vector.dot_product( m_data->m_start_radius_vector );
			m_data->m_start_plane_vector = m_data->m_start_radius_vector - dot* m_data->m_curr_axis_vector;
			m_data->m_start_plane_vector.normalize();
			float dot1 = m_data->m_curr_axis_vector.dot_product( m_data->m_start_plane_vector );
			dot1 = dot1 ;
		}
		else
		{
			m_data->m_resulting_transform.identity();
		}
	}
	else
	{
		bool res = plane_ray_intersect( control_position, get_view_transform().k.xyz(), mouse_origin, mouse_direction, m_data->m_drag_start );
		ASSERT_U( res, "The plane must have intersection with the ray." );

		m_rolling_first_step = true;
	}
}

void transform_control_rotation::execute_input		( )
{
	float3 mouse_origin;
	float3 mouse_direction;
	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );

	float4x4 control_tranform = calculate_fixed_size_transform( get_transform(), m_distance_from_cam, get_view_transform());
	
	float3 control_position = control_tranform.c.xyz();

	float radius = (m_active_axis != enum_transform_axis_all) ? m_radius : m_inner_radius;

	if( m_active_axis == enum_transform_axis_cam )
		radius = m_outer_radius;

	radius *= calculate_fixed_size_scale	( get_transform().c.xyz(), m_distance_from_cam, get_view_transform() );


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
			bool res = plane_ray_intersect( m_data->m_saved_origin, mouse_direction, mouse_origin, mouse_direction, inter_point );
			ASSERT_U( res, "The plane must have intersection with the ray." );

			inter_point = (inter_point - m_data->m_saved_origin ).normalize();
			radius_vector = inter_point*radius;
		}

		if( m_active_axis == enum_transform_axis_all )
		{
			m_data->m_curr_axis_vector = radius_vector^m_data->m_start_radius_vector.normalize();

			float curr_axis_magnitude = m_data->m_curr_axis_vector.magnitude();
			if( math::is_zero( curr_axis_magnitude ) )
				return;

			m_data->m_curr_axis_vector /= curr_axis_magnitude;

			m_data->m_start_plane_vector = m_data->m_start_radius_vector - m_data->m_curr_axis_vector.dot_product( m_data->m_start_radius_vector )* m_data->m_curr_axis_vector;
			m_data->m_start_plane_vector.normalize();
		}

		float3 curr_plane_vector = radius_vector - m_data->m_curr_axis_vector.dot_product( radius_vector ) * m_data->m_curr_axis_vector;
		curr_plane_vector.normalize();

		float3 normal_vector = m_data->m_start_plane_vector^curr_plane_vector;
		float normal_vector_magnitude = normal_vector.magnitude();

		if( math::is_zero( normal_vector_magnitude ) )
			return;

		normal_vector		/= normal_vector_magnitude;

		float direction		= -normal_vector|m_data->m_curr_axis_vector;

		float acosine = m_data->m_start_plane_vector | curr_plane_vector;

		ASSERT( math::valid( acosine ) );

		math::clamp( acosine, -1.f, 1.f );
		m_current_angle = direction*math::acos( acosine );
	}
	else
	{
		float3 curr_pos;
		bool res = plane_ray_intersect( control_position, get_view_transform().k.xyz(), mouse_origin, mouse_direction, curr_pos );
		ASSERT_U( res, "The plane must have intersection with the ray." );

		float3 drag_vector = curr_pos - m_data->m_drag_start;
	
		float drag_vector_magnitude = drag_vector.magnitude();
		if( math::is_zero( drag_vector_magnitude ) )
			return;
		
		float3 projected_vector;
		if( m_rolling_first_step )
		{
			m_rolling_first_step = false;
			m_data->m_rolling_axis = drag_vector/drag_vector_magnitude;
			projected_vector = drag_vector; 
		}
		else
		{
			projected_vector = m_data->m_rolling_axis*( m_data->m_rolling_axis | drag_vector );
		}
		

		m_current_angle = ( projected_vector | m_data->m_rolling_axis ) / radius*math::pi_d2;
	}

// 	float4x4 rotation	= create_rotation( m_curr_axis_vector,  m_current_angle );
// 	//This is needed to have the real transformation of an object
// 	float4x4 translation = create_translation( m_transform.c.xyz() );
// 	// 
// 	float4x4 rotation_tranform = invert4x3( translation )*rotation*translation;
// 
// 	if( m_active_axis == enum_transform_axis_all && !m_rolling_mode )
// 	{
// 		m_start_radius_vector = radius_vector;
// 		m_resulting_transform = m_resulting_transform*rotation_tranform;
// 	}
// 	else
// 		m_resulting_transform = rotation_tranform;
// 
// 	m_draw_transform	= m_transform*m_resulting_transform;


	

	float4x4 self_rotation = create_rotation_INCORRECT( get_transform().get_angles_xyz_INCORRECT() );
	float3 m_local_vector = math::invert4x3( self_rotation ).transform_direction( m_data->m_curr_axis_vector );
	float4x4 rotation	= create_rotation( m_local_vector,  m_current_angle );
	
	float4x4 rotation_tranform = /*math::invert4x3( self_rotation )**/rotation;
	
	if( m_active_axis == enum_transform_axis_all && !m_rolling_mode )
	{
		m_data->m_start_radius_vector = radius_vector;
		m_data->m_resulting_transform = m_data->m_resulting_transform*rotation_tranform;//*math::invert4x3(self_rotation);
	}
	else
		m_data->m_resulting_transform = rotation_tranform;//*math::invert4x3(self_rotation);

	m_data->m_draw_transform	= create_object_transform( get_transform(), enum_transform_mode_local );

	if( m_apply_command == nullptr )
	{
		m_apply_command = gcnew command_apply_control_transform(m_level_editor, this);
		m_level_editor->get_command_engine()->run_preview( m_apply_command );
	}
	else
		m_level_editor->get_command_engine()->preview();
}

void transform_control_rotation::end_input	( )
{
	super::end_input	();
	m_dragging = false;

	set_transform		( m_data->m_draw_transform );
	m_data->m_draw_transform	= get_transform();

	if( m_apply_command )
	{
		m_level_editor->get_command_engine()->end_preview();
		m_apply_command = nullptr;
	}

	m_data->m_resulting_transform.identity();
	m_current_angle = 0.f;
	m_preview_mode = false;
}

float4x4 transform_control_rotation::create_object_transform	( float4x4 const& object_tranform, enum_transform_mode mode )
{
	float4x4 origin_tranform;
	if( mode == enum_transform_mode_local )
		origin_tranform = object_tranform;
	else
		origin_tranform = get_transform();

	float4x4 translation	= create_translation( origin_tranform.c.xyz() );

	float4x4 rotation_to_apply;
	if( /*false && */m_active_axis != enum_transform_axis_all )
	{
		float3 axis = create_rotation_axis( origin_tranform );
		rotation_to_apply = create_rotation( axis, m_current_angle );
		return math::float4x4	( object_tranform*math::invert4x3(translation)*rotation_to_apply*translation );
	}
	else
	{
		float4x4 rotation		= create_rotation_INCORRECT( origin_tranform.get_angles_xyz_INCORRECT() );
		return math::float4x4	( object_tranform*math::invert4x3(rotation*translation)*m_data->m_resulting_transform*rotation*translation );
	}
}

void transform_control_rotation::collide	( collision_object const* object, float3 const&/* position*/ )	
{
	collision_object_transform_control const * transform_control_collision = static_cast_checked<collision_object_transform_control const*>(object);
	ASSERT(transform_control_collision->get_owner() == this);
	ASSERT( transform_control_collision->get_axis() >= enum_transform_axis_cam && 
			transform_control_collision->get_axis() <= enum_transform_axis_none);

	enum_transform_axis mode = transform_control_collision->get_axis();
	
	if( !(mode == enum_transform_axis_all && m_free_mode_modifier) )
	{
		set_active_axis( mode );
	}
}

void transform_control_rotation::set_active_axis	( enum_transform_axis mode )
{
	m_active_axis =  mode;
}


void transform_control_rotation::draw					( xray::math::float4x4 const& view_matrix )
{
	m_level_editor->get_editor_renderer().flush();

	if( !m_control_visible )
		return;

	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz(), m_distance_from_cam, view_matrix ) < 0 )
		return;

	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	if( m_draw_collision_geomtery )
	{
		u32 count = get_collision_objects().size();
		for( u32 i = 0; i < count; ++i)
			get_collision_objects()[i]->render( rend.debug() );
	}
	float4x4 draw_transform = calculate_fixed_size_transform	( m_data->m_draw_transform, m_distance_from_cam, view_matrix );
	float4x4 transform		= calculate_fixed_size_transform	( m_data->m_translation_transform, m_distance_from_cam, view_matrix );

	float3 pos_to_view = float3( view_matrix.c.x, view_matrix.c.y, view_matrix.c.z)-draw_transform.c.xyz();

	pos_to_view.normalize( );
	float4x4 rotate_to_view = create_rotation( (pos_to_view^view_matrix.j.xyz()).normalize(), pos_to_view );

	if( m_dragging )
		draw_last_position	( view_matrix, false );

	draw_inner_outer_lines( transform, rotate_to_view, false );

	rend.flush();

	//////////////////////////////////////////////////////////////////////////
	// Draw invisible sphere
	rend.setup_rotation_control_modes( false);
	rend.draw_ellipsoid_solid( transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color_argb( 0, 0, 0, 0 ) );
	rend.flush();

	//////////////////////////////////////////////////////////////////////////
	// Reset rotation control mode to draw axis lines normally
	rend.setup_rotation_control_modes( true);
	// Drawing axes'lines
	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, false );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, false );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, false );
	rend.flush( );

	rend.clear_zbuffer( 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// Draw invisible sphere
	rend.setup_rotation_control_modes( false);
	rend.draw_ellipsoid_solid( transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color_argb( 0, 0, 0, 0 ) );
	rend.flush();

	//////////////////////////////////////////////////////////////////////////
	// Draw pattern lines
	rend.setup_rotation_control_modes( true);
	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, true );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, true );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, true );
	rend.flush();

	rend.clear_zbuffer( 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// Draw inner and outer strip lines 
	draw_inner_outer_lines( transform, rotate_to_view, true );

	if( m_dragging )
		draw_last_position	( view_matrix, true );
	rend.flush();

	/*
	m_level_editor->get_editor_renderer().flush();

	if( !m_visible )
		return;

	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz(), m_distance_from_cam, view_matrix ) < 0 )
		return;

	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	if( m_draw_collision_geomtery )
	{
		u32 count = get_collision_objects().size();
		for( u32 i = 0; i < count; ++i)
			get_collision_objects()[i]->render( rend.debug() );
	}
	float4x4 draw_transform = calculate_fixed_size_transform	( m_data->m_draw_transform, m_distance_from_cam, view_matrix );
	float4x4 transform		= calculate_fixed_size_transform	( m_data->m_translation_transform, m_distance_from_cam, view_matrix );

	float3 pos_to_view = float3( view_matrix.c.x, view_matrix.c.y, view_matrix.c.z)-draw_transform.c.xyz();

	pos_to_view.normalize( );
	float4x4 rotate_to_view = create_rotation( (pos_to_view^view_matrix.j.xyz()).normalize(), pos_to_view );

	if( m_dragging )
		draw_last_position	( view_matrix, false );

	draw_inner_outer_lines( transform, rotate_to_view, false );

	rend.flush();
	//////////////////////////////////////////////////////////////////////////
	// Setup mode 1 to draw invisible sphere that will write into depth buffer,
	// and so will clip back lines of the control.
	rend.setup_rotation_control_modes( 1, 0x02 );
	rend.draw_ellipsoid_solid( transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color_argb( 0, 0, 0, 0 ) );
	rend.flush();

	//////////////////////////////////////////////////////////////////////////
	// Reset rotation control mode to draw axis lines normally.
	rend.setup_rotation_control_modes( 0, 0x02 );
	// Drawing axes'lines

	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, false );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, false );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, false );

	rend.flush( );
	//////////////////////////////////////////////////////////////////////////
	// Setup mode 2 to render axis ellipses into stencil buffer by changing each stencil value by mode 3

	rend.setup_rotation_control_modes( 2, 0x02 );
	rend.setup_rotation_control_modes( 3, 0x02 );

	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, false );
	rend.flush( );

	rend.setup_rotation_control_modes( 3, 0x05 );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, false );
	rend.flush( );

	rend.setup_rotation_control_modes( 3, 0x08 );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, false );

	rend.flush( );

	//////////////////////////////////////////////////////////////////////////
	// Setup mode 4 to decreases stencil values of axes ellipses back lines to prevent their drawing.

	rend.setup_rotation_control_modes( 4, 0x00 );
	rend.draw_ellipsoid_solid( transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color_argb( 0, 0, 0, 0 ) );
	rend.flush( );

	//////////////////////////////////////////////////////////////////////////
	// Setup mode 5 to render screen strip lines using stencil buffer, which will clip 
	// backside lines.
	// each ellipise is draw using its stencil ref value.
	rend.setup_rotation_control_modes( 5, 0x02 );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, true );
	rend.flush( );

	rend.setup_rotation_control_modes( 5, 0x05 );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, true );
	rend.flush( );

	rend.setup_rotation_control_modes( 5, 0x08 );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, true );
	rend.flush();

	//////////////////////////////////////////////////////////////////////////
	// Reset rotation control mode.
	rend.setup_rotation_control_modes( 0, 0x02 );

	// Draw inner and outer strip lines 
	draw_inner_outer_lines( transform, rotate_to_view, true );

	if( m_dragging )
		draw_last_position	( view_matrix, true );
	rend.flush();
*/

}
void transform_control_rotation::draw_axis_ellipse		( xray::math::float4x4 const& draw_transform, float3 rotation, enum_transform_axis axis, bool strip_lines )
{
	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	// Drawing axes'lines
	float3 end_point				= draw_transform.transform_position( float3( 10, 0, 0 ));

	float4x4 ellipse_transform		= create_scale( float3( m_radius, m_radius, m_radius))*create_rotation_INCORRECT( rotation );

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( ellipse_transform*draw_transform, get_color(axis), 1.f, m_transparent_line_pattern );
	else
		rend.draw_ellipse	( ellipse_transform*draw_transform, get_color(axis) );

}
void transform_control_rotation::draw_inner_outer_lines ( xray::math::float4x4 const& translation_draw_transform,  xray::math::float4x4 const& rotate_to_view, bool strip_lines )
{
	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	float4x4 ellipse_transform = create_scale( float3( m_outer_radius, 1, m_outer_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_cam), 1.f, m_transparent_line_pattern );
	else
		rend.draw_ellipse	( ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_cam) );

	ellipse_transform = create_scale( float3( m_inner_radius, 1, m_inner_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_none), 1.f, m_transparent_line_pattern );
	else
		rend.draw_ellipse	( ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_none) );

}

void transform_control_rotation::draw_last_position			( xray::math::float4x4 const& view_matrix, bool transparent_mode )
{
	if( m_rolling_mode || m_active_axis == enum_transform_axis_all )
		return;

	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();
	
	if( transparent_mode )
	{
		rend.flush();
		rend.setup_grid_render_mode( 50 );
	}

	float4x4		transform	= calculate_fixed_size_transform (	get_transform(), 
																	m_distance_from_cam, 
																	view_matrix );		

	float radius		= (m_active_axis == enum_transform_axis_cam) ? m_outer_radius : m_radius;
	float distance_scale = calculate_fixed_size_scale	( get_transform().c.xyz(), m_distance_from_cam, get_view_transform() );
	radius *= distance_scale;

	float3 start_vector	= m_data->m_start_plane_vector*radius;
	float3 end_vector	= create_rotation( m_data->m_curr_axis_vector, m_current_angle ).transform_position( m_data->m_start_plane_vector*radius );
	
	float3 origin		= transform.c.xyz();
	float3 start_point	= transform.c.xyz()+start_vector;
	float3 end_point	= transform.c.xyz()+end_vector;

	//if( !transparent_mode )
	//{
	//	rend.draw_line	(	origin, end_point, last_position_line_color() );
	//	rend.draw_line	( origin, start_point, last_position_line_color() );

	//	return;
	//}

	rend.draw_3D_screen_line ( origin, start_point, last_position_line_color(), 1.f, m_transparent_line_pattern);
	rend.draw_3D_screen_line ( origin, end_point, last_position_line_color(), 1.f, m_transparent_line_pattern);

	const float3 vector_to_object	= ( view_matrix.c.xyz()-origin ).normalize( );
	float4x4	rotate_to_view		= create_rotation( (vector_to_object^view_matrix.j.xyz()).normalize(), vector_to_object );


	float4x4 identity;
	identity.identity();

	rend.draw_3D_screen_point	( origin,		m_last_position_vertex_radius, last_position_line_color() );
	rend.draw_3D_screen_point	( end_point,	m_last_position_vertex_radius, last_position_line_color() );
	rend.draw_3D_screen_point	( start_point,	m_last_position_vertex_radius, last_position_line_color() );

	float4x4 step_rotation = create_rotation( m_data->m_curr_axis_vector, m_last_pos_segment_step );

	float a = m_last_pos_segment_step;
	float end = m_current_angle;

	if( m_current_angle < 0 )
	{
		a += m_current_angle;
		end = 0;
		std::swap( start_vector, end_vector );
	}

	rend.flush();
	rend.setup_grid_render_mode( 25 );

	float3 prev_vertex = start_vector;
	bool const backface = (m_data->m_curr_axis_vector | ( view_matrix.c.xyz() - origin )) > 0;

	color transparent_color = last_position_line_color();

	for( ; a< end ; a+= m_last_pos_segment_step )
	{
		float3 curr_vertex = step_rotation.transform_position( prev_vertex );

		if( !backface )
		{
			rend.draw_triangle		(	origin, 
										prev_vertex+origin, 
										curr_vertex+origin, 
										transparent_color );	
		}
		else
		{
			rend.draw_triangle		(	origin, 
				curr_vertex+origin, 
				prev_vertex+origin, 
				transparent_color );	
		}
		prev_vertex = curr_vertex;
	}

	if( !backface )
		rend.draw_triangle		( origin, prev_vertex+origin, end_vector+origin, transparent_color );	
	else
		rend.draw_triangle		( origin, end_vector+origin, prev_vertex+origin, transparent_color );	

	if( transparent_mode )
	{
		rend.flush();
		rend.remove_grid_render_mode();
	}
}

float3 transform_control_rotation::create_rotation_axis( float4x4 transform )
{
	float3 rotation_vector;

	if( m_active_axis == enum_transform_axis_cam )
	{
		rotation_vector = (get_view_transform().c.xyz() - transform.c.xyz() ).normalize();
	}
	else if( m_active_axis != enum_transform_axis_all || m_rolling_mode )
	{
		rotation_vector = get_axis_vector( m_active_axis );
		rotation_vector = transform.transform_direction( rotation_vector );
		rotation_vector.normalize();
	}

	return rotation_vector;
}

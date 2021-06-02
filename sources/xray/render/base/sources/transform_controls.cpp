////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_controls.h"
#include <xray/render/base/editor_renderer.h>
#include <xray/render/base/platform.h>
#include <xray/render/base/detail_world.h>

#include "debug_renderer.h"

using xray::float3;
using xray::float4x4;
using xray::render::base::editor::detail::transform_control_base;
using xray::render::base::editor::detail::translation_control;
using xray::render::base::editor::detail::scaling_control;
using xray::render::base::editor::detail::rotation_control;

transform_control_base::transform_control_base ( xray::render::base::debug::renderer& helper_renderer ):
	m_helper_renderer	( helper_renderer ),
	m_x_axis_color		( math::color_xrgb(	255,	0,		0) ),
	m_y_axis_color		( math::color_xrgb(	0,		255,	0) ),
	m_z_axis_color		( math::color_xrgb(	255,	255,	0) ),
	m_center_color		( math::color_xrgb(	255,	255,	255) )
{
	(void) m_transform.identity();
}

transform_control_base::~transform_control_base	( )
{
}

void transform_control_base::set_transform		( const float4x4& transform)
{
	m_transform = transform;
}
const float4x4& transform_control_base::get_transform ( )
{
	return m_transform;
}

void transform_control_base::set_view_matrix	( const float4x4& transform )
{
	m_view = transform;
}

float4x4 transform_control_base::calculate_fixed_size_transform	( )
{
	float4x4 result;
	float3 start_point ( 0, 0, 0 );
	start_point = m_transform.transform_position( start_point );

	float3 distance = (float3( m_view.c.x, m_view.c.y, m_view.c.z)-start_point);
	result = m_transform*create_translation( (distance.magnitude()-20)*distance.normalize());

	return result;
}

translation_control::translation_control		( xray::render::base::debug::renderer& helper_renderer ):
	transform_control_base( helper_renderer),
	m_arrow_size		( 5.f ),
	m_arrow_head_length	( 0.4f ),
	m_arrow_head_radius	( 0.15f ),
	m_plane_move_mode	( enum_move_plane_camrea )
{

}

translation_control::~translation_control		( )
{
}

void translation_control::draw					( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

	// Drawing translation control 
	float3 start_point ( 0, 0, 0 );
	start_point = transform.transform_position( start_point );

	// Drawing axes' arrow lines 
	float3 end_point	= transform.transform_position( float3( m_arrow_size, 0, 0 ));
	m_helper_renderer.draw_line		( start_point, end_point, m_x_axis_color);

	end_point			= transform.transform_position( float3( 0, m_arrow_size, 0 ));
	m_helper_renderer.draw_line		( start_point, end_point, m_y_axis_color);

	end_point			= transform.transform_position( float3( 0, 0, -m_arrow_size ));
	m_helper_renderer.draw_line		( start_point, end_point, m_z_axis_color);


	// Drawing arrows heads as cones 
	//
	// Drawing y axis cone.
	float4x4 cone_transform = create_translation( float3 ( 0, m_arrow_size-m_arrow_head_length, 0 ));
	m_helper_renderer.draw_cone_solid	(	cone_transform*transform, 
										float3( m_arrow_head_radius, m_arrow_head_length, m_arrow_head_radius ), 
										m_y_axis_color);

	// Drawing x axis cone.
	cone_transform = cone_transform*create_rotation( float3( 0, 0, math::deg2rad(-90)));
	m_helper_renderer.draw_cone_solid		(	cone_transform*transform, 
										float3( m_arrow_head_radius, m_arrow_head_length, m_arrow_head_radius ), 
										m_x_axis_color);

	// Drawing z axis cone.
	cone_transform = cone_transform*create_rotation( float3( 0, math::deg2rad(90), 0 ));
	m_helper_renderer.draw_cone_solid		(	cone_transform*transform, 
										float3( m_arrow_head_radius, 
										m_arrow_head_length, m_arrow_head_radius ), 
										m_z_axis_color);

	float4x4 rotate_to_view;
	if( m_plane_move_mode == enum_move_plane_camrea )
	{
		float3 vector_to_object =			( float3( m_view.c.x, m_view.c.y, m_view.c.z )-start_point ).normalize( );
		rotate_to_view = create_rotation( m_view.i.xyz(), vector_to_object );
	}
	else if ( m_plane_move_mode == enum_move_plane_xy )
	{
		rotate_to_view = math::create_rotation_x(  math::deg2rad(90) );
	}
	else if ( m_plane_move_mode == enum_move_plane_yz )
	{
		rotate_to_view = math::create_rotation_z(  math::deg2rad(90) );
	}
	else if ( m_plane_move_mode == enum_move_plane_zx )
	{
		rotate_to_view = rotate_to_view.identity();
	}

	m_helper_renderer.draw_rectangle	( rotate_to_view*transform, 
		float3( 2*m_arrow_head_radius, 2*m_arrow_head_radius, 2*m_arrow_head_radius ), 
		m_center_color );
}

void	translation_control::draw_collision_object	( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

	// Drawing axes' cylinders.
	float4x4 cylinder_transform = create_translation( float3( 0, (m_arrow_size-m_arrow_head_length)/2, 0) );

	// Drawing y axis.
	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size-m_arrow_head_length)/2, m_arrow_head_radius/2), 
		m_y_axis_color );

	// Drawing x axis.
	cylinder_transform = cylinder_transform*create_rotation( float3( 0, 0, math::deg2rad(-90)));

	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size-m_arrow_head_length)/2, m_arrow_head_radius/2), 
		m_x_axis_color );

	// Drawing z axis.
	cylinder_transform = cylinder_transform*create_rotation( float3( 0, math::deg2rad(90), 0));

	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size-m_arrow_head_length)/2, m_arrow_head_radius/2), 
		m_z_axis_color );


	// Drawing arrows heads as cones 
	//
	// Drawing y axis cone.
	float4x4 cone_transform = create_translation( float3 ( 0, m_arrow_size-m_arrow_head_length, 0 ));
	m_helper_renderer.draw_cone_solid	(	cone_transform*transform, 
		float3( m_arrow_head_radius, m_arrow_head_length, m_arrow_head_radius ), 
		m_y_axis_color);

	// Drawing x axis cone.
	cone_transform = cone_transform*create_rotation( float3( 0, 0, math::deg2rad(-90)));
	m_helper_renderer.draw_cone_solid		(	cone_transform*transform, 
		float3( m_arrow_head_radius, m_arrow_head_length, m_arrow_head_radius ), 
		m_x_axis_color);

	// Drawing z axis cone.
	cone_transform = cone_transform*create_rotation( float3( 0, math::deg2rad(90), 0 ));
	m_helper_renderer.draw_cone_solid		(	cone_transform*transform, 
		float3( m_arrow_head_radius, 
		m_arrow_head_length, m_arrow_head_radius ), 
		m_z_axis_color);

	const float3 start_point = transform.transform_position( float3( 0, 0, 0 ) );

	float4x4 rotate_to_view;
	if( m_plane_move_mode == enum_move_plane_camrea )
	{
		float3 vector_to_object =			( float3( m_view.c.x, m_view.c.y, m_view.c.z )-start_point ).normalize( );
		rotate_to_view = create_rotation( m_view.i.xyz(), vector_to_object );
	}
	else if ( m_plane_move_mode == enum_move_plane_xy )
	{
		rotate_to_view = math::create_rotation_x(  math::deg2rad(90) );
	}
	else if ( m_plane_move_mode == enum_move_plane_yz )
	{
		rotate_to_view = math::create_rotation_z(  math::deg2rad(90) );
	}
	else if ( m_plane_move_mode == enum_move_plane_zx )
	{
		rotate_to_view = rotate_to_view.identity();
	}

	m_helper_renderer.draw_rectangle_solid( rotate_to_view*transform, 
		float3( 2*m_arrow_head_radius, 2*m_arrow_head_radius, 2*m_arrow_head_radius ), 
		m_center_color );

}


scaling_control::scaling_control	( xray::render::base::debug::renderer& helper_renderer ):
transform_control_base( helper_renderer),
m_arrow_size		( 5.f ),
m_arrow_head_radius	( 0.15f )
{
}

scaling_control::~scaling_control	( )
{
}

void scaling_control::draw				( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

	// Drawing scale control 
	float3 start_point ( 0, 0, 0 );
	start_point = transform.transform_position( start_point );

	// Drawing axes'lines
	float3 end_point	= transform.transform_position( float3( m_arrow_size, 0, 0 ));
	m_helper_renderer.draw_line		( start_point, end_point, m_x_axis_color);

	end_point			= transform.transform_position( float3( 0, m_arrow_size, 0 ));
	m_helper_renderer.draw_line		( start_point, end_point, m_y_axis_color);

	end_point			= transform.transform_position( float3( 0, 0, -m_arrow_size ));
	m_helper_renderer.draw_line		( start_point, end_point, m_z_axis_color);

	// Drawing arrows heads as cubes 
	//

	// Drawing z axis cone.
	float4x4 cube_transform = create_translation( float3 ( m_arrow_size-m_arrow_head_radius, 0, 0 ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
									float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
									m_x_axis_color );

	// Drawing y axis cone.
	cube_transform = create_translation	( float3 ( 0, m_arrow_size-m_arrow_head_radius, 0 ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
									float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
									m_y_axis_color );

	// Drawing y axis cone.
	cube_transform = create_translation	( float3 ( 0, 0, -(m_arrow_size-m_arrow_head_radius) ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
									float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
									m_z_axis_color );

	m_helper_renderer.draw_cube_solid	(	transform, 
									float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
									m_center_color );
}

void scaling_control::draw_collision_object		( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

	// Drawing axes' cylinders.
	float4x4 cylinder_transform = create_translation( float3( 0, (m_arrow_size-m_arrow_head_radius)/2, 0) );

	// Drawing y axis.
	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size)/2-m_arrow_head_radius, m_arrow_head_radius/2), 
		m_y_axis_color );

	// Drawing x axis.
	cylinder_transform = cylinder_transform*create_rotation( float3( 0, 0, math::deg2rad(-90)));

	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size)/2-m_arrow_head_radius, m_arrow_head_radius/2), 
		m_x_axis_color );

	// Drawing z axis.
	cylinder_transform = cylinder_transform*create_rotation( float3( 0, math::deg2rad(90), 0));

	m_helper_renderer.draw_cylinder_solid(	cylinder_transform*transform, 
		float3( m_arrow_head_radius/2, (m_arrow_size)/2-m_arrow_head_radius, m_arrow_head_radius/2), 
		m_z_axis_color );


	// Drawing arrows heads as cubes 
	//

	// Drawing z axis cone.
	float4x4 cube_transform = create_translation( float3 ( m_arrow_size-m_arrow_head_radius, 0, 0 ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		m_x_axis_color );

	// Drawing y axis cone.
	cube_transform = create_translation	( float3 ( 0, m_arrow_size-m_arrow_head_radius, 0 ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		m_y_axis_color );

	// Drawing y axis cone.
	cube_transform = create_translation	( float3 ( 0, 0, -(m_arrow_size-m_arrow_head_radius) ));
	m_helper_renderer.draw_cube_solid	(	cube_transform*transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		m_z_axis_color );

	m_helper_renderer.draw_cube_solid	(	transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		m_center_color );
}

rotation_control::rotation_control	( xray::render::base::debug::renderer& helper_renderer ):
	super	( helper_renderer),
	m_radius( 4.5 ),
	m_outer_radius(5),
	m_inner_radius(4.0)
{
	m_helper_renderer.create_torus	( m_torus_vertexes, m_torus_indexes, m_radius, 0.25, 32, 32, m_y_axis_color);
	m_helper_renderer.create_torus	( m_torus_outer_vertexes, m_torus_outer_indexes, m_outer_radius, 0.25, 32, 32, m_y_axis_color);
}

rotation_control::~rotation_control	( )
{
}

void rotation_control::draw			( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

	// Drawing scale control 
	float3 start_point( 0, 0, 0 );
	start_point						= transform.transform_position( start_point );

	// Drawing axes'lines
	float3 end_point				= transform.transform_position( float3( 10, 0, 0 ));

	float4x4 ellipse_transform		= create_scale( float3( m_radius, m_radius, m_radius));
	m_helper_renderer.draw_ellipse	( ellipse_transform*transform, m_y_axis_color );

	ellipse_transform				= ellipse_transform*create_rotation( float3 ( 0, 0, math::deg2rad(-90)));
	m_helper_renderer.draw_ellipse	( ellipse_transform*transform, m_x_axis_color );

	ellipse_transform				= ellipse_transform*create_rotation( float3 ( 0, math::deg2rad(90), 0));
	m_helper_renderer.draw_ellipse	( ellipse_transform*transform, m_z_axis_color );

	start_point = float3( m_view.c.x, m_view.c.y, m_view.c.z)-start_point;

	float4x4 rotate_to_view = create_rotation( m_view.i.xyz(), (start_point).normalize( ));

	ellipse_transform = create_scale( float3( m_outer_radius, 1, m_outer_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	m_helper_renderer.draw_ellipse	( ellipse_transform*transform, math::color_xrgb( 255, 255, 255) );

	ellipse_transform = create_scale( float3( m_inner_radius, 1, m_inner_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	m_helper_renderer.draw_ellipse	( ellipse_transform*transform, math::color_xrgb( 0, 0, 0) );
}

void rotation_control::draw_collision_object	( )
{
	float4x4 transform = calculate_fixed_size_transform	( );

// 	m_helper_renderer.draw_triangles( m_torus_vertexes, m_torus_indexes);
// 	return;


	m_working_vertexes.resize( m_torus_vertexes.size() );


	// Draw y torus
	for( u32 i = 0; i< m_torus_vertexes.size(); ++i) 
	{
		m_working_vertexes[i].position = transform.transform_position( m_torus_vertexes[i].position);
		m_working_vertexes[i].color = m_y_axis_color;
	}
	m_helper_renderer.draw_triangles( m_working_vertexes, m_torus_indexes);

	// Draw x torus
	float4x4 temp_transform = create_rotation( float3( 0, 0, math::deg2rad(-90) ) )*transform;

	for( u32 i = 0; i< m_torus_vertexes.size(); ++i) 
	{
		m_working_vertexes[i].position = temp_transform.transform_position( m_torus_vertexes[i].position);
		m_working_vertexes[i].color = m_x_axis_color;
	}

	m_helper_renderer.draw_triangles( m_working_vertexes, m_torus_indexes);

	// Draw z torus
	temp_transform = create_rotation( float3( math::deg2rad(90), 0, 0 ) )*transform;

	for( u32 i = 0; i< m_torus_vertexes.size(); ++i) 
	{
		m_working_vertexes[i].position = temp_transform.transform_position( m_torus_vertexes[i].position);
		m_working_vertexes[i].color = m_z_axis_color;
	}

	m_helper_renderer.draw_triangles( m_working_vertexes, m_torus_indexes);

	
	// Drawing scale control 
	float3 start_point( 0, 0, 0 );
	start_point						= transform.transform_position( start_point );


	start_point = float3( m_view.c.x, m_view.c.y, m_view.c.z)-start_point;

	float4x4 rotate_to_view = create_rotation( m_view.i.xyz(), (start_point).normalize( ));

	rotate_to_view = rotate_to_view*transform;
	for( u32 i = 0; i< m_torus_outer_vertexes.size(); ++i) 
	{
		m_working_vertexes[i].position = rotate_to_view.transform_position( m_torus_outer_vertexes[i].position);
		m_working_vertexes[i].color = math::color_xrgb(	255, 255, 255);
	}

	m_helper_renderer.draw_triangles( m_working_vertexes, m_torus_outer_indexes);
	m_helper_renderer.draw_ellipsoid_solid( transform, float3( m_inner_radius, m_inner_radius, m_inner_radius), math::color_xrgb(	0, 0, 0) );
}
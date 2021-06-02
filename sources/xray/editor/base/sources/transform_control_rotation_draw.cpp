////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_rotation.h"

namespace xray{
namespace editor_base{

void transform_control_rotation::draw( render::scene_view_pointer scene_view, render::render_output_window_pointer output_window )
{
	if( !m_shown )
		return;

	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz() ) < 0 )
		return;

	render::editor::renderer& rend = get_editor_renderer();

	render::render_output_window_ptr output_window_ptr = output_window;
	render::scene_view_ptr scene_view_ptr = scene_view;
//	rend.flush					( scene_view_ptr, output_window_ptr, *m_viewport);

	float4x4 draw_transform = calculate_fixed_size_transform	( m_data->m_draw_transform );
	float4x4 transform		= calculate_fixed_size_transform	( m_data->m_translation_transform );

	float3 pos_to_view		= get_inv_view().c.xyz()-draw_transform.c.xyz();
	if(pos_to_view.length()>math::epsilon_3)
		pos_to_view.normalize( );
	else
		pos_to_view			= get_inv_view().k.xyz();

	float4x4 rotate_to_view = create_rotation( (pos_to_view^get_inv_view().j.xyz()).normalize(), pos_to_view );

	if( m_dragging ) {
		draw_last_position	(false );
	}

	draw_inner_outer_lines( transform, rotate_to_view, false );

//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);

	//////////////////////////////////////////////////////////////////////////
	// Draw invisible sphere
	rend.setup_rotation_control_modes( false);
	rend.debug().draw_ellipsoid_solid( render::scene_ptr( m_helper->m_scene ), transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color( 0, 0, 0, 0 ), false );
//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);

	//////////////////////////////////////////////////////////////////////////
	// Reset rotation control mode to draw axis lines normally
	rend.setup_rotation_control_modes( true);
	// Drawing axes'lines
	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, false );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, false );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, false );
//	rend.flush( scene_view_ptr, output_window_ptr, *m_viewport);

	rend.clear_zbuffer( 1.0f);
	
	//////////////////////////////////////////////////////////////////////////
	// Draw invisible sphere
	rend.setup_rotation_control_modes( false);
	rend.debug().draw_ellipsoid_solid( render::scene_ptr( m_helper->m_scene ), transform, float3( m_inner_radius, m_inner_radius, m_inner_radius ), math::color( 0, 0, 0, 0 ), false );
//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);

	//////////////////////////////////////////////////////////////////////////
	// Draw pattern lines
	rend.setup_rotation_control_modes( true);
	draw_axis_ellipse	( draw_transform, float3( 0, 0, math::deg2rad(-90) ),	enum_transform_axis_x, true );
	draw_axis_ellipse	( draw_transform, float3( 0, 0, 0 ),	enum_transform_axis_y, true );
	draw_axis_ellipse	( draw_transform, float3( math::deg2rad(-90), 0, 0 ),	enum_transform_axis_z, true );
//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);

	rend.clear_zbuffer( 1.0f);

	//////////////////////////////////////////////////////////////////////////
	// Draw inner and outer strip lines 
	draw_inner_outer_lines( transform, rotate_to_view, true );

	if( m_dragging )
		draw_last_position	(true );

//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
}

void transform_control_rotation::draw_axis_ellipse( math::float4x4 const& draw_transform, float3 rotation, enum_transform_axis axis, bool strip_lines )
{
	xray::render::editor::renderer& rend = get_editor_renderer();

	// Drawing axes'lines
	float3 end_point				= draw_transform.transform_position( float3( 10, 0, 0 ));

	float4x4 ellipse_transform		= create_scale( float3( m_radius, m_radius, m_radius))*create_rotation( rotation );

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( render::scene_ptr(m_helper->m_scene), ellipse_transform*draw_transform, get_color(axis), 1.f, m_transparent_line_pattern );
	else
		rend.debug().draw_ellipse	( render::scene_ptr( m_helper->m_scene ), ellipse_transform*draw_transform, get_color(axis), false );

}
void transform_control_rotation::draw_inner_outer_lines( math::float4x4 const& translation_draw_transform, math::float4x4 const& rotate_to_view, bool strip_lines )
{
	render::editor::renderer& rend = get_editor_renderer();

	float4x4 ellipse_transform = create_scale( float3( m_outer_radius, 1, m_outer_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( render::scene_ptr(m_helper->m_scene), ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_cam), 1.f, m_transparent_line_pattern );
	else
		rend.debug().draw_ellipse	( render::scene_ptr( m_helper->m_scene ), ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_cam), false );

	if(m_picked_axis == enum_transform_axis_all)
	{
	ellipse_transform = create_scale( float3( m_inner_radius, 1, m_inner_radius));
	ellipse_transform = ellipse_transform*rotate_to_view;

	if( strip_lines )
		rend.draw_3D_screen_ellipse	( render::scene_ptr(m_helper->m_scene), ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_all), 1.f, m_transparent_line_pattern );
	else
		rend.debug().draw_ellipse	( render::scene_ptr( m_helper->m_scene ), ellipse_transform*translation_draw_transform, get_color(enum_transform_axis_all), false );
	}
}

void transform_control_rotation::draw_last_position( bool transparent_mode )
{
	if( m_rolling_mode || m_active_axis == enum_transform_axis_all )
		return;
	math::float4x4 const& inv_view_matrix = get_inv_view();

	render::editor::renderer& rend = get_editor_renderer();

	if( transparent_mode )
	{
//		rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
		rend.setup_grid_render_mode( 50 );
	}

	float4x4 transform		= calculate_fixed_size_transform(	get_matrix() );

	float radius			= (m_active_axis == enum_transform_axis_cam) ? m_outer_radius : m_radius;
	float distance_scale	= calculate_fixed_size_scale( get_matrix().c.xyz() );
	radius					*= distance_scale;

	float3 start_vector	= m_data->m_start_plane_vector*radius;
	float3 end_vector	= create_rotation( m_data->m_curr_axis_vector, m_current_angle ).transform_position( m_data->m_start_plane_vector*radius );
	
	float3 origin		= transform.c.xyz();
	float3 start_point	= transform.c.xyz()+start_vector;
	float3 end_point	= transform.c.xyz()+end_vector;

	rend.draw_3D_screen_line ( render::scene_ptr(m_helper->m_scene), origin, start_point, last_position_line_color(), 1.f, m_transparent_line_pattern);
	rend.draw_3D_screen_line ( render::scene_ptr(m_helper->m_scene), origin, end_point, last_position_line_color(), 1.f, m_transparent_line_pattern);

	const float3 vector_to_object	= ( inv_view_matrix.c.xyz()-origin ).normalize( );
	float4x4	rotate_to_view		= create_rotation( (vector_to_object^inv_view_matrix.j.xyz()).normalize(), vector_to_object );


	float4x4 identity;
	identity.identity();

	rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), origin,		m_last_position_vertex_radius, last_position_line_color() );
	rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), end_point,	m_last_position_vertex_radius, last_position_line_color() );
	rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), start_point,	m_last_position_vertex_radius, last_position_line_color() );

	float4x4 step_rotation = create_rotation( m_data->m_curr_axis_vector, m_last_pos_segment_step );

	float a = m_last_pos_segment_step;
	float end = m_current_angle;

	if( m_current_angle < 0 )
	{
		a += m_current_angle;
		end = 0;
		std::swap( start_vector, end_vector );
	}

//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
	rend.setup_grid_render_mode( 25 );

	float3 prev_vertex = start_vector;
	bool const backface = (m_data->m_curr_axis_vector | ( inv_view_matrix.c.xyz() - origin )) > 0;

	color transparent_color = last_position_line_color();

	for( ; a< end ; a+= m_last_pos_segment_step )
	{
		float3 curr_vertex = step_rotation.transform_position( prev_vertex );

		if( !backface )
		{
			rend.debug().draw_triangle		(
				render::scene_ptr( m_helper->m_scene ),
				origin, 
				prev_vertex+origin, 
				curr_vertex+origin, 
				transparent_color,
				false
			);	
		}
		else
		{
			rend.debug().draw_triangle		(
				render::scene_ptr( m_helper->m_scene ),
				origin, 
				curr_vertex+origin, 
				prev_vertex+origin, 
				transparent_color,
				false
			);	
		}
		prev_vertex = curr_vertex;
	}

	if( !backface )
		rend.debug().draw_triangle		( render::scene_ptr( m_helper->m_scene ), origin, prev_vertex+origin, end_vector+origin, transparent_color, false );	
	else
		rend.debug().draw_triangle		( render::scene_ptr( m_helper->m_scene ), origin, end_vector+origin, prev_vertex+origin, transparent_color, false );	

	if( transparent_mode )
	{
		rend.remove_grid_render_mode();
	}
}

} //namespace editor
} // namespace xray

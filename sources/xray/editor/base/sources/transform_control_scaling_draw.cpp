////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_scaling.h"

namespace xray{
namespace editor_base{

void transform_control_scaling::draw( render::scene_view_pointer scene_view, xray::render::render_output_window_pointer output_window)
{
	if( !m_shown )
		return;

	render::render_output_window_ptr output_window_ptr = output_window;
	render::scene_view_ptr scene_view_ptr = scene_view;
//	get_editor_renderer().flush( scene_view_ptr, output_window_ptr, *m_viewport );

	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz() ) < 0 )
		return;

	R_ASSERT	( m_helper->m_scene_view == scene_view );
	R_ASSERT	( m_helper->m_render_output_window == output_window );

	draw_impl	( false );
	draw_impl	( true );
}

void transform_control_scaling::draw_impl( bool transparent_mode )
{
	render::editor::renderer& rend = get_editor_renderer();

	float4x4 transform = calculate_fixed_size_transform	( m_data->m_draw_transform );

	// Drawing scale control 
	float3 start_point ( 0, 0, 0 );
	start_point = transform.transform_position( start_point );

	// Drawing axes'lines
	float3 end_point;
	math::color	const	line_clr( 100, 100, 100 );

	if( m_dragging)
		draw_last_position			( transparent_mode );
	
	end_point	= transform.transform_position( float3( m_data->m_draw_arrows_lengths.x, 0, 0 ));
	bool	x_axis_visible	= true;//!one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius );
			x_axis_visible	&= is_axis_visible(enum_transform_axis_x);

	if( x_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, line_clr, 1.f, m_transparent_line_pattern, false );
		else
			rend.debug().draw_line		( render::scene_ptr( m_helper->m_scene ), start_point, end_point, line_clr, false );
	}

	end_point			= transform.transform_position( float3( 0, m_data->m_draw_arrows_lengths.y, 0 ));
	bool	y_axis_visible	= true;//!one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius );
			y_axis_visible	&= is_axis_visible(enum_transform_axis_y);

	if( y_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, line_clr, 1.f, m_transparent_line_pattern, false );
		else
			rend.debug().draw_line		( render::scene_ptr( m_helper->m_scene ), start_point, end_point, line_clr, false );
	}

	end_point			= transform.transform_position( float3( 0, 0, m_data->m_draw_arrows_lengths.z ));
	bool	z_axis_visible	= true;//!one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius );
			z_axis_visible	&= is_axis_visible(enum_transform_axis_z);

	if( z_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, math::color(	100, 100, 100 ), 1.f, m_transparent_line_pattern, false );
		else
			rend.debug().draw_line		( render::scene_ptr( m_helper->m_scene ), start_point, end_point, math::color(	100, 100, 100 ), false );
	}


	// Drawing arrows heads as cubes 
	//
	float4x4 cube_transform;

	if( transparent_mode )
	{
//		rend.flush( render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
		rend.setup_grid_render_mode( 50 );
	}
	// Drawing x axis cube.
	if( x_axis_visible )
	{
		cube_transform = create_translation( float3 ( m_data->m_draw_arrows_lengths.x, 0, 0 ));
		rend.debug().draw_cube_solid	(
			render::scene_ptr( m_helper->m_scene ),
			cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_x),
			false
		);
	}

	// Drawing y axis cube.
	if( y_axis_visible )
	{
		cube_transform = create_translation	( float3 ( 0, m_data->m_draw_arrows_lengths.y, 0 ));
		rend.debug().draw_cube_solid	(
			render::scene_ptr( m_helper->m_scene ),
			cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_y),
			false
		);
	}

	// Drawing z axis cube.
	if( z_axis_visible )
	{
		cube_transform = create_translation	( float3 ( 0, 0, (m_data->m_draw_arrows_lengths.z) ));
		rend.debug().draw_cube_solid	(
			render::scene_ptr( m_helper->m_scene ),
			cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_z), false
		);
	}

	rend.debug().draw_cube_solid	(
		render::scene_ptr( m_helper->m_scene ),
		transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		get_color(enum_transform_axis_all),
		false
	);

	if( transparent_mode )
	{
//		rend.flush( render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
		rend.remove_grid_render_mode();
	}

//	rend.flush( render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
}

void transform_control_scaling::draw_last_position( bool transparent_mode )
{
	render::editor::renderer& rend = get_editor_renderer();
	
	math::float4x4 const& inv_view_matrix = get_inv_view();

	float4x4		transform	= calculate_fixed_size_transform( get_matrix() );

	float4x4		transform_translation = create_translation( transform.c.xyz() );

	float4x4		tmp_transform;
	float4x4		rotate_to_view		= create_rotation( inv_view_matrix.i.xyz(), -inv_view_matrix.k.xyz() );
	float3			start_point			= transform.c.xyz();

	float3			end_point;
	float4x4		translation;

	if( transparent_mode )
	{
//		rend.flush( render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
		rend.setup_grid_render_mode( 50 );
	}

	// Drawing axes' arrow lines 
	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_x )
	{
		translation		= create_translation( float3( m_arrow_length, 0, 0 ))*transform;

		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false );
		if( transparent_mode )
		{
			rend.draw_3D_screen_line( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern );
		}
		else
			rend.debug().draw_line	( render::scene_ptr( m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );
	}

	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_y )
	{
		translation		= create_translation( float3( 0, m_arrow_length, 0 ))*transform;
		
		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false );
		if( transparent_mode ) 
		{
			rend.draw_3D_screen_line( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
		}
		else
			rend.debug().draw_line	( render::scene_ptr( m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );
	}

	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_z )
	{
		translation		= create_translation( float3( 0, 0, m_arrow_length ))*transform;
		
		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false );
		if( transparent_mode ) 
		{
			rend.draw_3D_screen_line( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
		}
		else
			rend.debug().draw_line	( render::scene_ptr( m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );
	}

	//if (transparent_mode)
	rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), transform.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false );

	if( transparent_mode )
	{
//		rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
		rend.remove_grid_render_mode();
	}
//	rend.flush(render::scene_view_ptr( m_helper->m_scene_view ), render::render_output_window_ptr( m_helper->m_render_output_window ), *m_viewport);
}

} //namespace editor_base
} // namespace xray

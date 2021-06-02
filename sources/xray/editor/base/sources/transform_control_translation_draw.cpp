////////////////////////////////////////////////////////////////////////////
//	Created		: 18.03.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_translation.h"
#include "transform_control_object.h"
#include <xray/editor/base/collision_object_control.h>

namespace xray{
namespace editor_base{

void transform_control_translation::draw( render::scene_view_pointer scene_view, 
										 render::render_output_window_pointer output_window )
{
	if( !m_shown )
		return;

	render::render_output_window_ptr output_window_ptr	= output_window;
	render::scene_view_ptr scene_view_ptr				= scene_view;
	
	math::float4x4 const draw_transform		= calculate( get_matrix() );
	
	if( calculate_fixed_size_scale( draw_transform.c.xyz() ) < 0 )
		return;

	R_ASSERT				( m_helper->m_scene_view == scene_view );
	R_ASSERT				( m_helper->m_render_output_window == output_window );

	draw_impl				( false );
	draw_impl				( true );
}

void transform_control_translation::draw_impl( bool transparent_mode )
{
	render::editor::renderer& rend = get_editor_renderer();

	//float4x4 transform = calculate_fixed_size_transform	( create_translation(get_matrix().c.xyz()) );
	float4x4 const& transform = calculate_fixed_size_transform	( get_matrix() );


	// Drawing translation control 
	float3 start_point		= transform.c.xyz();

	if( m_dragging)
		draw_last_position	( transparent_mode );

	// Drawing axes' arrow lines 
	float3 end_point	= transform.transform_position( float3( m_arrow_size, 0, 0 ));

	bool x_axis_visible	= is_axis_visible(enum_transform_axis_x);

	math::color c = get_color(enum_transform_axis_x);
	if( x_axis_visible )
	{
		if( !transparent_mode)
			rend.debug().draw_line		( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, false );
		else
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, 1.f, m_transparent_line_pattern, false);
	}
			

	end_point				= transform.transform_position( float3( 0, m_arrow_size, 0 ));
	bool	y_axis_visible	= is_axis_visible(enum_transform_axis_y);

	c = get_color(enum_transform_axis_y);
	if( y_axis_visible )
		if( !transparent_mode)
			rend.debug().draw_line		( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, false);
		else
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, 1.f, m_transparent_line_pattern, false);



	end_point				= transform.transform_position( float3( 0, 0, m_arrow_size ));
	bool	z_axis_visible	= is_axis_visible(enum_transform_axis_z);
	
	c = get_color(enum_transform_axis_z);
	if( z_axis_visible )
	{
		if( !transparent_mode)
			rend.debug().draw_line		( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, false);
		else
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, end_point, c, 1.f, m_transparent_line_pattern, false);
	}

	// draw center box
	//float4x4 rotate_to_view = create_plane_box_rotation( m_active_plane, create_translation( transform.c.xyz() ) );
	float4x4 rotate_to_view = create_plane_box_rotation( m_active_plane, transform );
	rotate_to_view			= rotate_to_view*transform;
	//rotate_to_view			= rotate_to_view * create_translation(get_matrix().c.xyz());

	float box_side			= m_arrow_head_radius*2.0f;
	float3 box_size			= float3(box_side,box_side,box_side);
	
	c = get_color(enum_transform_axis_cam);
	
	bool bpivot = (m_helper->m_object->current_mode()==editor_base::transform_control_object::mode::edit_pivot);

	if( !transparent_mode)
	{
		if(bpivot)
			rend.debug().draw_ellipse( render::scene_ptr(m_helper->m_scene), create_scale(box_size)*rotate_to_view, c, false);
		else
			rend.debug().draw_rectangle	( render::scene_ptr(m_helper->m_scene), rotate_to_view, box_size, c, false );
	}else
	{		
		if(bpivot)
			rend.draw_3D_screen_ellipse( render::scene_ptr(m_helper->m_scene), create_scale(box_size)*rotate_to_view, c, 1.f, u32(-1), false );
		else
			rend.draw_3D_screen_rectangle ( render::scene_ptr(m_helper->m_scene), create_scale(box_size)*rotate_to_view, c, 1.f, m_transparent_line_pattern, false );
	}


	if( m_draw_arrow_heads )
	{
		if( transparent_mode )
		{
//			rend.flush					( render::scene_view_ptr(m_helper->m_scene_view), render::render_output_window_ptr( m_helper->m_render_output_window), *m_viewport );
			rend.setup_grid_render_mode	( 50 );
		}
		float3 heads_cone_size(m_arrow_head_radius, m_arrow_head_length/2, m_arrow_head_radius);
		
		// Drawing y axis cone.
		float4x4 cone_transform= create_translation( float3 ( 0, m_arrow_size+m_arrow_head_length/2, 0 ));

		if( y_axis_visible )
		{
			c				= get_color(enum_transform_axis_y);
			rend.debug().draw_cone_solid( render::scene_ptr(m_helper->m_scene), cone_transform*transform, heads_cone_size, c, false );
		}

		// Drawing x axis cone.
		if( x_axis_visible )
		{
			c = get_color(enum_transform_axis_x);
			rend.debug().draw_cone_solid( render::scene_ptr(m_helper->m_scene), cone_transform * math::create_rotation_z(math::pi_d2) * transform, heads_cone_size, c, false );
		}

		// Drawing z axis cone.
		if( z_axis_visible )
		{
			c = get_color(enum_transform_axis_z);
			rend.debug().draw_cone_solid( render::scene_ptr(m_helper->m_scene), cone_transform * math::create_rotation_x(-math::pi_d2) * transform, heads_cone_size, c, false );
		}

		if( transparent_mode )
		{
//			rend.flush(render::scene_view_ptr(m_helper->m_scene_view), render::render_output_window_ptr(m_helper->m_render_output_window), *m_viewport);
			rend.remove_grid_render_mode();
		}
	}
	//get_collision_objects().back()->render( render::scene_ptr(m_helper->m_scene), rend.debug() );
//	rend.flush( render::scene_view_ptr( m_helper->m_scene_view), render::render_output_window_ptr(m_helper->m_render_output_window), *m_viewport);
}

void transform_control_translation::draw_last_position( bool transparent_mode )
{
	render::editor::renderer& rend = get_editor_renderer();

	float4x4		transform	= calculate_fixed_size_transform( create_translation(get_matrix().c.xyz()) );		

	float3			start_point	= transform.c.xyz();

	float4x4		translation;
	float4x4		tmp_transform;

	if( transparent_mode )
	{
//		rend.flush(view_id, w, NULL);
		rend.setup_grid_render_mode( 50 );
	}

	// Drawing axes' arrow lines 
	if( m_plane_mode || m_active_axis == enum_transform_axis_x )
	{
		translation		= create_translation( float3( m_arrow_size, 0, 0 ))*transform;
		
		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false);
		if( transparent_mode )
		{
			rend.draw_3D_screen_line( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
		}
		else
			rend.debug().draw_line	( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );
	}

	if( m_plane_mode || m_active_axis == enum_transform_axis_y )
	{
 		translation		= create_translation( float3( 0, m_arrow_size, 0 ))*transform;

		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color(), false );
		if( transparent_mode )
		{
			rend.draw_3D_screen_line( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
		}
		else
			rend.debug().draw_line	( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );
	}

	if( m_plane_mode || m_active_axis == enum_transform_axis_z )
	{
 		translation		= create_translation( float3( 0, 0, m_arrow_size))*transform;

		rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), translation.c.xyz(), m_last_position_vertex_radius, last_position_line_color(), false );
		if( transparent_mode )
		{
			rend.draw_3D_screen_line	( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
		}
		else
			rend.debug().draw_line	( render::scene_ptr(m_helper->m_scene), start_point, translation.c.xyz(), last_position_line_color(), false );

	}

	rend.draw_3D_screen_point	( render::scene_ptr(m_helper->m_scene), transform.c.xyz(), m_last_position_vertex_radius, last_position_line_color(), false );

	if( transparent_mode )
	{
//		rend.flush	( view_id, w );
		rend.remove_grid_render_mode();
	}
//	rend.flush		( view_id, w );
}

} //namespace editor
} // namespace xray

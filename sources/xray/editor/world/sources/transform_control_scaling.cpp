////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_scaling.h"

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
using xray::editor::transform_control_scaling;
using xray::editor::transform_control_base;

using xray::editor::collision_object_transform_control;
using xray::editor::collision_object;
using xray::editor::enum_transform_axis;
using xray::editor::enum_transform_mode;
using xray::collision::space_partitioning_tree;

struct transform_control_scaling_internal
{
	transform_control_scaling_internal()
	:m_resulting_transform		( float4x4().identity() ),
	m_draw_arrows_lengths		( 4.0f, 4.0f, 4.0f ),
	m_scale_vector				( 0, 0, 0 )
	{}

	float3								m_drag_start;
	float3								m_curr_axis;

	float4x4							m_resulting_transform;
	float4x4							m_transform_original;
	float4x4							m_draw_transform;
	float3								m_draw_arrows_lengths;
	float3								m_scale_vector;

};

/////////////////////////////  transform_control_scaling /////////////////////////////////////////////
transform_control_scaling::transform_control_scaling( xray::editor::level_editor^ le ):
super						( le ),
m_arrow_length				( 4.f ),
m_arrow_head_radius			( 0.15f ),
m_center_box_drag_factor	( 12.f ),
m_initialized				( false ),
m_plane_mode				( true ),
m_data						(NEW (transform_control_scaling_internal) )
{
	m_control_id			= "scale";
}

transform_control_scaling::~transform_control_scaling()
{
	destroy	( );
	DELETE (m_data);
}

void transform_control_scaling::initialize		( )
{
	if( m_initialized )
	{
		ASSERT( !m_initialized );
		return;
	}

	super::initialize							();

	collision_object_transform_control* tem_obj;
	float4x4 tmp_transform;

	tmp_transform = tmp_transform.identity();

	vectora< math::float3 >						vertices			(g_allocator);
	vectora< u16 >								render_indices		(g_allocator);

	geometry_utils::create_cylinder( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_cursor_thickness, (m_arrow_length-m_arrow_head_radius)/2, m_cursor_thickness )	);
	//		float3( 2.5, 5, 2.5 )	);

	tmp_transform = create_translation( float3( 0, (m_arrow_length-m_arrow_head_radius)/2, 0 ));

	geometry_utils::create_cube( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_arrow_head_radius/2+m_cursor_thickness, m_arrow_head_radius/2+m_cursor_thickness, m_arrow_head_radius/2+m_cursor_thickness)	);

	tmp_transform = create_translation( float3( 0, m_arrow_length-m_arrow_head_radius/2, 0 ));

	vectora< u32 >			collision_indices		(*g_allocator);
	vectora< u16 >::const_iterator it				= render_indices.begin();
	vectora< u16 >::const_iterator it_e				= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	xray::non_null<collision::geometry>::ptr geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_y );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = create_translation( float3( m_arrow_length-m_arrow_head_radius/2, 0, 0 ));

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_x );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = create_translation( float3( 0, 0, m_arrow_length-m_arrow_head_radius/2 ));

	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_z );
	tem_obj->set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	// Create center box collision object
	tem_obj = NEW (collision_object_transform_control)( &(*geom), this, enum_transform_axis_all );
	tem_obj->set_matrix_original( tmp_transform.identity() );
	tem_obj->set_priority( collision_objects_priority_controls );
	get_collision_objects().push_back( tem_obj );

	m_initialized = true;
}

void transform_control_scaling::destroy		( )
{
	if( !m_initialized )
		return;

	super::destroy	();

	m_initialized = false;
}

void transform_control_scaling::resize		( float size )
{
	ASSERT( size >=  0, "Size must be greater or equal to 0" );

	bool visible = m_control_visible;
	if( visible )
		show		( false );

	bool initialized = m_initialized;
	if( initialized )
		destroy		( );

	m_arrow_length = size;
	m_data->m_draw_arrows_lengths = float3( m_arrow_length, m_arrow_length, m_arrow_length );

	if( initialized )		
		initialize	( );

	update		();

	if( visible )
		show		( true );
}

void transform_control_scaling::set_transform		( float4x4 const& transform )
{
	// Scaling control ignores the object scaling 
	float4x4 translation_tranform	= create_translation	( transform.c.xyz() );
	float4x4 rotation_tranform		= create_rotation_INCORRECT	( transform.get_angles_xyz_INCORRECT() );
	m_data->m_transform_original	= transform;
	translation_tranform			= rotation_tranform*translation_tranform;
	super::set_transform			( translation_tranform );
	m_data->m_draw_transform		= translation_tranform;
}

void transform_control_scaling::update_visibility	( )
{
	if( m_dragging && !m_plane_mode && m_active_axis != enum_transform_axis_all)
	{
		for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
			set_axis_visible(false,idx);

		set_axis_visible(true, m_active_axis);
	}
	else
		for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
			set_axis_visible(true, idx);
}

void transform_control_scaling::start_input	( )
{
	super::start_input	();
	m_dragging			= true; 
	m_preview_mode		= true;

	float3	saved_origin = get_transform().transform_position( float3( 0, 0, 0 ) );

	if( !m_plane_mode_modifier )
		m_plane_mode = false;
	else
		m_plane_mode = true;

	reset_colors		( );
	update_visibility	( );

	get_color(m_active_axis) = selection_color();

	if( m_active_axis != enum_transform_axis_all )
		m_data->m_curr_axis = get_axis_vector( m_active_axis );
	else
		m_data->m_curr_axis = get_view_transform().j.xyz();

	m_data->m_curr_axis = get_transform().transform_direction( m_data->m_curr_axis );

	float3				mouse_origin;
	float3				mouse_direction;
	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );

	if( m_active_axis != enum_transform_axis_all )
		rey_nearest_point( saved_origin, m_data->m_curr_axis, mouse_origin, mouse_direction, m_data->m_drag_start ); 
	else
		plane_ray_intersect( saved_origin, -get_view_transform().k.xyz(), mouse_origin, mouse_direction, m_data->m_drag_start );


	m_data->m_draw_transform = get_transform();

	m_data->m_resulting_transform.identity();
}

void transform_control_scaling::execute_input	( )
{
	float3			mouse_origin;
	float3			mouse_direction;
	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );


	float3 curr_axis;
	if( m_active_axis != enum_transform_axis_all )
		curr_axis = get_axis_vector( m_active_axis );
	else
		curr_axis = get_view_transform().j.xyz();

	float3 curr_pos;
	m_data->m_scale_vector = float3( 0.f, 0.f, 0.f );
	
	float3	m_saved_origin = get_transform().transform_position( float3( 0.f, 0.f, 0.f ) );
	if( m_active_axis != enum_transform_axis_all)
	{
		if( rey_nearest_point( m_saved_origin, m_data->m_curr_axis, mouse_origin, mouse_direction, curr_pos ) ) 
		{
			float3 start_vector			= (m_data->m_drag_start-m_saved_origin);
			float3 curr_vector			= (curr_pos-m_saved_origin);

			if( !m_plane_mode)
			{
				float4x4 tranlation		= create_translation( get_transform().c.xyz() );
				float3 drag_vector		= ( curr_vector-start_vector )/start_vector.magnitude();
				m_data->m_scale_vector	= curr_axis*( drag_vector|m_data->m_curr_axis );
			}
			else
			{
				float4x4 tranlation		= create_translation( get_transform().c.xyz() );
				float3 drag_vector		= ( curr_vector-start_vector )/start_vector.magnitude();
				m_data->m_scale_vector	= ((float3( 1, 1, 1 ) - curr_axis)*( drag_vector|m_data->m_curr_axis ));
			}
		}
	}
	else
	{
		if( plane_ray_intersect( m_saved_origin, -get_view_transform().k.xyz(), mouse_origin, mouse_direction, curr_pos ) )
		{
			float3 curr_axis			= get_view_transform().i.xyz();
			curr_axis.normalize			();
			float drag_factor			= dot_product( curr_axis, curr_pos-m_data->m_drag_start)/m_center_box_drag_factor;
			m_data->m_scale_vector		= float3( drag_factor, drag_factor, drag_factor );
		}
	}

	m_data->m_scale_vector				+= float3( 1, 1, 1 );
	if( m_data->m_scale_vector.x <0 || m_data->m_scale_vector.y <0 || m_data->m_scale_vector.z <0 )
		return;

	float4x4 scale = create_scale( m_data->m_scale_vector );

	float4x4 test_transform = m_data->m_transform_original*scale;
	if(	math::is_zero( test_transform.determinant(), math::epsilon_5 ) )
		return;


	if(	math::is_zero( test_transform.i.xyz().magnitude() ) )
		return;

	if(	math::is_zero( test_transform.j.xyz().magnitude() ) )
		return;

	if(	math::is_zero( test_transform.k.xyz().magnitude() ) )
		return;

	m_data->m_resulting_transform = scale;

	m_data->m_draw_arrows_lengths = scale.transform( float3( m_arrow_length, m_arrow_length, m_arrow_length ));

	if( m_apply_command == nullptr )
	{
		m_apply_command = gcnew command_apply_control_transform(m_level_editor, this);
		m_level_editor->get_command_engine()->run_preview( m_apply_command );
	}
	else
		m_level_editor->get_command_engine()->preview();
}

void transform_control_scaling::end_input	( )
{
	super::end_input	();
	m_dragging						= false; 
	m_data->m_draw_arrows_lengths	= float3( m_arrow_length, m_arrow_length, m_arrow_length );

	update_visibility				( );

	if( m_apply_command )
	{
		m_level_editor->get_command_engine()->end_preview();
		m_apply_command				= nullptr;
	}

	m_data->m_resulting_transform.identity();
	m_data->m_scale_vector			= float3( 0, 0, 0 );
	m_preview_mode					= false;
}

float4x4 transform_control_scaling::create_object_transform	( float4x4 const& object_tranform, enum_transform_mode mode )
{
	float4x4 resulting_transform;
	if( mode == enum_transform_mode_control )
	{
		float4x4 base_rotation		= create_rotation_INCORRECT( get_transform().get_angles_xyz_INCORRECT() );
		float4x4 base_translation	= create_translation( get_transform().c.xyz() );

		m_data->m_resulting_transform =	math::invert4x3(base_rotation)*m_data->m_resulting_transform*base_rotation;
		
		return object_tranform*math::invert4x3(base_translation)*resulting_transform*base_translation;
	}
	else
	{
		resulting_transform = m_data->m_resulting_transform;
		return resulting_transform*object_tranform;
	}
}

void transform_control_scaling::collide			( collision_object const* object, float3  const& /*position*/ )
{
	collision_object_transform_control const * transform_control_collision = static_cast_checked<collision_object_transform_control const*>(object);
	ASSERT(transform_control_collision->get_owner() == this);
	ASSERT( transform_control_collision->get_axis() >= enum_transform_axis_cam && 
		transform_control_collision->get_axis() <= enum_transform_axis_none);

	enum_transform_axis mode = transform_control_collision->get_axis();

	set_active_axis( mode );
}

void transform_control_scaling::set_active_axis	( enum_transform_axis mode )
{
	m_active_axis =  mode;
}

void transform_control_scaling::draw					( xray::math::float4x4 const& inv_view_matrix )
{
	m_level_editor->get_editor_renderer().flush();

	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz(), m_distance_from_cam, inv_view_matrix ) < 0 )
		return;

	draw_impl	( inv_view_matrix, false );
	draw_impl	( inv_view_matrix, true );
}

void transform_control_scaling::draw_impl( xray::math::float4x4 const& inv_view_matrix, bool transparent_mode )
{
	if( !m_control_visible )
		return;

	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	if( m_draw_collision_geomtery && !transparent_mode )
	{
		u32 count = get_collision_objects().size();
		for( u32 i = 0; i < count; ++i)
		{
			get_collision_objects()[i]->render( rend.debug() );
			rend.draw_aabb( get_collision_objects()[i]->get_center(), get_collision_objects()[i]->get_extents()*0.5f, 0xFFFFFF );
		}
	}
	float4x4 transform = calculate_fixed_size_transform	( m_data->m_draw_transform, m_distance_from_cam, inv_view_matrix );

	// Drawing scale control 
	float3 start_point ( 0, 0, 0 );
	start_point = transform.transform_position( start_point );

	// Drawing axes'lines
	float3 end_point;

	if( m_dragging)
		draw_last_position			( inv_view_matrix, transparent_mode );
	
	end_point	= transform.transform_position( float3( m_data->m_draw_arrows_lengths.x, 0, 0 ));
	bool	x_axis_visible	= !one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius, inv_view_matrix );
			x_axis_visible	= (x_axis_visible && is_axis_visible(enum_transform_axis_x));

	if( x_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( start_point, end_point, math::color_xrgb(	100, 100, 100 ), 1.f, m_transparent_line_pattern );
		else
			rend.draw_line		( start_point, end_point, math::color_xrgb(	100, 100, 100 ) );
	}

	end_point			= transform.transform_position( float3( 0, m_data->m_draw_arrows_lengths.y, 0 ));
	bool	y_axis_visible	= !one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius,  inv_view_matrix );
			y_axis_visible	= (y_axis_visible && is_axis_visible(enum_transform_axis_y));

	if( y_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( start_point, end_point, math::color_xrgb(	100, 100, 100 ), 1.f, m_transparent_line_pattern );
		else
			rend.draw_line		( start_point, end_point, math::color_xrgb(	100, 100, 100 ) );
	}

	end_point			= transform.transform_position( float3( 0, 0, m_data->m_draw_arrows_lengths.z ));
	bool	z_axis_visible	= !one_covers_another( end_point, m_arrow_head_radius, start_point, m_arrow_head_radius, inv_view_matrix );
			z_axis_visible	= (z_axis_visible && is_axis_visible(enum_transform_axis_z));

	if( z_axis_visible )
	{
		if( transparent_mode )
			rend.draw_3D_screen_line	( start_point, end_point, math::color_xrgb(	100, 100, 100 ), 1.f, m_transparent_line_pattern );
		else
			rend.draw_line		( start_point, end_point, math::color_xrgb(	100, 100, 100 ) );
	}


	// Drawing arrows heads as cubes 
	//
	float4x4 cube_transform;

	if( transparent_mode )
	{
		rend.flush();
		rend.setup_grid_render_mode( 50 );
	}
	// Drawing x axis cube.
	if( x_axis_visible )
	{
		cube_transform = create_translation( float3 ( m_data->m_draw_arrows_lengths.x, 0, 0 ));
		rend.draw_cube_solid	(	cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_x) );
	}

	// Drawing y axis cube.
	if( y_axis_visible )
	{
		cube_transform = create_translation	( float3 ( 0, m_data->m_draw_arrows_lengths.y, 0 ));
		rend.draw_cube_solid	(	cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_y) );
	}

	// Drawing z axis cube.
	if( z_axis_visible )
	{
		cube_transform = create_translation	( float3 ( 0, 0, (m_data->m_draw_arrows_lengths.z) ));
		rend.draw_cube_solid	(	cube_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
			get_color(enum_transform_axis_z) );
	}

	rend.draw_cube_solid	(	transform, 
		float3( m_arrow_head_radius, m_arrow_head_radius, m_arrow_head_radius), 
		get_color(enum_transform_axis_all) );

	if( transparent_mode )
	{
		rend.flush();
		rend.remove_grid_render_mode();
	}

}

void transform_control_scaling::draw_last_position			( xray::math::float4x4 const& view_matrix, bool transparent_mode )
{
	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	float4x4		transform	= calculate_fixed_size_transform (	get_transform(), 
																	m_distance_from_cam, 
																	view_matrix );		

	float4x4		transform_translation = create_translation( transform.c.xyz() );

	float4x4		tmp_transform;
	float4x4		rotate_to_view		= create_rotation( view_matrix.i.xyz(), -view_matrix.k.xyz() );
	float3			start_point			= transform.c.xyz();

	float3			end_point;
	float4x4		translation;

	// Drawing axes' arrow lines 
	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_x )
	{
		translation		= create_translation( float3( m_arrow_length, 0, 0 ))*transform;

		if( transparent_mode ) 
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern );
			rend.draw_3D_screen_point	( translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color() );
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());
		
	}

	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_y )
	{
		translation		= create_translation( float3( 0, m_arrow_length, 0 ))*transform;
		if( transparent_mode ) 
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
			rend.draw_3D_screen_point	( translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color() );
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());
	}

	if( m_active_axis == enum_transform_axis_all || m_active_axis == enum_transform_axis_z )
	{
		translation		= create_translation( float3( 0, 0, m_arrow_length ))*transform;
		if( transparent_mode ) 
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
			rend.draw_3D_screen_point	( translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color() );
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());
	}

	if( transparent_mode ) 
		rend.draw_3D_screen_point	( transform.c.xyz(),		m_last_position_vertex_radius, last_position_line_color() );

}
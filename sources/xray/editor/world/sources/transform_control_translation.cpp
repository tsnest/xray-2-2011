////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_translation.h"
#include "collision_object_control.h"
#include "level_editor.h"
#include "project.h"
#include "object_base.h"
#include "command_apply_control_transform.h"

#pragma unmanaged
#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/render/world.h>
#include <xray/editor/world/engine.h>
#include <xray/geometry_utils.h>
#pragma managed

using xray::math::float3;
using xray::math::float4x4;
using namespace xray::memory;


using xray::editor::editor_control_base;
using xray::editor::transform_control_translation;
using xray::editor::transform_control_base;

using xray::editor::collision_object_transform_control;
using xray::editor::collision_object;
using xray::editor::enum_transform_axis;
using xray::editor::enum_transform_mode;
using xray::collision::space_partitioning_tree;

struct transform_control_translation_internal
{
	transform_control_translation_internal()
	:m_resulting_transform		( float4x4().identity() )
	{}
	float3								m_saved_origin;
	float3								m_drag_start;
	float3								m_curr_axis;

	float4x4							m_resulting_transform;
	float4x4							m_draw_transform;
};

/////////////////////////////  transform_control_translation /////////////////////////////////////////////
transform_control_translation::transform_control_translation( xray::editor::level_editor^ le ):
super						( le ),
m_data						(NEW (transform_control_translation_internal) ),
m_arrow_size				( 4.0f ),
m_arrow_head_length			( 0.8f ),
m_arrow_head_radius			( 0.15f ),
m_center_box_radius			( 2*m_arrow_head_radius ),
m_active_plane				( enum_transform_axis_cam ),
m_initialized				( false ),
m_plane_mode				( true ),
m_axis_dragging				( false ),
m_draw_arrow_heads			( true ),
//m_draw_distance_from_cam	( m_distance_from_cam ),
m_center_box				( NULL )
{
	m_control_id			= "translation";
}

transform_control_translation::~transform_control_translation()
{
	destroy	( );
	DELETE (m_data);
}

void transform_control_translation::initialize		( )
{
	if( m_initialized )
	{
		ASSERT( !m_initialized );
		return;
	}

	super::initialize							();

	collision_object_transform_control* tem_obj;
	float4x4 tmp_transform;
	tmp_transform.identity();

	vectora< math::float3 >						vertices			(g_allocator);
	vectora< u16 >								render_indices		(g_allocator);

	geometry_utils::create_cylinder( 
		vertices, 
		render_indices, 
		tmp_transform,
		float3( m_cursor_thickness, (m_arrow_size)/2, m_cursor_thickness)	);

	tmp_transform = create_translation( float3( 0, (m_arrow_size)/2, 0 ));
	
	vectora< u32 >			collision_indices		(*g_allocator);
	vectora< u16 >::const_iterator it;
	vectora< u16 >::const_iterator it_e = render_indices.end();
	for( it = render_indices.begin(); it!=it_e; ++it)
		collision_indices.push_back(*it);

	xray::non_null<collision::geometry>::ptr tmp_geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	
	get_geometries().push_back( &*tmp_geom );

	//collision_object* tem_obj1 = NEW (collision_object)( tmp_transform, &(*tmp_geom) );

	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_y );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj -> set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );
	
	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_x );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj -> set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );
	
	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_z );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj -> set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );


	geometry_utils::create_cone( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		//float3( m_arrow_head_radius, 1, m_arrow_head_radius)	);
		float3( m_arrow_head_radius+m_cursor_thickness, m_arrow_head_length/2+m_cursor_thickness, m_arrow_head_radius+m_cursor_thickness)	);

	
	tmp_transform = create_translation( float3( 0, m_arrow_size+m_arrow_head_length/2, 0 ));

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	tmp_geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*tmp_geom );

	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_y );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_x );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_z );
	tem_obj -> set_matrix_original( tmp_transform );
	tem_obj->set_priority( collision_objects_priority_controls+1 );
	get_collision_objects().push_back( tem_obj );

	// Create plane movment box
	geometry_utils::create_cube( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_center_box_radius+m_cursor_thickness, m_cursor_thickness, m_center_box_radius+m_cursor_thickness)	);

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	tmp_geom = collision::create_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*tmp_geom );

	tmp_transform = create_plane_box_rotation( enum_transform_axis_y, get_transform(), get_view_transform() );
	
	tem_obj = NEW (collision_object_transform_control)( &(*tmp_geom), this, enum_transform_axis_cam );
	tem_obj->set_priority( collision_objects_priority_controls );
	get_collision_objects().push_back( tem_obj );

	m_center_box = tem_obj;

	m_initialized = true;
}

void transform_control_translation::destroy		( )
{
	if( !m_initialized)
		return;
	super::destroy	();
	m_initialized = false;

}

void transform_control_translation::resize		( float size )
{
	ASSERT( size >=  0, "Size must be greater or equal to 0" );

	bool visible = m_control_visible;
	if( visible )
		show		( false );

	bool initialized = m_initialized;
	if( initialized )
		destroy		( );

	m_arrow_size = size;

	if( initialized )
		initialize	( );

	update		();

	if( visible )
		show		( true );
}

void transform_control_translation::update			( )
{
	super::update			( );

	if( m_initialized && m_control_visible )
		m_center_box->set_matrix_original(  create_plane_box_rotation( m_active_plane, get_transform(), get_view_transform() ) );

}

void transform_control_translation::set_transform		( float4x4 const& transform )
{
	// translation control ignores the object orientation 
	float4x4 translation_tranform = create_translation( transform.c.xyz() );
	super::set_transform( translation_tranform );
	m_data->m_draw_transform = translation_tranform;
}

void transform_control_translation::update_colors		( )
{
	reset_colors();
	
	if( m_plane_mode )
		get_color(enum_transform_axis_cam) = selection_color();
	else
		get_color(m_active_axis) = selection_color();
}

void transform_control_translation::update_visibility	( )
{
	m_draw_arrow_heads = true;
	if( m_dragging )
	{
		if( !m_plane_mode  )
		{
			for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
				set_axis_visible(false,idx);

			set_axis_visible(true, m_active_axis);
		}
		else
		{
			for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
				set_axis_visible(true, idx);
			if( !m_axis_dragging )
			{
				set_axis_visible(false, m_active_plane);
			}
			m_draw_arrow_heads = false;
		}
	}
	else
	for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
		set_axis_visible(true, idx);
}

void transform_control_translation::update_active_mode	( )
{
	m_axis_dragging = false;
	
	if(	!m_plane_mode_modifier )
	{
		//if( !m_free_mode_modifier )
			m_plane_mode = false;
	}
	else
	{
		set_active_plane( m_active_axis );

// 		// This line is only for correspondence with Maya behaviour.
// 		m_active_axis = enum_transform_axis_cam;

		m_plane_mode = true;
	}


	if( m_active_axis == enum_transform_axis_cam )
	{
		m_plane_mode = true;
		m_axis_dragging = false;
	}
	else
		m_axis_dragging = true;
}

void transform_control_translation::start_input	( )
{
	super::start_input	();

	m_dragging			= true;
	m_preview_mode		= true;
	update_active_mode	( );
	update_colors		( );
	update_visibility	( );

	float3	m_saved_origin = get_transform().transform_position( float3( 0, 0, 0 ) );
	float3	curr_axis;
	
	if( !m_plane_mode)
		curr_axis = get_axis_vector( m_active_axis );
	else
		curr_axis = get_axis_vector( m_active_plane );

	float3 mouse_origin;
	float3 mouse_direction;
	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );

	if( !m_plane_mode )
		 rey_nearest_point( m_saved_origin, curr_axis, mouse_origin, mouse_direction, m_data->m_drag_start ); 
	else
	{
		if( m_axis_dragging )
			rey_nearest_point( mouse_origin, mouse_direction, m_saved_origin, curr_axis, m_data->m_drag_start ); 
		else
			plane_ray_intersect( m_saved_origin, curr_axis, mouse_origin, mouse_direction, m_data->m_drag_start ); 
	}

	m_data->m_draw_transform = get_transform();
	m_data->m_resulting_transform.identity();
}

void transform_control_translation::execute_input	( )
{
	float3 mouse_origin;
	float3 mouse_direction;
	m_level_editor->get_mouse_ray( mouse_origin, mouse_direction );

	float3 curr_pos;
	float3 cam_pos = get_view_transform().c.xyz();

	if( !m_plane_mode)
	{
		float3	m_saved_origin = get_transform().transform_position( float3( 0, 0, 0 ) );
		float3	m_curr_axis = get_axis_vector( m_active_axis );

		if( !rey_nearest_point( m_saved_origin, m_curr_axis, mouse_origin, mouse_direction, curr_pos ) ) 
			return;
	}
	else
	{
		if( !plane_ray_intersect( m_data->m_drag_start, get_axis_vector( m_active_plane ), mouse_origin, mouse_direction, curr_pos ) )
			return;
	}

	float drag_distance = (m_data->m_drag_start-curr_pos).magnitude();
	if( math::float_max < drag_distance || math::is_zero(drag_distance) )
		return;

	m_data->m_resulting_transform = create_translation( curr_pos - m_data->m_drag_start );
	m_data->m_draw_transform = get_transform()*m_data->m_resulting_transform;

//	m_draw_distance_from_cam = m_distance_from_cam;// /drag_distance*(curr_pos-cam_pos).magnitude();


	if( m_apply_command == nullptr )
	{
		m_apply_command = gcnew command_apply_control_transform(m_level_editor, this);
		m_level_editor->get_command_engine()->run_preview( m_apply_command );
	}
	else
		m_level_editor->get_command_engine()->preview( );
}	

void transform_control_translation::end_input	( )
{
	super::end_input		();
	m_dragging				= false;
 	update_visibility		( );

	if( m_apply_command )
	{
		m_level_editor->get_command_engine()->end_preview();
		m_apply_command = nullptr;
	}

	m_data->m_resulting_transform.identity();
	m_preview_mode		= false;

	set_transform( m_level_editor->get_project()->aim_object()->get_transform() );
}

float4x4	transform_control_translation::create_object_transform	( float4x4 const& object_tranform, enum_transform_mode )
{
	return math::float4x4( object_tranform*m_data->m_resulting_transform );
}

void transform_control_translation::collide		( collision_object const* object, float3 const& /*position*/ )
{
	collision_object_transform_control const * transform_control_collision = static_cast_checked<collision_object_transform_control const*>(object);
	ASSERT(transform_control_collision->get_owner() == this);
	ASSERT( transform_control_collision->get_axis() >= enum_transform_axis_cam && 
			transform_control_collision->get_axis() <= enum_transform_axis_none);

	enum_transform_axis mode = (enum_transform_axis) transform_control_collision->get_axis();
	
	set_active_axis( mode );
}

void transform_control_translation::set_active_axis	( enum_transform_axis mode )
{
	m_active_axis =  mode;
}

void transform_control_translation::set_active_plane ( enum_transform_axis mode )
{
	m_active_plane =  mode;
}

float4x4 transform_control_translation::create_plane_box_rotation( enum_transform_axis mode, float4x4 const& transform, float4x4 const& view )
{
	XRAY_UNREFERENCED_PARAMETER	( transform );

	if( mode == enum_transform_axis_cam )
	{
		const float3 vector_to_object =	view.k.xyz();//( view.c.xyz()-transform.c.xyz() ).normalize( );
		return create_rotation( view.i.xyz(), vector_to_object );
	}
	else if ( mode == enum_transform_axis_z )
	{
		return math::create_rotation_x(  -math::deg2rad(90) );
	}
	else if ( mode == enum_transform_axis_x )
	{
		return math::create_rotation_z(  -math::deg2rad(90) );
	}
	else if ( mode == enum_transform_axis_y )
	{
		return float4x4().identity();
	}

	return float4x4().identity();
}

void transform_control_translation::draw					( xray::math::float4x4 const& view_matrix )
{
	m_level_editor->get_editor_renderer().flush();
	
	if( calculate_fixed_size_scale( m_data->m_draw_transform.c.xyz(), m_distance_from_cam, view_matrix ) < 0 )
		return;
	
	draw_impl				( view_matrix, false );
	draw_impl				( view_matrix, true );
}

void transform_control_translation::draw_impl				( xray::math::float4x4 const& view_matrix, bool transparent_mode )
{
	if( !m_control_visible )
		return;

	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	if( m_draw_collision_geomtery )
	{
		u32 count = get_collision_objects().size();
		for( u32 i = 0; i < count; ++i)
		{
			get_collision_objects()[i]->render( rend.debug() );
			rend.draw_aabb( get_collision_objects()[i]->get_center(), get_collision_objects()[i]->get_extents()*0.5f, 0xFFFFFF );
		}
	}

	float4x4 transform = calculate_fixed_size_transform	( m_data->m_draw_transform, m_distance_from_cam, view_matrix );

	// Drawing translation control 
	float3 start_point ( 0, 0, 0 );
	start_point = transform.transform_position( start_point );

	if( m_dragging)
		draw_last_position			( view_matrix, transparent_mode );

	// Drawing axes' arrow lines 
	float3 end_point;
	end_point				= transform.transform_position( float3( m_arrow_size, 0, 0 ));
	bool	x_axis_visible	= !one_covers_another( end_point, 2*m_arrow_head_radius, start_point, m_center_box_radius, view_matrix );
			x_axis_visible	= (x_axis_visible && is_axis_visible(enum_transform_axis_x));

	if( x_axis_visible )
	{
		if( !transparent_mode)
			rend.draw_line		( start_point, end_point, get_color(enum_transform_axis_x));
		else
			rend.draw_3D_screen_line	( start_point, end_point, get_color(enum_transform_axis_x), 1.f, m_transparent_line_pattern);
	}
			

	end_point				= transform.transform_position( float3( 0, m_arrow_size, 0 ));
	bool	y_axis_visible	= !one_covers_another( end_point, 2*m_arrow_head_radius, start_point, m_center_box_radius,  view_matrix );
			y_axis_visible	= (y_axis_visible && is_axis_visible(enum_transform_axis_y));

	if( y_axis_visible )
		if( !transparent_mode)
			rend.draw_line		( start_point, end_point, get_color(enum_transform_axis_y));
		else
			rend.draw_3D_screen_line	( start_point, end_point, get_color(enum_transform_axis_y), 1.f, m_transparent_line_pattern);



	end_point				= transform.transform_position( float3( 0, 0, m_arrow_size ));
	bool	z_axis_visible	= !one_covers_another( end_point, 2*m_arrow_head_radius, start_point, m_center_box_radius, view_matrix );
			z_axis_visible	= (z_axis_visible && is_axis_visible(enum_transform_axis_z));

	if( z_axis_visible )
		if( !transparent_mode)
			rend.draw_line		( start_point, end_point, get_color(enum_transform_axis_z));
		else
			rend.draw_3D_screen_line	( start_point, end_point, get_color(enum_transform_axis_z), 1.f, m_transparent_line_pattern);


	// draw center box
	float4x4 rotate_to_view = create_plane_box_rotation( m_active_plane, transform, view_matrix )	;
	rotate_to_view = rotate_to_view*transform;

	if( !transparent_mode)
	{
		rend.draw_rectangle	( rotate_to_view, 
			float3( m_center_box_radius, m_center_box_radius, m_center_box_radius ), 
			get_color(enum_transform_axis_cam) );
	}
	else
	{		
		rend.draw_3D_screen_rectangle ( create_scale(float3( m_center_box_radius, m_center_box_radius, m_center_box_radius ))*rotate_to_view, 
			get_color(enum_transform_axis_cam), 1.f, m_transparent_line_pattern );
	}


	if( m_draw_arrow_heads )
	{
		if( transparent_mode )
		{
			rend.flush();
			rend.setup_grid_render_mode( 50 );
		}

		// Drawing arrows heads as cones 
		//
		// Drawing y axis cone.
		float4x4 cone_transform = create_translation( float3 ( 0, m_arrow_size+m_arrow_head_length/2, 0 ));

		if( y_axis_visible )
			rend.draw_cone_solid	(	cone_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_length/2, m_arrow_head_radius ), 
			get_color(enum_transform_axis_y));

		// Drawing x axis cone.
		cone_transform = cone_transform*math::create_rotation_z( math::pi_d2 );
		if( x_axis_visible )
			rend.draw_cone_solid		(	cone_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_length/2, m_arrow_head_radius ), 
			get_color(enum_transform_axis_x));

		// Drawing z axis cone.
		cone_transform = cone_transform*math::create_rotation_y( math::pi_d2 );
		if( z_axis_visible )
			rend.draw_cone_solid		(	cone_transform*transform, 
			float3( m_arrow_head_radius, m_arrow_head_length/2, m_arrow_head_radius ), 
			get_color(enum_transform_axis_z));

		if( transparent_mode )
		{
			rend.flush();
			rend.remove_grid_render_mode();
		}
	}

	rend.flush();
}

void transform_control_translation::draw_last_position			( xray::math::float4x4 const& view_matrix, bool transparent_mode )
{
	xray::render::editor::renderer& rend = m_level_editor->get_editor_renderer();

	float4x4		transform			= calculate_fixed_size_transform	( create_translation(get_transform().c.xyz()), m_distance_from_cam, view_matrix );		

	float3			start_point			= transform.c.xyz();

	float4x4		translation;
	float4x4		tmp_transform;


	// Drawing axes' arrow lines 
	if( m_plane_mode || m_active_axis == enum_transform_axis_x )
	{
		translation		= create_translation( float3( m_arrow_size, 0, 0 ))*transform;
		if( transparent_mode )
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
			rend.draw_3D_screen_point	( translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color());
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());
	}

	if( m_plane_mode || m_active_axis == enum_transform_axis_y )
	{
 		translation		= create_translation( float3( 0, m_arrow_size, 0 ))*transform;
		if( transparent_mode )
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
			rend.draw_3D_screen_point	( translation.c.xyz(),		m_last_position_vertex_radius, last_position_line_color() );
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());
	}

	if( m_plane_mode || m_active_axis == enum_transform_axis_z )
	{
 		translation		= create_translation( float3( 0, 0, m_arrow_size))*transform;

		if( transparent_mode )
		{
			rend.draw_3D_screen_line	( start_point, translation.c.xyz(), last_position_line_color(), 1.f, m_transparent_line_pattern);
			rend.draw_3D_screen_point	( translation.c.xyz(), m_last_position_vertex_radius, last_position_line_color() );
		}
		else
			rend.draw_line	( start_point, translation.c.xyz(), last_position_line_color());

	}

	if( transparent_mode )
		rend.draw_3D_screen_point	( transform.c.xyz(), m_last_position_vertex_radius, last_position_line_color() );
}
////////////////////////////////////////////////////////////////////////////
//	Created		: 16.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_translation.h"
#include "scene_view_panel.h"
#include "transform_control_object.h"
#include <xray/editor/base/collision_object_control.h>

#pragma managed( push, off )
#	include <xray/geometry_utils.h>
#pragma managed( pop )

namespace xray{
namespace editor_base{

transform_control_translation::transform_control_translation( transform_control_helper^ helper ):
super						( helper ),
m_arrow_size				( 4.0f ),
m_arrow_head_length			( 0.8f ),
m_arrow_head_radius			( 0.15f ),
m_active_plane				( enum_transform_axis_cam ),
m_plane_mode				( true ),
m_axis_dragging				( false ),
m_draw_arrow_heads			( true ),
m_snap_mode					( snap_mode::no_snap )
{
	m_control_id			= "translation";
	m_mouse_drag_start		= NEW(float3);
	m_origin_drag_start		= NEW(float3);
	m_step_snap_rem			= NEW(float3);
}

transform_control_translation::~transform_control_translation()
{
	DELETE	( m_mouse_drag_start );
	DELETE	( m_origin_drag_start );
	DELETE	( m_step_snap_rem );
}

float4x4 transform_control_translation::calculate( float4x4 const& m )
{
	return m * *m_result;
}

void transform_control_translation::initialize( )
{
	super::initialize							( );

	collision_object_transform_control* tem_obj;
	float4x4 tmp_transform;
	tmp_transform.identity();

	vectora< math::float3 >						vertices			(&debug::g_mt_allocator);
	vectora< u16 >								render_indices		(&debug::g_mt_allocator);

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


	xray::non_null<collision::geometry>::ptr tmp_geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	
	get_geometries().push_back( &*tmp_geom );


	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_y
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );
	
	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_x
	);

	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );
	
	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_z
	);

	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );


	geometry_utils::create_cone( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_arrow_head_radius, //+m_cursor_thickness, 
				m_arrow_head_length/2, //+m_cursor_thickness, 
				m_arrow_head_radius) //+m_cursor_thickness
				);

	
	tmp_transform = create_translation( float3( 0, m_arrow_size+m_arrow_head_length/2, 0 ));

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	tmp_geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*tmp_geom );

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_y
	);

	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_z( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_x
	);

	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = tmp_transform*math::create_rotation_y( math::pi_d2 );

	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_z
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	// Create plane movement box
	float box_size_xz = m_arrow_head_radius*2.0f+m_cursor_thickness;
	geometry_utils::create_cube( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( box_size_xz, m_cursor_thickness, box_size_xz)	);

	collision_indices.clear();
	it		= render_indices.begin();
	it_e	= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

	tmp_geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );

	get_geometries().push_back( &*tmp_geom );

	tmp_transform = create_plane_box_rotation( enum_transform_axis_y, get_matrix() );
	
	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), tmp_geom ),
		this,
		enum_transform_axis_cam
	);
	get_collision_objects().push_back( tem_obj );
}

void transform_control_translation::update( )
{
	super::update			( );

//	if( m_shown )
//		get_collision_objects().back()->set_matrix_original(  create_plane_box_rotation( m_active_plane, get_matrix() ) );

}

void transform_control_translation::set_matrix( float4x4 const& m )
{
	R_ASSERT(m.valid());
	// translation control ignores the object orientation 
	if ( m_helper->m_object->translation_ignore_rotation() )
		super::set_matrix			( create_translation( m.c.xyz() ) );
	else
		super::set_matrix			( create_rotation( m.get_angles_xyz() ) * create_translation( m.c.xyz() ) );
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
				set_axis_visible(false, m_active_plane);

			m_draw_arrow_heads = false;
		}
	}else
	for(int idx=0; idx<enum_transform_axis_COUNT; ++idx)
		set_axis_visible(true, idx);
}

void transform_control_translation::update_active_mode	( )
{
	m_axis_dragging = false;
	
	if(	!m_plane_mode_modifier )
	{
		m_plane_mode = false;
	}else
	{
		set_active_plane( m_active_axis );
		m_plane_mode = true;
	}


	if( m_active_axis == enum_transform_axis_cam )
	{
		m_plane_mode	= true;
		m_axis_dragging = false;
	}else
		m_axis_dragging = true;
}

void transform_control_translation::one_step_execute( enum_transform_axis axis, float value )
{
	m_active_axis						= axis;
	m_helper->m_object->start_modify	( this );

	float3 drag_vector					= get_axis_vector( m_active_axis ) * (step_value*value);
	*m_result							= math::create_translation( drag_vector );

	m_helper->m_object->execute_preview	( this );
	m_helper->m_object->end_modify		( this );
	m_active_axis						= enum_transform_axis_none;
}

void transform_control_translation::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	super::start_input	( mode, button, coll, pos );

	m_dragging			= true;
	m_preview_mode		= true;
	update_active_mode	( );
	update_visibility	( );

	*m_origin_drag_start	= get_matrix().c.xyz();
	
	float3 mouse_origin, mouse_direction;
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );
	if( !m_plane_mode )
	{	
		float3 curr_axis = get_matrix().transform_direction( get_axis_vector( m_active_axis ) ); 
		ray_nearest_point( *m_origin_drag_start, curr_axis, mouse_origin, mouse_direction, *m_mouse_drag_start ); 
	}
	else
	{
		if(m_active_axis==enum_transform_axis_cam)
		{
			*m_mouse_drag_start = *m_origin_drag_start;
		} 
		else
		{
			float3 curr_axis = get_axis_vector( m_active_plane );
			if( m_axis_dragging )
				ray_nearest_point( mouse_origin, mouse_direction, *m_origin_drag_start, curr_axis, *m_mouse_drag_start ); 
			else
				plane_ray_intersect( *m_origin_drag_start, curr_axis, mouse_origin, mouse_direction, *m_mouse_drag_start ); 
		}
	}

	m_result->identity	( );

	float3 p = get_matrix().c.xyz();
	p.x				= p.x - int(p.x);
	p.y				= p.y - int(p.y);
	p.z				= p.z - int(p.z);
	*m_step_snap_rem = p;
}

void transform_control_translation::execute_input( )
{
	if(m_snap_mode==snap_mode::no_snap)
		execute_input_no_snap();
	else
	if(m_snap_mode==snap_mode::step_snap)
		execute_input_step_snap();
	if(m_snap_mode==snap_mode::face_snap || m_snap_mode==snap_mode::vertex_snap)
		execute_input_vf_snap();

	super::execute_input	( );
}

void transform_control_translation::end_input( )
{
	m_dragging				= false;
 	update_visibility		( );

	super::end_input		( );

	m_result->identity		( );
	m_preview_mode			= false;
	m_snap_mode				= snap_mode::no_snap;
}

void transform_control_translation::set_active_plane( enum_transform_axis mode )
{
	m_active_plane =  mode;
}

float4x4 transform_control_translation::create_plane_box_rotation( enum_transform_axis mode, float4x4 const& transform )
{
	XRAY_UNREFERENCED_PARAMETER	( transform );
	float4x4 const& view	= get_inv_view();

	if( mode == enum_transform_axis_cam )
	{
		const float3 vector_to_object =	view.k.xyz();
		float4x4 inverted_matrix;
		inverted_matrix.try_invert( transform );
		R_ASSERT( inverted_matrix.valid() );
		return create_rotation( view.i.xyz(), vector_to_object ) * create_rotation(  inverted_matrix.get_angles_xyz() );
	}
	else if ( mode == enum_transform_axis_z )
	{
		return math::create_rotation_x(  -math::pi_d2 );
	}
	else if ( mode == enum_transform_axis_x )
	{
		return math::create_rotation_z(  -math::pi_d2 );
	}
	else if ( mode == enum_transform_axis_y )
	{
		return float4x4().identity();
	}

	return float4x4().identity();
}

} //namespace editor
} // namespace xray

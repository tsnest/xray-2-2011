////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_scaling.h"
#include <xray/editor/base/collision_object_control.h>
#include "transform_control_object.h"

#pragma managed( push, off )
#include <xray/geometry_utils.h>
#pragma managed( pop )

namespace xray{
namespace editor_base{

transform_control_scaling::transform_control_scaling( transform_control_helper^ helper ):
super						( helper ),
m_arrow_length				( 4.f ),
m_arrow_head_radius			( 0.15f ),
m_center_box_drag_factor	( 12.f ),
m_plane_mode				( true ),
m_data						( NEW (transform_control_scaling_internal) )
{
	m_control_id			= "scale";
}

transform_control_scaling::~transform_control_scaling()
{
	destroy		( );
	DELETE		( m_data );
}

float4x4 transform_control_scaling::calculate( float4x4 const& m )
{
	return *m_result * m;
}

void transform_control_scaling::initialize( )
{
	super::initialize					( );

	collision_object_transform_control* tem_obj;
	float4x4 tmp_transform;


	vectora< math::float3 >						vertices			(&debug::g_mt_allocator);
	vectora< u16 >								render_indices		(&debug::g_mt_allocator);

	tmp_transform.identity();
	geometry_utils::create_cylinder( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_cursor_thickness, (m_arrow_length-m_arrow_head_radius)/2, m_cursor_thickness )	
		);

	tmp_transform = create_translation( float3( 0, (m_arrow_length-m_arrow_head_radius)/2, 0 ));
	geometry_utils::create_cube( 
		vertices, 
		render_indices, 
		tmp_transform.identity(),
		float3( m_arrow_head_radius,
				m_arrow_head_radius,
				m_arrow_head_radius)
		);

	tmp_transform = create_translation( float3( 0, m_arrow_length-m_arrow_head_radius/2, 0 ));

	vectora< u32 >			collision_indices		(*g_allocator);
	vectora< u16 >::const_iterator it				= render_indices.begin();
	vectora< u16 >::const_iterator it_e				= render_indices.end();
	for(; it!=it_e; ++it)
		collision_indices.push_back(*it);

//	xray::non_null<collision::geometry>::ptr geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	xray::non_null<collision::geometry>::ptr geom = collision::new_triangle_mesh_geometry( g_allocator, (float3*)&vertices[0], vertices.size(), &collision_indices[0], collision_indices.size() );
	get_geometries().push_back( &*geom );

	tem_obj = NEW (collision_object_transform_control)	( 
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_y
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = create_translation( float3( m_arrow_length-m_arrow_head_radius/2, 0, 0 ));

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_x
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	tmp_transform = create_translation( float3( 0, 0, m_arrow_length-m_arrow_head_radius/2 ));

	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_z
	);
	tem_obj->set_matrix_original( tmp_transform );
	get_collision_objects().push_back( tem_obj );

	// Create center box collision object
	tem_obj = NEW (collision_object_transform_control)	(
		&*collision::new_triangle_mesh_geometry_instance( g_allocator, float4x4().identity(), geom ),
		this,
		enum_transform_axis_all
	);
	tem_obj->set_matrix_original( tmp_transform.identity() );
	get_collision_objects().push_back( tem_obj );
}

void transform_control_scaling::set_matrix( float4x4 const& m )
{
	// Scaling control ignores the object scaling 
	float4x4 translation			= create_translation( m.c.xyz() );
	float4x4 rotation				= create_rotation	( m.get_angles_xyz() );
	m_data->m_transform_original	= m;
	translation						= rotation * translation;
	super::set_matrix				( translation );
	m_data->m_draw_transform		= translation;
}

void transform_control_scaling::update_visibility( )
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

void transform_control_scaling::one_step_execute( enum_transform_axis axis, float _v )
{
	float const v						= 1.0+step_value*_v;
	m_active_axis						= axis;
	m_helper->m_object->start_modify	( this );
	*m_result = create_scale			( float3(v, v, v ) );
	m_helper->m_object->execute_preview	( this );
	m_helper->m_object->end_modify		( this );
	m_active_axis						= enum_transform_axis_none;
}

void transform_control_scaling::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	super::start_input	( mode, button, coll, pos );
	m_dragging			= true; 
	m_preview_mode		= true;

	float3	saved_origin		= get_matrix().c.xyz();

	if( !m_plane_mode_modifier )
		m_plane_mode = false;
	else
		m_plane_mode = true;

	update_visibility	( );

	if( m_active_axis != enum_transform_axis_all )
		m_data->m_curr_axis = get_axis_vector( m_active_axis );
	else
		m_data->m_curr_axis = get_inv_view().j.xyz();

	m_data->m_curr_axis = get_matrix().transform_direction( m_data->m_curr_axis );

	float3				mouse_origin;
	float3				mouse_direction;
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

	if( m_active_axis != enum_transform_axis_all )
		ray_nearest_point( saved_origin, m_data->m_curr_axis, mouse_origin, mouse_direction, m_data->m_drag_start ); 
	else
		plane_ray_intersect( saved_origin, -get_inv_view().k.xyz(), mouse_origin, mouse_direction, m_data->m_drag_start );


	m_data->m_draw_transform = get_matrix();

	m_result->identity		( );
}

void transform_control_scaling::execute_input	( )
{
	float3			mouse_origin;
	float3			mouse_direction;
	m_helper->get_mouse_ray( mouse_origin, mouse_direction );

//	if old == prev mouse pos return

	float3 curr_axis;
	if( m_active_axis != enum_transform_axis_all )
		curr_axis = get_axis_vector( m_active_axis );
	else
		curr_axis = get_inv_view().j.xyz();

	float3 curr_pos;
	m_data->m_scale_vector = float3( 0.f, 0.f, 0.f );
	
	float3	saved_origin = get_matrix().c.xyz();

	if( m_active_axis != enum_transform_axis_all)
	{
		if( ray_nearest_point( saved_origin, m_data->m_curr_axis, mouse_origin, mouse_direction, curr_pos ) ) 
		{
			float3 start_vector			= (m_data->m_drag_start-saved_origin);
			float3 curr_vector			= (curr_pos-saved_origin);

			if(start_vector.is_similar(curr_vector))
				return;

			if( !m_plane_mode)
			{
				float3 drag_vector		= ( curr_vector-start_vector )/start_vector.length();
				m_data->m_scale_vector	= curr_axis * ( drag_vector|m_data->m_curr_axis );
			}
			else
			{
				float3 drag_vector		= ( curr_vector-start_vector )/start_vector.length();
				m_data->m_scale_vector	= ((float3( 1, 1, 1 ) - curr_axis)*( drag_vector|m_data->m_curr_axis ));
			}
		}
	}
	else
	{
		if( plane_ray_intersect( saved_origin, -get_inv_view().k.xyz(), mouse_origin, mouse_direction, curr_pos ) )
		{
			float3 curr_axis			= get_inv_view().i.xyz();
			curr_axis.normalize			();
			float drag_factor			= dot_product( curr_axis, curr_pos-m_data->m_drag_start)/m_center_box_drag_factor;
			m_data->m_scale_vector		= float3( drag_factor, drag_factor, drag_factor );
		}
	}

	m_data->m_scale_vector				+= float3( 1, 1, 1 );

	float4x4 scale = create_scale( m_data->m_scale_vector );

	float4x4 test_transform = m_data->m_transform_original*scale;
	if(	math::is_zero( test_transform.determinant4x3(), math::epsilon_5 ) )
		return;


	if(	math::is_zero( test_transform.i.xyz().length() ) )
		return;

	if(	math::is_zero( test_transform.j.xyz().length() ) )
		return;

	if(	math::is_zero( test_transform.k.xyz().length() ) )
		return;

	*m_result						= scale;

	m_data->m_draw_arrows_lengths	= scale.transform( float3( m_arrow_length, m_arrow_length, m_arrow_length ));

	super::execute_input			( );
}

void transform_control_scaling::end_input( )
{
	m_dragging						= false; 
	m_data->m_draw_arrows_lengths	= float3( m_arrow_length, m_arrow_length, m_arrow_length );

	update_visibility				( );

	super::end_input				( );

	m_result->identity				( );
	m_data->m_scale_vector			= float3( 0, 0, 0 );
	m_preview_mode					= false;
}

} //namespace editor
} //namespace xray

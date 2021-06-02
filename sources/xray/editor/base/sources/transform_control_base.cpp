////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "transform_control_base.h"
#include "transform_control_object.h"
#include <xray/editor/base/collision_object_control.h>
#include <xray/editor/base/collision_object_types.h>

#pragma managed( push, off )
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/collision_object.h>
#pragma managed( pop )

using xray::collision::space_partitioning_tree;

namespace xray {
namespace editor_base {

struct transform_control_base_internal : private boost::noncopyable
{
	transform_control_base_internal()
	:m_matrix					( float4x4().identity() ),
	m_inv_view					( float4x4().identity() ),
	m_selected_color			( 255,	255, 0, 255 ),
	m_last_position_lines_color	( 100, 100, 100, 255 )
	{
		m_axes_vectors[enum_transform_axis_cam]		= float3( 0, 0, 0 );
		m_axes_vectors[enum_transform_axis_x]		= float3( 1, 0, 0 );
		m_axes_vectors[enum_transform_axis_y]		= float3( 0, 1, 0 );
		m_axes_vectors[enum_transform_axis_z]		= float3( 0, 0, 1 );
		m_axes_vectors[enum_transform_axis_all]		= float3( 1, 1, 1 ).normalize();
		m_axes_vectors[enum_transform_axis_none]	= float3( 0, 0, 0 );

		m_colors_default[enum_transform_axis_cam]	= math::color(	0,		255,	255	);
		m_colors_default[enum_transform_axis_x]		= math::color(	255,	0,		0	);
		m_colors_default[enum_transform_axis_y]		= math::color(	0,		255,	0	);
		m_colors_default[enum_transform_axis_z]		= math::color(	0,		0,		255 );
		m_colors_default[enum_transform_axis_all]	= math::color( 70,		70,		70	);
		m_colors_default[enum_transform_axis_none]	= math::color( 0,		0,		0	);

		memory::fill8(m_axes_visibilities, true, enum_transform_axis_COUNT);

	}
	collision_objects_vec				m_collision_objects;
	geometries_vec						m_geometries;


	float4x4							m_matrix;
	float4x4							m_inv_view;
	float3 			m_axes_vectors		[enum_transform_axis_COUNT];
	color			m_colors_default	[enum_transform_axis_COUNT];
	bool			m_axes_visibilities	[enum_transform_axis_COUNT];

	color const		m_selected_color;
	color const		m_last_position_lines_color;

};

transform_control_base::transform_control_base( transform_control_helper^ helper ) :
	m_data							( NEW (transform_control_base_internal) ),
	m_shown							( false ),
	m_dragging						( false ),
	m_preview_mode					( false ),
	m_cursor_thickness				( 0.05f ),
	m_last_position_vertex_radius	( 5.f ),
	m_transparent_line_pattern		( 0x55555555 ),
	m_active_axis					( enum_transform_axis_none ),
	m_picked_axis					( enum_transform_axis_none ),
	m_helper						( helper ),
	m_viewport						( NEW( render::viewport_type ) ( float2(0.f, 0.f), float2(1.f, 1.f) ) )
{
	sensitivity						= 1.0f;
	m_result						= NEW(math::float4x4)();
	m_result->identity				( );
}

transform_control_base::~transform_control_base()
{
	destroy							( );
	DELETE							( m_viewport  );
	DELETE							( m_result );
	DELETE							( m_data );
}

xray::render::editor::renderer&	transform_control_base::get_editor_renderer( )
{
	return *m_helper->m_editor_renderer;
}

void transform_control_base::activate( bool bactivate )
{
	super::activate	( bactivate );

	if(!bactivate)
		show_( false );
}

void transform_control_base::initialize( )
{
}

void transform_control_base::destroy( )
{
	for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
	{
		collision::collision_object* o = static_cast_checked<collision::collision_object*>(m_data->m_collision_objects[i]);
		o->destroy( g_allocator );
		DELETE ( o );
	}

	m_data->m_collision_objects.clear();

	for( u32 i = 0; i < m_data->m_geometries.size(); ++i)
		collision::delete_geometry( g_allocator, m_data->m_geometries[i] );

	m_data->m_geometries.clear( );
}

void transform_control_base::set_matrix( float4x4 const& m )
{
	m_data->m_matrix						= m;

	space_partitioning_tree* const tree		= m_helper->m_collision_tree;

	collision_objects_vec::const_iterator	i = m_data->m_collision_objects.begin();
	collision_objects_vec::const_iterator	e = m_data->m_collision_objects.end();
	for( ; i != e; ++i )
	{
		collision::collision_object* o = static_cast_checked<collision::collision_object*>(*i);
		tree->move		( o, (*i)->get_original_transform() * calculate_fixed_size_transform( m_data->m_matrix ) );
	}
}

void transform_control_base::update( )
{
	if( !m_helper->empty() && !m_shown )
		show_(true);
	else
	if( m_helper->empty() && m_shown )
		show_(false);

	if ( !m_shown )
		return;

	m_data->m_inv_view		= m_helper->get_inverted_view_matrix( );
	R_ASSERT				( m_data->m_inv_view.valid() );

	m_helper->m_changed		= false;
	float4x4 m				= m_helper->m_object->get_ancor_transform( );
	R_ASSERT				(m.valid());
	set_matrix				( m );

	m_picked_axis			= pick_axis( );
}

float transform_control_base::calculate_fixed_size_scale( float3 const& position )
{
//	const float distance					= 20.0f; // distance from camera, when control has identity size
//	0.0 ... 1.0
	float const min_dist					= 10; 
	float const max_dist					= 70; 
	const float distance					= min_dist + (max_dist-min_dist)*(1.0f-gizmo_size);

	ASSERT									( distance != 0 );
	float3 const camera_to_position			= position - get_inv_view().c.xyz();
	float const camera_to_position_length	= camera_to_position.length();

	if( math::is_zero( camera_to_position_length ) )
		return 1.0f;

	float const result = (camera_to_position_length / distance);
	return math::max( result, .05f );
}

float4x4 transform_control_base::calculate_fixed_size_transform( float4x4 const& transform )
{
	float scale				= calculate_fixed_size_scale( transform.c.xyz() );
	return	create_scale	( float3( scale, scale, scale ) ) * transform;
}

bool transform_control_base::ray_nearest_point( float3 const& point1, 
											   float3 const& direction1, 
											   float3 const& point2, 
											   float3 const& direction2, 
											   float3& intersect_point )
{
	float3 dir1 = direction1; dir1.normalize();
	float3 dir2	= direction2; dir2.normalize();

	float3 perp_direction = dir1^dir2; // cross product

	if( fabs( dir1.dot_product(dir2)) > (1 - math::epsilon_3) )//
		return false;

	float3 dir3 = perp_direction^dir2;
	return plane_ray_intersect	( point2, dir3, point1, dir1, intersect_point );
}

bool transform_control_base::plane_ray_intersect( float3 const& plane_origin, float3 const& plane_normal, float3 const& ray_origin, float3 const& ray_direction, float3& intersect )
{
	if( fabs( plane_normal.dot_product(ray_direction)) < math::epsilon_3 )
		return false;

	float d = -plane_normal.dot_product(plane_origin);

	float t = -( d + (plane_normal.dot_product(ray_origin))) / plane_normal.dot_product(ray_direction);

	intersect = ray_origin+t*ray_direction;

	return true;
}

void transform_control_base::show_( bool show )
{
	if( m_shown != show )
	{
		space_partitioning_tree* tree = m_helper->m_collision_tree;

		if( show )
		{
			if( m_helper->m_object->attach( this ) )
			{
				m_shown = show;

				for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
				{
					collision::collision_object* o = static_cast_checked<collision::collision_object*>(m_data->m_collision_objects[i]);
					tree->insert( o, m_data->m_matrix );
				}
			}
		}
		else
		{
			m_shown = show;

			if(m_helper->m_object)
				m_helper->m_object->detach( this );

			for( u32 i = 0; i < m_data->m_collision_objects.size(); ++i)
				tree->erase( m_data->m_collision_objects[i] );
		}
	}

	if( m_shown )
		set_matrix	( m_helper->m_object->get_ancor_transform( ) );
}

float3 transform_control_base::get_axis_vector( enum_transform_axis mode )
{
	R_ASSERT_CMP ( mode, <, enum_transform_axis_COUNT );

	if( mode == enum_transform_axis_cam )
		return -m_data->m_inv_view.k.xyz();

	return m_data->m_axes_vectors[mode];
}

bool transform_control_base::one_covers_another	(	float3 position1, float radius1, 
													float3 position2, float radius2 )
{
	math::float4x4 const& view	= get_inv_view();
	float3 distance_vector1		= position1 - view.c.xyz();
	float3 distance_vector2		= position2 - view.c.xyz();
	float distance1				= distance_vector1.length();
	float distance2				= distance_vector2.length();

	if( distance1 < distance2 )
	{	
		if( math::is_zero( distance1 ) )
			return true;

		float scale_factor		= distance2/distance1;
		radius1					*= scale_factor;
		distance_vector1		*= scale_factor;
		position1				= view.c.xyz() + distance_vector1;
	}else
		return false;

	if( (position2-position1).length() < radius2+radius1)
		return true;

	return false;
}

void transform_control_base::set_mode_modfiers( bool plane_mode, bool free_mode )
{
	m_plane_mode_modifier	= plane_mode;
	m_free_mode_modifier	= free_mode;
}

u32 transform_control_base::acceptable_collision_type()
{
	return collision_object_type_control;
}

float4x4 const&	transform_control_base::get_inv_view()
{
	return m_data->m_inv_view;
}

float4x4 const&	transform_control_base::get_matrix()
{
	return m_data->m_matrix;
}

geometries_vec& transform_control_base::get_geometries()
{
	return m_data->m_geometries;
}

collision_objects_vec& transform_control_base::get_collision_objects()
{
	return m_data->m_collision_objects;
}

color const& transform_control_base::get_color(int idx)
{
	if(idx==m_active_axis || idx==m_picked_axis) 
		return m_data->m_selected_color;
	else
		return m_data->m_colors_default[idx];
}

bool transform_control_base::is_axis_visible(int idx)
{
	return m_data->m_axes_visibilities[idx];
}

void transform_control_base::set_axis_visible(bool val, int idx)
{
	m_data->m_axes_visibilities[idx]=val;
}

color const& transform_control_base::selection_color()
{
	return m_data->m_selected_color;
}

color const& transform_control_base::last_position_line_color()
{
	return m_data->m_last_position_lines_color;
}

void transform_control_base::start_input( int mode, int button, collision::object const* coll, math::float3 const& pos )
{
	set_mode_modfiers	( (mode==1), (button==1) );

	if(coll)
	{
		collision_object_transform_control const * transform_control_collision = static_cast_checked<collision_object_transform_control const*>(coll);
		ASSERT(transform_control_collision->get_owner() == this);
		m_active_axis = (enum_transform_axis) transform_control_collision->get_axis();
	}else
		m_active_axis	= enum_transform_axis_cam;

	super::start_input	( mode, button, coll, pos );

	R_ASSERT			( !m_helper->empty() );
	m_helper->m_object->start_modify		( this );
}

void transform_control_base::execute_input( )
{
	R_ASSERT			( !m_helper->empty() );
	m_helper->m_object->execute_preview		( this );

	set_matrix	( m_helper->m_object->get_ancor_transform( ) );
}

void transform_control_base::end_input( )
{
	R_ASSERT			( !m_helper->empty() );
	m_helper->m_object->end_modify		( this );
	m_active_axis		= enum_transform_axis_none;
}

enum_transform_axis transform_control_base::pick_axis( )
{
	float3 origin;
	float3 direction;
	m_helper->get_mouse_ray( origin, direction );

	collision::ray_objects_type				result_objects(g_allocator);

	if(m_helper->m_collision_tree->ray_query(
		collision_object_type_control,
		origin,
		direction,
		1000.0f,
		result_objects,
		xray::collision::objects_predicate_type() ))
	{
		collision_object_transform_control const* o = static_cast_checked<collision_object_transform_control const*>( result_objects[0].object );
		return o->get_axis();
	}
	 
	 return enum_transform_axis_none;
}

} // editor_base
} // xray

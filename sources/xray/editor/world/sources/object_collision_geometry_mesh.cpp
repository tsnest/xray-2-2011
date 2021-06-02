////////////////////////////////////////////////////////////////////////////
//	Created		: 19.08.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "object_collision_geometry_mesh.h"
#include "object_collision_geometry.h"
#include "tool_base.h"
#include "tool_misc.h"
#include "level_editor.h"
#include "object_collision_geometry_mesh_move_command.h"
#include "object_collision_geometry_set_plane_action.h"

#pragma managed(push, on)
#include <xray/geometry_primitives.h>
#include <xray/collision/primitives.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/editor/base/collision_object_types.h>
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)

using System::Windows::Media::Media3D::Point4D;
using xray::editor::wpf_controls::property_descriptor;
using xray::editor::wpf_controls::object_property_value;

namespace xray{
namespace editor{

////////////////			I N I T I A L I Z E				////////////////

object_collision_geometry_mesh::object_collision_geometry_mesh ( object_collision_geometry^ parent ):
	m_matrix		( NEW(float4x4) ),
	m_primitive		( NEW(collision::primitive) ),
	m_parent		( parent ),
	m_is_anti_mesh	( false )
{	
}

object_collision_geometry_mesh::object_collision_geometry_mesh ( object_collision_geometry^ parent, Boolean is_anti_mesh ):
	m_matrix		( NEW(float4x4) ),
	m_primitive		( NEW(collision::primitive) ),
	m_parent		( parent ),
	m_is_anti_mesh	( is_anti_mesh )
{
}

object_collision_geometry_mesh::~object_collision_geometry_mesh ( )
{
	activate	( false );
	DELETE		( m_matrix );
	DELETE		( m_primitive );
}

////////////////			P R O P E R T I E S				////////////////

Float3 object_collision_geometry_mesh::position::get( )
{
	return Float3( m_matrix->c.xyz( ) );
}
void object_collision_geometry_mesh::position::set( Float3 p )
{
	m_matrix->c.xyz( )		= float3( p );
	m_parent->set_modified	( );
}

Float3 object_collision_geometry_mesh::rotation::get( )
{
	float3 angles_rad		= m_matrix->get_angles_xyz( );
	return Float3( angles_rad * ( 180.0f / math::pi ) );
}
void object_collision_geometry_mesh::rotation::set( Float3 p )
{
	float3 angles_rad		= float3( p ) * ( math::pi / 180.0f );

	*m_matrix				= create_rotation( angles_rad ) * create_translation( m_matrix->c.xyz( ) );
	m_parent->set_modified	( );
}

Float3 object_collision_geometry_mesh::scale::get( )
{
	return Float3(m_primitive->data_);
}
void object_collision_geometry_mesh::scale::set( Float3 p )
{
	m_primitive->data_		= float3(p);
	m_parent->set_modified	( );
}

int object_collision_geometry_mesh::type::get( )
{
	return m_primitive->type;
}
void object_collision_geometry_mesh::type::set( int t )
{
	m_primitive->type		= (collision::primitive_type)t;

	if( m_primitive->type == collision::primitive_cylinder )
		m_primitive->type = collision::primitive_capsule;

	if( m_primitive->type == collision::primitive_truncated_sphere )
	{
		List<Point4D>^ planes = gcnew List<Point4D>( );
		m_additional_data = planes;
	}

	m_parent->set_modified	( );
}

float4x4 object_collision_geometry_mesh::matrix::get ( )
{
	return *m_matrix;
}
void object_collision_geometry_mesh::matrix::set( float4x4 matrix )
{
	*m_matrix = matrix;
}

collision::primitive const& object_collision_geometry_mesh::primitive::get ( )
{
	return *m_primitive;
}

////////////////		P U B L I C   M E T H O D S			////////////////

void object_collision_geometry_mesh::render( render::scene_ptr const& scene, render::debug::renderer& renderer )
{
	float4x4 local_matrix = *m_matrix * m_parent->get_transform( );

	math::color color		=  ( m_is_anti_mesh ) ? math::color( 178, 104, 35, 200 )	: math::color( 100, 100, 200, 200 );//rgba
	math::color edge_color	=  ( m_is_anti_mesh ) ? math::color( 100, 80, 20, 200 )		: math::color( 80, 80, 160, 200 );//rgba
	switch (m_primitive->type)
	{
	case collision::primitive_sphere:
		{
			collision::sphere sphere = m_primitive->sphere( );
			renderer.draw_sphere_solid		( scene, local_matrix.c.xyz( ), sphere.radius, color );
			renderer.draw_sphere			( scene, local_matrix.c.xyz( ), sphere.radius, edge_color, false );
		}break;

	case collision::primitive_box:
		{
			collision::box box = m_primitive->box( );
			renderer.draw_cube_solid		( scene, local_matrix, box.half_side, color );
			renderer.draw_cube				( scene, local_matrix, box.half_side, edge_color, false );
		}break;

	case collision::primitive_cylinder:
		{
			collision::cylinder cylinder = m_primitive->cylinder( );
			renderer.draw_cylinder			( scene, local_matrix, float3( cylinder.radius, cylinder.half_length, cylinder.radius), color );
			renderer.draw_cylinder_solid	( scene, local_matrix, float3( cylinder.radius, cylinder.half_length, cylinder.radius), edge_color, true );	
		}break;
	case collision::primitive_capsule:
		{
			collision::capsule capsule = m_primitive->capsule( );
			renderer.draw_solid_capsule		( scene, local_matrix, float3( capsule.radius, capsule.half_length, capsule.radius), color );
			renderer.draw_line_capsule		( scene, local_matrix, float3( capsule.radius, capsule.half_length, capsule.radius), edge_color, false );
		}break;
	case collision::primitive_truncated_sphere:
		{
			collision::truncated_sphere sphere = m_primitive->truncated_sphere( );

			local_matrix = math::create_scale( float3( sphere.radius, sphere.radius, sphere.radius ) ) * local_matrix;

			//solid
			{
				render::debug_indices_type		indices( g_allocator );
				indices.assign( geometry_utils::ellipsoid_solid::faces, geometry_utils::ellipsoid_solid::faces + geometry_utils::ellipsoid_solid::index_count );

				render::debug_vertices_type		vertices( g_allocator );
				vertices.resize( geometry_utils::ellipsoid_solid::vertex_count );

				render::debug_vertices_type::iterator		i = vertices.begin( );
				render::debug_vertices_type::iterator		e = vertices.end( );
				for ( u32 j = 0; i != e; ++i, j += 3 )
				{
					float3 vertex( geometry_utils::ellipsoid_solid::vertices[ j ], geometry_utils::ellipsoid_solid::vertices[ j + 1 ], geometry_utils::ellipsoid_solid::vertices[ j + 2 ] );
					( *i ) = render::vertex_colored( vertex, color );
				}

				List<Point4D>^ planes = safe_cast<List<Point4D>^>( m_additional_data );
				u32 planes_count = planes->Count;
				for( u32 k = 0; k < planes_count; ++k )
				{
					Point4D	point					= planes[k];
					float4	plane					( (Single)point.X, (Single)point.Y, (Single)point.Z, (Single)point.W );
					float	plane_radius_squared	= 1.0f - math::sqr( plane.w );
					float	plane_radius			= math::sqrt( plane_radius_squared );
					float3	plane_position			= plane.xyz( ) * plane.w;

					render::debug_vertices_type::iterator		i = vertices.begin( );
					render::debug_vertices_type::iterator		e = vertices.end( );
					for ( ; i != e; ++i )
					{
						float3 vertex = ( *i ).position;
						
						float pos = ( vertex - plane_position ) | plane.xyz( );
						if( pos > 0 )
						{
							vertex -= plane.xyz( ) * pos;
							float3 plane_to_vertex = vertex - plane_position;
							if( plane_to_vertex.squared_length( ) > plane_radius_squared )
							{
								float	plane_to_vertex_distance	= plane_to_vertex.length( );
								float3	plane_to_vertex_normal		= plane_to_vertex / plane_to_vertex_distance;
								vertex	-= plane_to_vertex_normal * ( plane_to_vertex_distance - plane_radius );
							}
						}
						
						( *i ).position = vertex;
					}
				}

				i = vertices.begin( );
				for ( ; i != e; ++i )
					( *i ).position = ( *i ).position * local_matrix;

				renderer.draw_triangles	( scene, vertices, indices, false );
			}

			//wireframe
			{
				render::debug_indices_type		indices( g_allocator );
				indices.assign( geometry_utils::ellipsoid::pairs, geometry_utils::ellipsoid::pairs + ( geometry_utils::ellipsoid::pair_count * 2 ) );

				render::debug_vertices_type		vertices( g_allocator );
				vertices.resize( geometry_utils::ellipsoid::vertex_count );

				render::debug_vertices_type::iterator		i = vertices.begin( );
				render::debug_vertices_type::iterator		e = vertices.end( );
				for ( u32 j = 0; i != e; ++i, j += 3 )
				{
					float3 vertex( geometry_utils::ellipsoid::vertices[ j ], geometry_utils::ellipsoid::vertices[ j + 1 ], geometry_utils::ellipsoid::vertices[ j + 2 ] );
					( *i ) = render::vertex_colored( vertex, edge_color );
				}

				List<Point4D>^ planes = safe_cast<List<Point4D>^>( m_additional_data );
				u32 planes_count = planes->Count;
				for( u32 k = 0; k < planes_count; ++k )
				{
					Point4D	point					= planes[k];
					float4	plane					( (Single)point.X, (Single)point.Y, (Single)point.Z, (Single)point.W );
					float	plane_radius_squared	= 1.0f - math::sqr( plane.w );
					float	plane_radius			= math::sqrt( plane_radius_squared );
					float3	plane_position			= plane.xyz( ) * plane.w;

					render::debug_vertices_type::iterator		i = vertices.begin( );
					render::debug_vertices_type::iterator		e = vertices.end( );
					for ( ; i != e; ++i )
					{
						float3 vertex = ( *i ).position;
						
						float pos = ( vertex - plane.xyz( ) * plane.w ) | plane.xyz( );
						if( pos > 0 )
						{
							vertex -= plane.xyz( ) * pos;
							float3 plane_to_vertex = vertex - plane_position;
							if( plane_to_vertex.squared_length( ) > plane_radius_squared )
							{
								float	plane_to_vertex_distance	= plane_to_vertex.length( );
								float3	plane_to_vertex_normal		= plane_to_vertex / plane_to_vertex_distance;
								vertex	-= plane_to_vertex_normal * ( plane_to_vertex_distance - plane_radius );
							}
						}
						
						( *i ).position = vertex;
					}
				}

				i = vertices.begin( );
				for ( ; i != e; ++i )
					( *i ).position = ( *i ).position * local_matrix;

				renderer.draw_lines	( scene, vertices, indices, false );
			}

		}break;
	}
}

void object_collision_geometry_mesh::dbg_render_nearest_point( render::scene_ptr const& scene, render::debug::renderer& r, float3 const& point )
{
	m_collision_object->dbg_render_nearest_point( scene, r,  point );
}

collision::geometry_instance*	object_collision_geometry_mesh::get_geometry_instance	( )
{
	return m_collision_object->get_geometry_instance( );
}

void object_collision_geometry_mesh::load( configs::lua_config_value const& t )
{
	m_primitive->type	= (collision::primitive_type)((int)t["type"]);
	float3 position		= t["position"];
	float3 rotation		= t["rotation"];
	float3 scale		= t["scale"];

	*m_matrix = create_rotation(rotation)* create_translation(position);

	switch (m_primitive->type)
	{

	case collision::primitive_sphere:
		{
			m_primitive->data_ = float3(scale.x, scale.x, scale.x) ;
		}break;

	case collision::primitive_box:
		{
			m_primitive->data_ = scale;
		}break;
	case collision::primitive_cylinder:
	case collision::primitive_capsule:
		{
			m_primitive->data_ = float3(scale.x, scale.y, scale.x) ;
		}break;
	case collision::primitive_truncated_sphere:
		{
			m_primitive->data_ = scale;
			
			configs::lua_config_value const& planes_cfg = t["planes"];
			List<Point4D>^ planes	= gcnew List<Point4D>( );
			u32 count				= planes_cfg.size( );
			for ( u32 i = 0; i < count; ++i )
			{
				float4 plane = planes_cfg[i];
				planes->Add( Point4D( plane.x, plane.y, plane.z, plane.w ) );
			}

			m_additional_data = planes;

		}break;
	}
}

void object_collision_geometry_mesh::save( configs::lua_config_value& t )
{
	t["type"]			= (int)m_primitive->type;
	t["position"]		= m_matrix->c.xyz();
	t["rotation"]		= m_matrix->get_angles_xyz();
	t["scale"]			= m_primitive->data_;

	if( m_primitive->type == collision::primitive_truncated_sphere )
	{
		configs::lua_config_value planes_cfg = t["planes"];
		List<Point4D>^ planes	= safe_cast<List<Point4D>^>( m_additional_data );
		u32 count				= planes->Count;
		for ( u32 i = 0; i < count; ++i )
			planes_cfg[i] = float4( (Single)planes[i].X, (Single)planes[i].Y, (Single)planes[i].Z, (Single)planes[i].W );
	}
}

collision::geometry_instance* object_collision_geometry_mesh::create_geometry( )
{
	collision::geometry_instance* result = NULL; 
	switch (m_primitive->type)
	{
	case collision::primitive_sphere:
		{
			collision::sphere sphere		= m_primitive->sphere	( );
			result =  &*collision::new_sphere_geometry_instance( g_allocator, float4x4().identity(), sphere.radius );
		}break;

	case collision::primitive_box:
		{
			collision::box box				= m_primitive->box		( );
			result = &*collision::new_box_geometry_instance( g_allocator, create_scale( box.half_side ) );
		}break;

	case collision::primitive_cylinder:
		{
			collision::cylinder cylinder	= m_primitive->cylinder	( );
			result = &*collision::new_cylinder_geometry_instance( g_allocator, float4x4().identity(), cylinder.radius, cylinder.half_length );
		}break;
	case collision::primitive_capsule:
		{
			collision::capsule capsule		= m_primitive->capsule	( );
			result = &*collision::new_capsule_geometry_instance( g_allocator, float4x4().identity(), capsule.radius, capsule.half_length );
		}break;
	case collision::primitive_truncated_sphere:
		{
			collision::truncated_sphere sp = m_primitive->truncated_sphere();
			result =  &*collision::new_sphere_geometry_instance( g_allocator, float4x4().identity(), sp.radius );
		}break;
	}
	return result;
}

void object_collision_geometry_mesh::activate( bool value )
{
	if( value == ( m_collision_object != NULL ) )
		return;

	if( value )
	{
		m_collision_object	= NEW (object_collision_geometry_mesh_collision)( this );
		m_parent->get_collision_tree( )->insert	(	m_collision_object, 
													*m_matrix * m_parent->get_transform( )
												);
	}
	else
	{
		m_parent->get_collision_tree( )->erase( m_collision_object );
		DELETE						( m_collision_object );
		m_collision_object	= NULL;
	}
}

math::float4x4 start_modify_matrix;

void object_collision_geometry_mesh::start_modify( editor_base::transform_control_base^ control )
{
	object_collision_geometry_mesh_move_command^ command = gcnew object_collision_geometry_mesh_move_command(
		m_parent, m_parent->get_mesh_index( this, m_is_anti_mesh ), m_is_anti_mesh, control
	);

	m_parent->owner_tool( )->get_level_editor( )->get_command_engine( )->run_preview( command );
}

void object_collision_geometry_mesh::execute_preview( editor_base::transform_control_base^ )
{
	m_parent->owner_tool( )->get_level_editor( )->get_command_engine( )->preview( );
}

void object_collision_geometry_mesh::end_modify( editor_base::transform_control_base^ )
{
	m_parent->owner_tool( )->get_level_editor( )->get_command_engine( )->end_preview( );
}

float4x4 object_collision_geometry_mesh::get_ancor_transform( )
{
	return *m_matrix * m_parent->get_transform( );
}

property_container^  object_collision_geometry_mesh::get_property_container	( )
{
	property_container^	sub		= gcnew property_container;
	editor_base::object_properties::initialize_property_container( this, sub );

	if( m_primitive->type == collision::primitive_truncated_sphere )
	{
		sub->properties->add( gcnew property_descriptor( "planes", gcnew object_property_value<List<Point4D>^>( safe_cast<List<Point4D>^>( m_additional_data ) ) ) );
	}

	return sub;
}

void object_collision_geometry_mesh::set_selected( bool value )
{
	if( value )
	{
		if( m_primitive->type == collision::primitive_truncated_sphere )
			safe_cast<tool_misc^>( m_parent->owner_tool( ) )->collision_geometry_set_plane_action->set_collision_geometry_mesh( this );

		m_parent->select_mesh( this );

		last_property_descriptor->is_selected = true;
	}
	else
	{
		if( m_primitive->type == collision::primitive_truncated_sphere )
			safe_cast<tool_misc^>( m_parent->owner_tool( ) )->collision_geometry_set_plane_action->set_collision_geometry_mesh( nullptr );

		last_property_descriptor->is_selected = false;

		m_parent->select_mesh( nullptr );
	}
}

aabb object_collision_geometry_mesh::get_aabb ( )
{
	return m_collision_object->get_aabb( );
}

void object_collision_geometry_mesh::set_plane ( float4 const& plane )
{
	R_ASSERT( m_primitive->type == collision::primitive_truncated_sphere );
	List<Point4D>^ planes = safe_cast<List<Point4D>^>( m_additional_data );
	planes->Add( Point4D( plane.x, plane.y, plane.z, plane.w ) );
}

float3 object_collision_geometry_mesh::get_absolute_position ( )
{
	return m_parent->get_transform( ).c.xyz( ) + m_matrix->c.xyz( );
}

void object_collision_geometry_mesh::update_collision ( )
{
	activate	( false );
	activate	( true );
}

void  object_collision_geometry_mesh::update_position ( float4x4 const& matrix )
{
	*m_matrix = create_rotation				( matrix.get_angles_xyz( ) ) * create_translation( matrix.c.xyz( ) );
	m_parent->get_collision_tree( )->move	( m_collision_object, *m_matrix * m_parent->get_transform( ) );
}

////////////////		C O L L I S I O N			////////////////

object_collision_geometry_mesh_collision::object_collision_geometry_mesh_collision( object_collision_geometry_mesh^ parent )
:super(g_allocator, xray::editor_base::collision_object_type_touch, parent->create_geometry() )
{
	m_parent = parent;
}

bool object_collision_geometry_mesh_collision::touch( ) const
{
	m_parent->set_selected( true );
	return true;
}

} // namespace editor
} // namespace xray

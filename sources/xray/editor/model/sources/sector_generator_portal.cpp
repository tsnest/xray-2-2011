////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sector_generator_portal.h"
#include "bsp_tree_triangle.h"
#include "edge_utilities.h"
#include "bsp_tree_utilities.h"
#include <xray/collision/geometry.h>
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)
#include "triangles_mesh_utilities.h"
#include "xray\ai_navigation\sources\delaunay_triangulator.h"

namespace xray {
namespace model_editor {
math::cuboid create_cuboid_from_edge( 
			math::float3 const& first,
			math::float3 const& second,
			math::float3 const& normal,
			math::float4x4& m,
			math::float3 const& pos_ext,
			math::float3 const& neg_ext
)
{
	R_ASSERT( !math::is_similar( first, second ) );
	math::float3 const& center_point = 0.5f * ( first  + second );

	math::float3 const& e_norm = ( second - first ).normalize();
	math::plane p;
	math::plane planes[ math::cuboid::plane_count ];
	p.normal			= e_norm;
	p.d					= -( p.normal | ( second + p.normal * pos_ext.x ) );
	planes[ 0 ]			= p;

	p.normal			= -e_norm;
	p.d					= -( p.normal | ( first + p.normal  * neg_ext.x ) );
	planes[ 1 ]			= p;

	p.normal			= normal;
	p.d					= -( p.normal | ( center_point + p.normal * pos_ext.z ) );
	planes[ 2 ]			= p;
	float dist = p.classify( center_point );
	R_ASSERT( dist < 0 );

	p.normal			= -normal;
	p.d					= -( p.normal | ( center_point + p.normal * neg_ext.z ) );
	planes[ 3 ]			= p;
	dist = p.classify( center_point );
	R_ASSERT( dist < 0 );

	math::float3 const& up_n = ( normal ^ e_norm ).normalize();

	p.normal			= up_n;
	p.d					= -( p.normal | ( center_point + p.normal * pos_ext.y ) );
	planes[ 4 ]			= p;
	dist = p.classify( center_point );
	R_ASSERT( dist < 0 );

	p.normal			= -up_n;
	p.d					= -( p.normal | ( center_point + p.normal * neg_ext.y ) );
	planes[ 5 ]			= p;
	dist = p.classify( center_point );
	R_ASSERT( dist < 0 );

	math::cuboid result( planes );

	m.identity();
	m.i.xyz()	= e_norm;	
	m.j.xyz()	= up_n;
	m.k.xyz()	= normal;
	math::float3 const scale_vec (
		( planes[ 0 ].normal * planes[ 0 ].d - planes[ 1 ].normal * planes[ 1 ].d ).length(),
		( planes[ 4 ].normal * planes[ 4 ].d - planes[ 5 ].normal * planes[ 5 ].d ).length(),
		( planes[ 2 ].normal * planes[ 2 ].d - planes[ 3 ].normal * planes[ 3 ].d ).length()		
		);
	math::float4x4 const& scale_matr	= math::create_scale( scale_vec );
	math::float4x4 const& tranlate_matr	= math::create_translation( center_point + ( m.transform_direction( pos_ext - neg_ext ) ) / 2.0f );
	math::float4x4 temp;
	R_ASSERT_U( temp.try_invert( scale_matr ) );
	R_ASSERT_U( temp.try_invert( m ) );
	R_ASSERT_U( temp.try_invert( tranlate_matr ) );
	m = scale_matr * m * tranlate_matr;
	R_ASSERT_U( temp.try_invert( m ) );

	return result;
}

bool segment_intersects_cuboid( math::float3 const& point1, math::float3 const& point2, math::cuboid const& c )
{
	math::float3 intersection;
	math::cuboid::planes_type const& planes = c.planes();
	math::aabb_plane const * const plane_end = planes + math::cuboid::plane_count;
	for ( math::aabb_plane const * plane_it = planes; plane_it !=  plane_end; ++plane_it )
	{
		if ( plane_it->plane.intersect_segment( point1, point2, intersection ) )
		{
			bool inside = true;
			for ( math::aabb_plane const * inner_it = planes; inner_it !=  plane_end; ++inner_it )
			{
				if ( inner_it == plane_it )
					continue;
				if ( inner_it->plane.classify( intersection ) > 0 )
				{
					inside = false;
					break;
				}
			}
			if ( inside )
				return true;
		}
	}
	return false;
}

//struct edge_empty: public std::unary_function<edge const&, bool>
//{
//	bool operator()( edge const& e ) const
//	{
//		return e.indices[ 0 ] == e.indices[ 1 ];
//	}
//};
//
//----------------------------------------------------------------------
sector_generator_portal::sector_generator_portal():
m_id( bsp_tree_triangle::msc_empty_id ),
m_broken( false ),
m_square( 0.0f ),
m_hanged_flags( 0 )
{

}

sector_generator_portal::sector_generator_portal( vertices_type const&	vertices, indices_type const& indices, math::plane const& p ):
m_vertices( vertices ),
m_indices( indices ),
m_plane( p ),
m_id( bsp_tree_triangle::msc_empty_id ),
m_broken( false ),
m_square( 0.0f ),
m_hanged_flags( 0 )
{
	R_ASSERT( indices.size() % 3 == 0 );
	m_sectors[ 0 ] = bsp_tree_triangle::msc_empty_id;
	m_sectors[ 1 ] = bsp_tree_triangle::msc_empty_id;
	
	u32 const triangles_count = m_indices.size() / 3;
	u32 largest_triangle_id = bsp_tree_triangle::msc_empty_id;
	float max_square = 0.0f;
	for ( u32 i = 0; i < triangles_count; ++i )
	{
		math::float3 const& first	= m_vertices[ m_indices[ i * 3 ] ];
		math::float3 const& second	= m_vertices[ m_indices[ i * 3 + 1 ] ];
		math::float3 const& third	= m_vertices[ m_indices[ i * 3 + 2 ] ];
		float const square = triangle_square( first, second, third );
		m_square += square;
		if ( square > max_square )
		{
			max_square = square;
			largest_triangle_id = i;
		}
	}
	if ( largest_triangle_id != bsp_tree_triangle::msc_empty_id )
	{
		m_plane = math::create_plane(  
			 m_vertices[ m_indices[ largest_triangle_id * 3 ] ],
			 m_vertices[ m_indices[ largest_triangle_id * 3 + 1 ] ],
			 m_vertices[ m_indices[ largest_triangle_id * 3 + 2 ] ]
		);
	}
	this->initialize_outer_edges();
}

void sector_generator_portal::initialize_outer_edges()
{
	struct edge_element_equals_to: public std::unary_function<edge const&, bool>
	{
		edge_element_equals_to( u32 element_index, u32 element_value )
			:m_element_index( element_index ), m_element_value( element_value ){}
		bool operator()( edge const& e ) const
		{
			return e.indices[ m_element_index ] == m_element_value;
		}
	private:
		u32		m_element_index;
		u32		m_element_value;
	};

	struct edge_contains: public std::unary_function<edge const&, bool>
	{
		edge_contains( u32 element_value )
			: m_element_value( element_value ){}
		bool operator()( edge const& e ) const
		{
			return e.indices[ 0 ] == m_element_value || e.indices[ 1 ] == m_element_value;
		}
	private:
		u32		m_element_value;
	};

	edge_to_u32_type edge_count;
	u32 const triangle_count = m_indices.size() / 3;
	R_ASSERT( triangle_count != 0 );
	for ( u32 i = 0; i < triangle_count; ++i )
	{
		edge const edge0( m_indices[ i * 3 ], m_indices[ i * 3 + 1 ] );
		++edge_count[ edge0 ];
		edge const edge1( m_indices[ i * 3 + 1 ], m_indices[ i * 3 + 2 ] );
		++edge_count[ edge1 ];
		edge const edge2( m_indices[ i * 3 + 2 ], m_indices[ i * 3 ] );
		++edge_count[ edge2 ];
	}
	edge_to_u32_type::const_iterator const edge_count_end = edge_count.end();
	for ( edge_to_u32_type::const_iterator it = edge_count.begin(); it != edge_count_end; ++it )
	{
		if ( it->second == 1 )
		{
			m_outer_edges.push_back( it->first );
		}
	}
	coord_indices_pair coordinate_indices;
	xray::ai::navigation::fill_coordinate_indices( m_plane.normal, coordinate_indices );
	merge_consecutive_edges( coordinate_indices, m_vertices, m_outer_edges );
	//m_outer_edges.erase( 
	//	std::remove_if( m_outer_edges.begin(), m_outer_edges.end(), edge_empty() ),
	//	m_outer_edges.end()
	//);
	R_ASSERT( !m_outer_edges.empty() );
	{
		u32 const vertex_count = m_vertices.size();
		buffer_vector<u32> vertex_use_count( ALLOCA( sizeof( u32 ) * vertex_count ), vertex_count, vertex_count );
		std::fill( vertex_use_count.begin(), vertex_use_count.end(), 0 );

		edges_type::const_iterator const edges_end = m_outer_edges.end();
		for ( edges_type::const_iterator it = m_outer_edges.begin(); it != edges_end; ++it )
		{
			R_ASSERT( it->vertex_index0 != it->vertex_index1 );
			++vertex_use_count[ it->vertex_index0 ];
			++vertex_use_count[ it->vertex_index1 ];
		}

		for ( u32 i = 0; i < vertex_count; ++i )
		{
			if ( vertex_use_count[ i ] == 1 )
			{
				m_outer_edges.erase(
					std::remove_if( m_outer_edges.begin(), m_outer_edges.end(), edge_contains( i ) ),
					m_outer_edges.end()
				);
			}
		}
	}

	edges_type::iterator  const edges_end	= m_outer_edges.end();
	edges_type::iterator current_edge_it	= m_outer_edges.begin();
	edges_type::iterator after_current_it	= current_edge_it + 1; 
	while ( after_current_it != edges_end )
	{
		edges_type::iterator next_edge = std::find_if( 
			after_current_it,
			edges_end,
			edge_element_equals_to( 0, current_edge_it->indices[ 1 ] )
		);
		if ( next_edge == edges_end )
		{
			//edges_type all_edges;
			//for ( u32 i = 0; i < triangle_count; ++i )
			//{
			//	all_edges.push_back( edge ( m_indices[ i * 3 ], m_indices[ i * 3 + 1 ] ) );
			//	all_edges.push_back( edge ( m_indices[ i * 3 + 1 ], m_indices[ i * 3 + 2 ] ) );
			//	all_edges.push_back( edge ( m_indices[ i * 3 + 2 ], m_indices[ i * 3 ] ) );
			//}

			//input_indices_type indices_from_edges;
			//get_vertex_indices_of_edges( all_edges, indices_from_edges );
			//save_to_lua( m_vertices, indices_from_edges, all_edges, m_plane.normal );
			//UNREACHABLE_CODE();
			m_broken = true;
			m_outer_edges.clear();
			return;
		}
		R_ASSERT( next_edge != edges_end );
		std::iter_swap( after_current_it, next_edge );
		current_edge_it = after_current_it;
		++after_current_it;
	}
	R_ASSERT( m_outer_edges.front().vertex_index0 == m_outer_edges.back().vertex_index1 );
}

sector_generator_portal::outer_edges_type  sector_generator_portal::get_outer_edges() const
{
	outer_edges_type result;
	result.reserve( m_outer_edges.size() );
	edges_type::const_iterator const edges_end = m_outer_edges.end();
	for ( edges_type::const_iterator it = m_outer_edges.begin(); it != edges_end; ++it )
	{
		result.push_back( std::make_pair( m_vertices[ it->vertex_index0 ], m_vertices[ it->vertex_index1 ] ) );
	}
	return result;
}

void sector_generator_portal::add_sector( u32 sector_id )
{
	if ( m_sectors[ 0 ] == sector_id || m_sectors[ 1 ] == sector_id )
		return;
	if ( m_sectors[ 0 ] == bsp_tree_triangle::msc_empty_id )
		m_sectors[ 0 ] = sector_id;
	else if ( m_sectors[ 1 ] == bsp_tree_triangle::msc_empty_id )
		m_sectors[ 1 ] = sector_id;
	else
	{
		m_broken = true;
	}
}

void sector_generator_portal::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	math::color const line_color	( 128, 128, 200 );
	math::color const& plane_color =  !m_broken ? math::color( 128, 128, m_outer_edges.size() <= 4 ? 200 : 0, 128 ) : math::color( 255, 0, 0, 128 );

	u32 const triangles_count = m_indices.size() / 3;
	for ( u32 i = 0; i < triangles_count; ++i )
	{
		math::float3 const& first	= m_vertices[ m_indices[ i * 3 ] ];
		math::float3 const& second	= m_vertices[ m_indices[ i * 3 + 1 ] ];
		math::float3 const& third	= m_vertices[ m_indices[ i * 3 + 2 ] ];
		r.draw_triangle( scene, first, second, third, plane_color );
		r.draw_triangle( scene, first, third, second, plane_color );
	}
	if ( !m_hanged_flags )
	{
		edges_type::const_iterator const	outer_edges_end = m_outer_edges.end();
		for ( edges_type::const_iterator it = m_outer_edges.begin(); it != outer_edges_end; ++it )
		{
			r.draw_line( scene, m_vertices[ it->vertex_index0 ], m_vertices[ it->vertex_index1 ], line_color );
		}
	}
	else
	{
		math::color const hanged_line_color( 255, 0, 0 );
		u32 const edges_count = m_outer_edges.size();
		for ( u32 i = 0; i < edges_count; ++i )
		{
			r.draw_line( 
				scene, 
				m_vertices[ m_outer_edges[ i ].vertex_index0 ], 
				m_vertices[ m_outer_edges[ i ].vertex_index1 ], 
				m_hanged_flags & ( 1 << i ) ? hanged_line_color : line_color 
			);
		}
		//static math::color const cuboid_color( 255, 128, 0 );
		//matrices_type::const_iterator const matrices_end = m_matrices_for_hanged_edges.end();
		//for ( matrices_type::const_iterator it = m_matrices_for_hanged_edges.begin(); it != matrices_end; ++it )
		//{
		//	r.draw_cube ( scene, *it, math::float3( 0.5f, 0.5f, 0.5f ), cuboid_color );
		//}
	}
	//test_points_type::const_iterator const test_points_end = m_test_points.end();
	//for ( test_points_type::const_iterator it = m_test_points.begin();it != test_points_end; ++it )
	//{
	//	r.draw_sphere	( scene, it->pos, 1e-2f, math::color( 255, it->hanged ? 0 : 255, 0 ) );
	//	r.draw_line		( scene, it->pos, it->ray_end, math::color( 255, it->hanged ? 0 : 255, 0 ) );
	//}
}
//static inline bool true_predicate( xray::collision::ray_triangle_result const& ) 
//{
//	return true;
//}
//static inline bool false_predicate( xray::collision::ray_triangle_result const& ) 
//{
//	return false;
//}
void sector_generator_portal::test_if_hanged( triangles_mesh_type const& mesh )
{
	u32 const edges_count = m_outer_edges.size();
	R_ASSERT( edges_count <= sizeof( u32 ) * CHAR_BIT );
	for ( u32 i = 0; i < edges_count; ++i )
	{
		math::float3 const& first_point		= m_vertices[ m_outer_edges[ i ].vertex_index0 ];
		math::float3 const& second_point	= m_vertices[ m_outer_edges[ i ].vertex_index1 ];
		math::float4x4 matr, inverted;
		math::cuboid const& edge_cuboid		= create_cuboid_from_edge( 
			first_point, 
			second_point, 
			m_plane.normal, 
			matr,
			math::float3( -1e-3f, 1e-5f, 1e-3f ),
			math::float3( -1e-3f, 1e-3f, 1e-3f )
		);
		bool success = inverted.try_invert( matr );
		R_ASSERT( success );
		xray::collision::triangles_type triangles( g_allocator );
		mesh.spatial_tree->cuboid_query( NULL, edge_cuboid, triangles );
		triangles.erase(  
			std::remove_if( triangles.begin(), triangles.end(), std::not1( triangle_intersects_aabb( mesh, inverted ) ) ),
			triangles.end()
		);
		bool edge_is_hanged = true;
		if ( !triangles.empty() )
			if ( !this->test_if_edge_hanged_with_coplanar_edges( i, triangles, mesh ) )
				edge_is_hanged = false;
			else if ( !this->test_if_edge_hanged_with_rays( i, mesh ) )
				edge_is_hanged = false;
		if ( edge_is_hanged )
			m_hanged_flags |= ( 1 << i );
	}
}

bool sector_generator_portal::test_if_edge_hanged_with_rays	( u32 edge_id, triangles_mesh_type const& mesh )
{
	math::float3 const& first_point		= m_vertices[ m_outer_edges[ edge_id ].vertex_index0 ];
	math::float3 const& second_point	= m_vertices[ m_outer_edges[ edge_id ].vertex_index1 ];
	u32 const c_points_count = 10;
	float const c_max_distance = 0.05f;
	float const c_offset = 0.025f;
	math::float3 const& first_to_second			= second_point - first_point;
	math::float3 const& dir						= math::normalize( first_to_second ^ m_plane.normal );
	math::float3 const& first_to_second_dir		= math::normalize( first_to_second );
	math::float3 const& first_to_second_rate	= ( first_to_second - 2.0f *  c_offset * first_to_second_dir ) / static_cast<float>( c_points_count );
	math::float3 const& origin_start			= first_point + ( first_to_second_dir - dir ) * c_offset;
	bool edge_is_hanged = false;
	for ( u32 i = 0; i <= c_points_count; ++i )
	{
		math::float3 const& origin = origin_start + first_to_second_rate * static_cast<float>( i );
		float distance = std::numeric_limits<float>::max();
		if ( !mesh.spatial_tree->ray_test ( origin, dir, c_max_distance, distance ) || distance > c_max_distance )
		{
			edge_is_hanged = true;
			break;
			//m_test_points.push_back( test_point( true, origin, dir, c_max_distance ) );
		}
	}
	return edge_is_hanged;
}

bool sector_generator_portal::test_if_edge_hanged_with_coplanar_edges( u32 edge_id, collision::triangles_type const& triangles, triangles_mesh_type const& mesh )
{
	math::float3 const& edge_start	= m_vertices[ m_outer_edges[ edge_id ].vertex_index0 ];
	math::float3 const& edge_finish = m_vertices[ m_outer_edges[ edge_id ].vertex_index1 ]; 
	coord_indices_pair coordinate_indices;
	xray::ai::navigation::fill_coordinate_indices( m_plane.normal, coordinate_indices );

	edges_type coplanar_edges;
	coplanar_edges.reserve( triangles.size() * 3 );
	collision::triangles_type::const_iterator const triangles_end = triangles.end();
	for ( collision::triangles_type::const_iterator it = triangles.begin(); it != triangles_end; ++it )
	{
		u32 const triangle_id	=	it->triangle_id;
		u32 const first_index	=	mesh.indices[ triangle_id * 3 ];
		u32 const second_index	=	mesh.indices[ triangle_id * 3 + 1 ];
		u32 const third_index	=	mesh.indices[ triangle_id * 3 + 2 ];
		math::float3 const& first_point		= mesh.vertices[ first_index ];
		math::float3 const& second_point	= mesh.vertices[ second_index ];
		math::float3 const& third_point		= mesh.vertices[ third_index ];
		float first_dist, second_dist, third_dist;
		bool const first_on_plane	=	math::is_zero( first_dist	= m_plane.classify( first_point ) );
		bool const second_on_plane	=	math::is_zero( second_dist	= m_plane.classify( second_point ) );
		bool const third_on_plane	=	math::is_zero( third_dist	= m_plane.classify( third_point ) );
		if ( first_on_plane && second_on_plane && is_collinear( coordinate_indices, edge_start, edge_finish, first_point, second_point ) )
			coplanar_edges.push_back( edge( first_index, second_index ) );
		if ( second_on_plane && third_on_plane && is_collinear( coordinate_indices, edge_start, edge_finish, second_point, third_point ) )
			coplanar_edges.push_back( edge( second_index, third_index ) );
		if ( third_on_plane && first_on_plane && is_collinear( coordinate_indices, edge_start, edge_finish, third_point, first_point ) )
			coplanar_edges.push_back( edge( third_index, first_index ) );
	}
	
	std::sort( coplanar_edges.begin(), coplanar_edges.end(), edge_less() );
	coplanar_edges.erase(
		std::unique( coplanar_edges.begin(), coplanar_edges.end(), edge_equals() ),
		coplanar_edges.end()
		);
	merge_consecutive_edges	( coordinate_indices, mesh.vertices, coplanar_edges );

	bool edge_hanged = true;
	edges_type::const_iterator const coplanar_end = coplanar_edges.end();
	for ( edges_type::const_iterator it = coplanar_edges.begin(); it != coplanar_end; ++it )
	{
		math::float3 const& v0				= m_vertices[ m_outer_edges[ edge_id ].vertex_index0 ];
		math::float3 const& v1				= m_vertices[ m_outer_edges[ edge_id ].vertex_index1 ];
		
		math::float3 const& u0				= mesh.vertices[ it->vertex_index0 ];
		math::float3 const& u1				= mesh.vertices[ it->vertex_index1 ];
		
		float const testee_sqr_len			= ( v1 - v0 ).squared_length();
		float const current_edge_sqr_len	= ( u1 - u0 ).squared_length();

		if ( math::is_relatively_similar( testee_sqr_len, current_edge_sqr_len, math::epsilon_5 ) )
		{
			edge_hanged = !(
				math::is_similar( v0, u0 ) && math::is_similar( v1, u1 ) || 
			    math::is_similar( v0, u1 ) && math::is_similar( v1, u0 ) 
			);
		}
		else if ( testee_sqr_len < current_edge_sqr_len )
		{
			bool const v0_is_ok = math::is_similar( v0, u0 ) || math::is_similar( v0, u1 ) ||
				is_on_segment( coordinate_indices, v0, u0, u1 );
			bool const v1_is_ok = math::is_similar( v1, u0 ) || math::is_similar( v1, u1 ) ||
				is_on_segment( coordinate_indices, v1, u0, u1 );
			edge_hanged = !( v0_is_ok && v1_is_ok );
		}
		if ( !edge_hanged )
			break;
	}
	return edge_hanged;
}

void sector_generator_portal::try_to_make_seal( triangles_mesh_type const& mesh )
{
	if ( m_hanged_flags == 0 )
		return;
	u32 const outer_edges_count = m_outer_edges.size();
	for ( u32 i = 0; i < outer_edges_count; ++i )
	{
		if ( ( m_hanged_flags & ( 1 << i ) ) == 0 )
			continue;
		if ( this->try_to_fix_hanged_edge( i, mesh ) )
			m_hanged_flags &= ~( 1 << i );
	}
	if ( !m_hanged_flags )
		this->test_if_hanged( mesh );
}

bool sector_generator_portal::try_to_fix_hanged_edge( u32 edge_index, triangles_mesh_type const& mesh  )
{
	u32 const first_index				= m_outer_edges[ edge_index ].vertex_index0;
	math::float3 const& first_point		= m_vertices[ first_index ];
	u32 const second_index				= m_outer_edges[ edge_index ].vertex_index1;
	math::float3 const& second_point	= m_vertices[ second_index ];
	
	u32 const prev_edge_index = ( edge_index + m_outer_edges.size() - 1 ) % m_outer_edges.size();
	u32 const next_edge_index = ( edge_index + 1 ) % m_outer_edges.size();
	edge const& prev_edge = m_outer_edges[ prev_edge_index ];
	edge const& next_edge = m_outer_edges[ next_edge_index ];
	
	float const prev_sqr_length = ( m_vertices[ prev_edge.vertex_index1 ] - m_vertices[ prev_edge.vertex_index0 ] ).squared_length();
	float const next_sqr_length = ( m_vertices[ next_edge.vertex_index1 ] - m_vertices[ next_edge.vertex_index0 ] ).squared_length();
	float const c_max_distance	= math::sqrt( std::max( prev_sqr_length, next_sqr_length ) ) * 3.0f;
	float const x_ext			= -( second_point - first_point ).length() * 0.1f;
	math::float4x4 matr;
	math::cuboid const& edge_cuboid = create_cuboid_from_edge( 
		first_point, 
		second_point, 
		m_plane.normal, 
		matr, 
		math::float3( x_ext, 1e-3f, 1e-3f ),
		math::float3( x_ext, c_max_distance, 1e-3f )
	);
	//m_matrices_for_hanged_edges.push_back( matr );
	
	collision::triangles_type triangles( g_allocator );
	mesh.spatial_tree->cuboid_query( NULL, edge_cuboid, triangles );
	this->prepare_triangles( triangles, mesh, first_point, second_point, matr );

	math::float3 const& prev_edge_ray_dir	= math::normalize( m_vertices[ prev_edge.vertex_index1 ] - m_vertices[ prev_edge.vertex_index0 ] );
	math::float3 const& next_edge_ray_dir	= math::normalize( m_vertices[ next_edge.vertex_index0 ] - m_vertices[ next_edge.vertex_index1 ] );

	collision::triangles_type::const_iterator const triangles_end = triangles.end();
	for ( collision::triangles_type::const_iterator it = triangles.begin(); it != triangles_end; )
	{
		collision::triangles_type::const_iterator const group_begin	= it;
		math::plane const& group_plane = mesh.data[ it->triangle_id ].plane;
		while ( it != triangles_end && mesh.data[ it->triangle_id ].is_coplanar( group_plane ) )
			++it;
		if ( this->try_group_of_triangle( group_begin, it, mesh, first_point, prev_edge_ray_dir, second_point, next_edge_ray_dir, c_max_distance ) )
		{
			math::plane const& group_plane = mesh.data[ group_begin->triangle_id ].plane;
			math::float3 first_intersection, second_intersection;
			bool success = group_plane.intersect_ray( first_point, prev_edge_ray_dir, first_intersection );
			R_ASSERT_U( success );
			success = group_plane.intersect_ray( second_point, next_edge_ray_dir, second_intersection );
			R_ASSERT_U( success );
			m_vertices[ first_index ]	= first_intersection;
			m_vertices[ second_index ]	= second_intersection;
			return true;
		}
	}
	return false;
}

bool sector_generator_portal::try_group_of_triangle( 
	collision::triangles_type::const_iterator triangles_begin,
	collision::triangles_type::const_iterator triangles_end, 
	triangles_mesh_type const& mesh, 
	math::float3 const& first_point, math::float3 const& first_dir,
	math::float3 const& second_point, math::float3 const& second_dir,
	float max_distance ) const
{
	R_ASSERT( !math::is_similar( first_point, second_point ) );
	R_ASSERT( triangles_begin != triangles_end );
	math::plane const& group_plane = mesh.data[ triangles_begin->triangle_id ].plane;

	float first_dist = std::numeric_limits<float>::max(), second_dist = std::numeric_limits<float>::max();
	bool const first_intersects		= group_plane.intersect_ray( first_point, first_dir, first_dist )		&& first_dist <= max_distance;
	bool const second_intersects	= group_plane.intersect_ray( second_point, second_dir, second_dist )	&& second_dist <= max_distance;
	if ( !first_intersects || !second_intersects )
		return false;

	coord_indices_pair coordinate_indices;
	xray::ai::navigation::fill_coordinate_indices( group_plane.normal, coordinate_indices );

	math::float3 const& first_to_second_normal	= math::normalize( second_point - first_point );
	//We use small shift to prevent missing to triangles on first ans last test points
	math::float3 const& first_intersection		= first_point	+ first_dir	 * first_dist + first_to_second_normal * math::epsilon_3;
	math::float3 const& second_intersection		= second_point	+ second_dir * second_dist - first_to_second_normal * math::epsilon_3;

	u32 const c_points_count = 10;
	math::float3 const& first_to_second			= second_intersection - first_intersection;
	math::float3 const& first_to_second_rate	= first_to_second / static_cast<float>( c_points_count );
	for ( u32 i = 0; i < c_points_count; ++i )
	{
		math::float3 const& current_point = first_intersection + first_to_second_rate * static_cast<float>( i );
		bool point_in_triangle = false;
		for ( collision::triangles_type::const_iterator triangle_it = triangles_begin; triangle_it != triangles_end; ++triangle_it )
		{
			math::float3 const& v0 = mesh.vertices[ mesh.indices[ triangle_it->triangle_id * 3     ] ];
			math::float3 const& v1 = mesh.vertices[ mesh.indices[ triangle_it->triangle_id * 3 + 1 ] ];
			math::float3 const& v2 = mesh.vertices[ mesh.indices[ triangle_it->triangle_id * 3 + 2 ] ];
			if ( is_point_inside_triangle( coordinate_indices, current_point, v0, v1, v2 ) ||
				 is_on_segment(coordinate_indices, current_point, v0, v1 ) ||
				 is_on_segment(coordinate_indices, current_point, v1, v2 ) ||
				 is_on_segment(coordinate_indices, current_point, v2, v0 ) )
			{
				point_in_triangle = true;
				break;
			}
		}
		if ( !point_in_triangle )
			return false;
	}
	return true;
}

void sector_generator_portal::prepare_triangles( collision::triangles_type& triangles, triangles_mesh_type const& mesh,
												 math::float3 const& first_point, math::float3 const& second_point, 
												 math::float4x4 const& matr ) const
{
	using namespace xray::collision;
	struct is_point_on_positive_side : public std::unary_function<triangle_result const&, bool>
	{
		is_point_on_positive_side( triangles_mesh_type const& mesh, math::float3 const& point )
		:m_mesh( &mesh ), m_point( point ){}
		bool operator()( triangle_result const& triangle ) const
		{
			u32 const triangle_id				= triangle.triangle_id;
			return m_mesh->data[ triangle_id ].plane.classify( m_point ) >= 0.0f;
		}
	private:
		triangles_mesh_type const*	m_mesh;
		math::float3				m_point;
	};

	struct triangle_plane_closer_to_point 
		: public std::binary_function<xray::collision::triangle_result const&, xray::collision::triangle_result const&, bool>
	{
		triangle_plane_closer_to_point( triangles_mesh_type const& mesh, math::float3 const& point )
		:m_mesh( &mesh ), m_point( point ){}
		bool operator() ( xray::collision::triangle_result const& left, xray::collision::triangle_result const& right ) const
		{
			math::plane const& left_plane	= m_mesh->data[ left.triangle_id ].plane;
			math::plane const& right_plane	= m_mesh->data[ right.triangle_id ].plane;
			return left_plane.classify( m_point ) < right_plane.classify( m_point );
		}
	private:
		triangles_mesh_type const*	m_mesh;
		math::float3				m_point;
	};
	struct are_vertices_on_plane: public std::unary_function<triangle_result const&, bool>
	{
		are_vertices_on_plane( triangles_mesh_type const& mesh, math::float3 const& point0, math::float3 const& point1 )
			:m_mesh( &mesh ), m_point0( point0 ), m_point1( point1 ){}
		bool operator()( triangle_result const& triangle )
		{
			math::plane const& triangle_plane = m_mesh->data[ triangle.triangle_id ].plane;
			return math::is_zero( triangle_plane.classify( m_point0 ) ) && math::is_zero( triangle_plane.classify( m_point1 ) );
		}
	private:
		triangles_mesh_type const*	m_mesh;
		math::float3				m_point0;
		math::float3				m_point1;
	};


	math::float4x4 inverted;
	bool success = inverted.try_invert( matr );
	R_ASSERT( success );
	
	triangles.erase(
		std::remove_if( triangles.begin(), triangles.end(), std::not1( triangle_intersects_aabb( mesh, inverted ) )	),
		triangles.end()
	);

	math::float3 const& center_point = 0.5f *( first_point + second_point );
	triangles.erase(
		std::remove_if( triangles.begin(), triangles.end(), std::not1( is_point_on_positive_side( mesh, center_point ) ) ),
		triangles.end()
	);

	triangles.erase(
		std::remove_if( triangles.begin(), triangles.end(), are_vertices_on_plane( mesh, first_point, second_point ) ),
		triangles.end()
	);

	std::sort( triangles.begin(), triangles.end(), triangle_plane_closer_to_point( mesh, center_point ) );
}

bool sector_generator_portal::is_edge_hanged ( u32 edge_id ) const 
{
	R_ASSERT( edge_id <= sizeof( m_hanged_flags ) * CHAR_BIT - 1 );
	return ( m_hanged_flags & ( 1 << edge_id ) ) != 0;
}


} // namespace model_editor
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sectors_generator.h"
#include <xray\ai_navigation\sources\graph_generator_adjacency_builder.h>
#include <xray/collision/geometry.h>
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)
#include <xray/math_randoms_generator.h>
#include <xray\ai_navigation\api.h>
#include "unmanaged_delegate.h"

namespace xray {
namespace model_editor {

bool is_point_inside_cuboid( math::float3 const& p, math::cuboid const& c )
{
	math::aabb_plane const * const plane_end = c.planes() + math::cuboid::plane_count;
	for ( math::aabb_plane const * plane_it = c.planes(); plane_it !=  plane_end; ++plane_it )
	{
		if ( plane_it->plane.classify( p ) > 0.0f )
			return false;
	}
	return true;
}

struct sector_square_less : public std::binary_function< spatial_sector const&, spatial_sector const&, bool >
{
	bool operator() ( spatial_sector const& first, spatial_sector const& second )
	{
		return first.get_square() < second.get_square();
	}
};
//
//template< typename T >
//struct id_less : public std::binary_function<T const&, T const&, bool>
//{
//	bool operator()( T const& left, T const& right ) const
//	{
//		return left.get_id() < right.get_id();
//	}
//};

u32 get_actual_triangle( u32 triangle_id, math::float3 const& point, triangles_mesh_type const& mesh )
{
	R_ASSERT( mesh.data[ triangle_id ].has_children() );
	math::plane const& triangle_plane = mesh.data[ triangle_id ].plane;
	u32 result_id = triangle_id;
	coord_indices_pair index_pair;
	xray::ai::navigation::fill_coordinate_indices( triangle_plane.normal, index_pair );
	while ( mesh.data[ result_id ].has_children() )
	{
		u32 const old_triangle_id_value = result_id;
		for ( u32 i = 0; i < 3; ++i )
		{
			u32 const child_id = mesh.data[ result_id ].children[ i ];
			if ( child_id != bsp_tree_triangle::msc_empty_id )
			{
				math::float3 const& v0 = mesh.vertices[ mesh.indices[ child_id * 3 ] ];
				math::float3 const& v1 = mesh.vertices[ mesh.indices[ child_id * 3 + 1 ] ];
				math::float3 const& v2 = mesh.vertices[ mesh.indices[ child_id * 3 + 2 ] ];
				if ( is_point_inside_triangle( index_pair, point, v0, v1, v2 ) )
				{
					result_id = child_id;
					break;
				}
				if ( is_on_segment( index_pair, point, v0, v1 ) )
				{
					u32 const neighbour_id	= mesh.data[ child_id ].neighbours[ 0 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id && 
						mesh.data[ child_id ].square < mesh.data[ neighbour_id ].square )
						result_id = neighbour_id;
					else
						result_id = child_id;
					break;
				}
				if ( is_on_segment( index_pair, point, v1, v2 ) )
				{
					u32 const neighbour_id	= mesh.data[ child_id ].neighbours[ 1 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id && 
						mesh.data[ child_id ].square < mesh.data[ neighbour_id ].square )
						result_id = neighbour_id;
					else
						result_id = child_id;
					break;
				}
				if ( is_on_segment( index_pair, point, v2, v0 ) )
				{
					u32 const neighbour_id	= mesh.data[ child_id ].neighbours[ 2 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id && 
						mesh.data[ child_id ].square < mesh.data[ neighbour_id ].square )
						result_id = neighbour_id;
					else
						result_id = child_id;
					break;
				}
			}
		}
		if ( result_id == old_triangle_id_value )
		{
			if ( mesh.data[ result_id ].children[ 0 ] != bsp_tree_triangle::msc_empty_id )
			{
				result_id = mesh.data[ result_id ].children[ 0 ];
			} 
			else if ( mesh.data[ result_id ].children[ 1 ] != bsp_tree_triangle::msc_empty_id )
			{
				result_id = mesh.data[ result_id ].children[ 1 ];
			} 
			else if ( mesh.data[ result_id ].children[ 2 ] != bsp_tree_triangle::msc_empty_id )
			{
				result_id = mesh.data[ result_id ].children[ 2 ];
			} 
			else 
				UNREACHABLE_CODE();
		}
	}
	R_ASSERT( triangle_id != result_id );
	R_ASSERT( !mesh.data[ result_id ].has_children() );
	R_ASSERT( mesh.data[ result_id ].sector_id != bsp_tree_triangle::msc_empty_id );
	return result_id;
}

//-------------------------------------------------------------

bool sectors_generator::ms_draw_delimited_edges = true;
bool sectors_generator::ms_draw_sectors			= false;
bool sectors_generator::ms_draw_cuboids			= false;
bool sectors_generator::ms_draw_all_portals		= true;
bool sectors_generator::ms_draw_hanged_portals	= true;

sectors_generator::sectors_generator( math::float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count ):
m_active_sector( u32( -1 ) ),
m_active_portal( u32( -1 ) )
{
	initialize_triangles_mesh( vertices, vertex_count, indices, index_count, m_mesh );
	ai::navigation::graph_generator_adjacency_builder ( m_mesh.indices, m_mesh.data );
	u32 const triangle_count = m_mesh.data.size();
	m_triangles.reserve( triangle_count );
	for ( u32 i = 0; i < triangle_count; ++i )
	{
		m_triangles.push_back( i );
	}
}

void sectors_generator::initialize_delimited_edges()
{
	float const c_precision				= math::epsilon_5;
	portals_type::const_iterator const portal_end = m_portals.end();
	for ( portals_type::const_iterator portal_it = m_portals.begin(); portal_it != portal_end; ++portal_it )
	{
		sector_generator_portal const& portal = portal_it->second;
		if ( portal.is_hanged() )
			continue;
		sector_generator_portal::outer_edges_type const edges = portal.get_outer_edges();
		sector_generator_portal::outer_edges_type::const_iterator const edge_end = edges.end();
		for ( sector_generator_portal::outer_edges_type::const_iterator  edge_it = edges.begin(); edge_it != edge_end; ++edge_it )
		{
			//if ( portal.is_edge_hanged( edge_it - edges.begin() ) )
			//	continue;
			math::float4x4 m;
			math::cuboid const& edge_cuboid = create_cuboid_from_edge( 
				edge_it->first, 
				edge_it->second, 
				portal.get_plane().normal, 
				m, 
				math::float3( -5e-3f, 1e-2f, 1e-3f ),
				math::float3( -5e-3f, 5e-2f, 1e-3f )
			);
			m_matrices_for_cuboid.push_back( m );
			math::float4x4 inverted;
			bool const success = inverted.try_invert( m );
			R_ASSERT( success );
			xray::collision::triangles_type triangles( g_allocator );
			m_mesh.spatial_tree->cuboid_query( NULL, edge_cuboid, triangles );
			triangles.erase(
				std::remove_if( triangles.begin(), triangles.end(), std::not1( triangle_intersects_aabb( m_mesh, inverted ) ) ),
				triangles.end()
			);
			coord_indices_pair coordinate_indices;
			xray::ai::navigation::fill_coordinate_indices( portal.get_plane().normal, coordinate_indices );

			xray::collision::triangles_type::const_iterator triangle_end = triangles.end();
			for ( xray::collision::triangles_type::const_iterator triangle_it = triangles.begin(); triangle_it != triangle_end; ++triangle_it )
			{
				u32 const triangle_id				= triangle_it->triangle_id;

				u32 const first						= m_mesh.indices[ triangle_id * 3 ];
				u32 const second					= m_mesh.indices[ triangle_id * 3 + 1 ];
				u32 const third						= m_mesh.indices[ triangle_id * 3 + 2 ];
				math::float3 const& first_point		= m_mesh.vertices[ first ];
				math::float3 const& second_point	= m_mesh.vertices[ second ];
				math::float3 const& third_point		= m_mesh.vertices[ third ];
				bool const first_on_plane	=	math::is_zero( portal.get_plane().classify( first_point ) );
				bool const second_on_plane	=	math::is_zero( portal.get_plane().classify( second_point ) );
				bool const third_on_plane	=	math::is_zero( portal.get_plane().classify( third_point ) );

				if ( first_on_plane && second_on_plane &&
					segment_intersects_cuboid( first_point, second_point, edge_cuboid ) &&
					 /*is_point_inside_cuboid( 0.5f * ( first_point + second_point ), edge_cuboid ) &&*/
					 is_collinear( coordinate_indices, edge_it->first, edge_it->second, first_point, second_point, c_precision ) )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( first, second ), std::max( first, second ) ), portal.get_id() ) );
					u32 const neighbour_id = m_mesh.data[ triangle_id ].neighbours[ 0 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
					{
						m_mesh.data[ triangle_id ].neighbours[ 0 ] = bsp_tree_triangle::msc_empty_id;
						replace_neighbour_of_triangle( neighbour_id, m_mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
					}
				}
				else if ( second_on_plane && third_on_plane && 
					segment_intersects_cuboid( second_point, third_point, edge_cuboid )&&
						  /*is_point_inside_cuboid( 0.5f * ( second_point + third_point ), edge_cuboid ) &&*/
						  is_collinear( coordinate_indices, edge_it->first, edge_it->second, second_point, third_point, c_precision ) )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( second, third ), std::max( second, third ) ), portal.get_id() ) );
					u32 const neighbour_id = m_mesh.data[ triangle_id ].neighbours[ 1 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
					{
						m_mesh.data[ triangle_id ].neighbours[ 1 ] = bsp_tree_triangle::msc_empty_id;
						replace_neighbour_of_triangle( neighbour_id, m_mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
					}
				}
				else if ( third_on_plane && first_on_plane &&
					segment_intersects_cuboid( third_point, first_point, edge_cuboid ) &&
						  /*is_point_inside_cuboid( 0.5f * ( third_point + first_point ), edge_cuboid ) &&*/
						  is_collinear(  coordinate_indices, edge_it->first, edge_it->second, third_point, first_point, c_precision ) )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( first, third ), std::max( first, third ) ), portal.get_id() ) );
					u32 const neighbour_id = m_mesh.data[ triangle_id ].neighbours[ 2 ];
					if ( neighbour_id != bsp_tree_triangle::msc_empty_id )
					{
						m_mesh.data[ triangle_id ].neighbours[ 2 ] = bsp_tree_triangle::msc_empty_id;
						replace_neighbour_of_triangle( neighbour_id, m_mesh, triangle_id, bsp_tree_triangle::msc_empty_id );
					}
				}
				else if ( calculate_side( portal.get_plane(), triangle_id, m_mesh ) == bsp_tree_triangle::spanning )
				{
					this->split_triangle_by_portal( triangle_id, portal_it->second );
				}
				//else
				//{
				//	m_triangles_to_highlight.push_back( triangle_id );
				//}
			}							   
		}
	}
	replace_triangles_with_their_children( m_mesh, m_triangles );
}

void sectors_generator::split_triangle_by_portal( u32 triangle_id,  sector_generator_portal const& p )
{
	if ( !m_mesh.data[ triangle_id ].has_children() )
	{
		R_ASSERT( is_triangle_adjacency_correct( triangle_id, m_mesh ) );
		triangle_ids_type splitted;
		split_triangle( p.get_plane(), triangle_id, m_mesh, splitted, splitted, &m_portal_edges );
		if ( !splitted.empty() )
		{
			triangle_ids_type::const_iterator const splitted_end = splitted.end();
			for ( triangle_ids_type::const_iterator splitted_it = splitted.begin(); splitted_it != splitted_end; ++splitted_it )
			{
				u32 const triangle_id						= *splitted_it;
				u32 const first_index				= m_mesh.indices[ triangle_id * 3 ];
				u32 const second_index				= m_mesh.indices[ triangle_id * 3 + 1 ];
				u32 const third_index				= m_mesh.indices[ triangle_id * 3 + 2 ];
				math::float3 const& first_point		= m_mesh.vertices[ first_index ]; 
				math::float3 const& second_point	= m_mesh.vertices[ second_index ]; 
				math::float3 const& third_point		= m_mesh.vertices[ third_index ]; 
				bool const first_on_plane			= math::is_zero( p.get_plane().classify( first_point ) );
				bool const second_on_plane			= math::is_zero( p.get_plane().classify( second_point ) );
				bool const third_on_plane			= math::is_zero( p.get_plane().classify( third_point ) );
				//R_ASSERT( !( first_on_plane && second_on_plane && third_on_plane ) );
				if ( first_on_plane && second_on_plane )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( first_index, second_index ), std::max( first_index, second_index ) ), p.get_id() ) );
				}
				else if ( second_on_plane && third_on_plane )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( second_index, third_index ), std::max( second_index, third_index ) ), p.get_id() ) );
				}
				else if ( third_on_plane && first_on_plane )
				{
					m_portal_edges.insert( std::make_pair( edge( std::min( third_index, first_index ), std::max( third_index, first_index ) ), p.get_id() ) );
				}
				m_triangles.insert( std::upper_bound( m_triangles.begin(), m_triangles.end(), *splitted_it ), *splitted_it );
			}
			typedef std::pair<triangle_ids_type::iterator, triangle_ids_type::iterator> triangle_id_pair_it;
			triangle_id_pair_it const pair_it = std::equal_range( m_triangles.begin(), m_triangles.end(), triangle_id );
			if ( pair_it.first != pair_it.second )
				m_triangles.erase( pair_it.first );
			
			for ( u32 i = 0; i < 3; ++i )
			{
				if ( m_mesh.data[ triangle_id ].neighbours[ i ] != bsp_tree_triangle::msc_empty_id &&
					 m_mesh.data[ m_mesh.data[ triangle_id ].neighbours[ i ] ].has_children() )
				{
					triangle_id_pair_it const pair_it = std::equal_range( m_triangles.begin(), m_triangles.end(), m_mesh.data[ triangle_id ].neighbours[ i ] );
					if ( pair_it.first != pair_it.second )
						m_triangles.erase( pair_it.first );
				}
			}
		}
	}
	else
	{
		if ( m_mesh.data[ triangle_id ].children[ 0 ] != bsp_tree_triangle::msc_empty_id )
			if ( calculate_side( p.get_plane(), m_mesh.data[ triangle_id ].children[ 0 ], m_mesh ) == bsp_tree_triangle::spanning )
				split_triangle_by_portal( m_mesh.data[ triangle_id ].children[ 0 ], p );
			else
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 0 ], p );
		
		if ( m_mesh.data[ triangle_id ].children[ 1 ] != bsp_tree_triangle::msc_empty_id )
			if ( calculate_side( p.get_plane(), m_mesh.data[ triangle_id ].children[ 1 ], m_mesh ) == bsp_tree_triangle::spanning )
				split_triangle_by_portal( m_mesh.data[ triangle_id ].children[ 1 ], p );
			else
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 1 ], p );
		
		if ( m_mesh.data[ triangle_id ].children[ 2 ] != bsp_tree_triangle::msc_empty_id )
			if ( calculate_side( p.get_plane(), m_mesh.data[ triangle_id ].children[ 2 ], m_mesh ) == bsp_tree_triangle::spanning )
				split_triangle_by_portal( m_mesh.data[ triangle_id ].children[ 2 ], p );
			else
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 2 ], p );
	}
	//R_ASSERT( is_triangle_adjacency_correct( triangle_id, m_mesh ) );
}

void sectors_generator::check_coplanar_edges( u32 triangle_id, sector_generator_portal const& p )
{
	u32 const first_index				= m_mesh.indices[ triangle_id * 3 ];
	u32 const second_index				= m_mesh.indices[ triangle_id * 3 + 1 ];
	u32 const third_index				= m_mesh.indices[ triangle_id * 3 + 2 ];
	math::float3 const& first_point		= m_mesh.vertices[ first_index ]; 
	math::float3 const& second_point	= m_mesh.vertices[ second_index ]; 
	math::float3 const& third_point		= m_mesh.vertices[ third_index ]; 
	bool const first_on_plane			= math::is_zero( p.get_plane().classify( first_point ) );
	bool const second_on_plane			= math::is_zero( p.get_plane().classify( second_point ) );
	bool const third_on_plane			= math::is_zero( p.get_plane().classify( third_point ) );

	if ( m_mesh.data[ triangle_id ].has_children() )
	{
		if ( first_on_plane && second_on_plane || second_on_plane && third_on_plane || third_on_plane && first_on_plane )
		{
			if ( m_mesh.data[ triangle_id ].children[ 0 ] != bsp_tree_triangle::msc_empty_id )
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 0 ], p );
			if ( m_mesh.data[ triangle_id ].children[ 1 ] != bsp_tree_triangle::msc_empty_id  )
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 1 ], p );
			if ( m_mesh.data[ triangle_id ].children[ 2 ] != bsp_tree_triangle::msc_empty_id  )
				this->check_coplanar_edges( m_mesh.data[ triangle_id ].children[ 2 ], p );
		}
	}
	else
	{
		if ( first_on_plane && second_on_plane )
		{
			m_portal_edges.insert( std::make_pair( edge( std::min( first_index, second_index ), std::max( first_index, second_index ) ), p.get_id() ) );
			m_mesh.data[ triangle_id ].neighbours[ 0 ] = bsp_tree_triangle::msc_empty_id;
		}
		else if ( second_on_plane && third_on_plane )
		{
			m_portal_edges.insert( std::make_pair( edge( std::min( second_index, third_index ), std::max( second_index, third_index ) ), p.get_id() ) );
			m_mesh.data[ triangle_id ].neighbours[ 1 ] = bsp_tree_triangle::msc_empty_id;
		}
		else if ( third_on_plane && first_on_plane )
		{
			m_portal_edges.insert( std::make_pair( edge( std::min( third_index, first_index ), std::max( third_index, first_index ) ), p.get_id() ) );
			m_mesh.data[ triangle_id ].neighbours[ 2 ] = bsp_tree_triangle::msc_empty_id;
		}
	}
}

void sectors_generator::render( render::scene_ptr const& scene, render::debug::renderer& r ) const
{
	if ( !m_bad_adjacencies.empty() )
	{
		static const math::color c_incorrect_color( 255, 0, 0 );
		static const math::color c_correct_color( 255, 255, 255 );
		static const math::color c_plane_color( 255, 255, 255, 128 );
		edge_to_bad_adjacencies_type::const_iterator const bad_adjacencies_end = m_bad_adjacencies.end();
		for ( edge_to_bad_adjacencies_type::const_iterator it = m_bad_adjacencies.begin(); it != bad_adjacencies_end; ++it )
		{
			bad_adjacencies_type::const_iterator const triangles_end = it->second.end();
			for ( bad_adjacencies_type::const_iterator triangle_it = it->second.begin(); triangle_it != triangles_end; ++triangle_it )
			{
				u32 const triangle_id	= triangle_it->triangle_id;
				u32 const edge_id		= triangle_it->edge_id;
				math::float3 const& first	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 ] ];
				math::float3 const& second	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 + 1 ] ];
				math::float3 const& third	= m_mesh.vertices[ m_mesh.indices[ triangle_id * 3 + 2 ] ];
				r.draw_line		( scene, first,	second,	edge_id == 0 ? c_incorrect_color : c_correct_color );
				r.draw_line		( scene, second, third,	edge_id == 1 ? c_incorrect_color : c_correct_color );
				r.draw_line		( scene, third,	first,	edge_id == 2 ? c_incorrect_color : c_correct_color );
				r.draw_triangle	( scene, first, second, third, c_plane_color );
			}
		}
	}

	if ( ms_draw_delimited_edges )
	{
		edge_to_u32_type::const_iterator const edge_end = m_portal_edges.end();
		for ( edge_to_u32_type::const_iterator edge_it = m_portal_edges.begin(); edge_it != edge_end; edge_it++ )
		{
			edge const& e = edge_it->first;
			r.draw_line( scene, m_mesh.vertices[ e.vertex_index0 ], m_mesh.vertices[ e.vertex_index1 ], math::color( 0, 255, 0 ) );
		}
	}
	
	if ( ms_draw_cuboids )
	{
		static math::color const cuboid_color( 255, 255, 0, 255 );
		matrices_type::const_iterator const matrices_end = m_matrices_for_cuboid.end();
		for ( matrices_type::const_iterator it = m_matrices_for_cuboid.begin(); it != matrices_end; ++it )
		{
			r.draw_cube ( scene, *it, math::float3( 0.5f, 0.5f, 0.5f ), cuboid_color );
		}
	}
	
		if ( m_active_sector != bsp_tree_triangle::msc_empty_id )
		{
			spatial_sector const& active_sector = m_sectors.find( m_active_sector )->second;
			m_sectors.find( m_active_sector )->second.render( scene, r );
			spatial_sector::portal_ids_type const& active_sector_portals = active_sector.get_portals();
			portals_type::const_iterator const portals_end = m_portals.end();
			for ( portals_type::const_iterator it = m_portals.begin(); it != portals_end; ++it )
			{
				if ( std::binary_search( active_sector_portals.begin(), active_sector_portals.end(), it->second.get_id() ) )
					it->second.render( scene, r );
			}
		}
		else if ( m_active_portal != bsp_tree_triangle::msc_empty_id )
		{
			sector_generator_portal const& active_portal = m_portals.find( m_active_portal )->second;
			active_portal.render( scene, r );
			if ( active_portal.get_sectors()[ 0 ] != bsp_tree_triangle::msc_empty_id )
				m_sectors.find( active_portal.get_sectors()[ 0 ] )->second.render( scene, r );
			if ( active_portal.get_sectors()[ 1 ] != bsp_tree_triangle::msc_empty_id )
				m_sectors.find(  active_portal.get_sectors()[ 1 ] )->second.render( scene, r );
		}
		else
		{
			if ( ms_draw_sectors )
			{
				spatial_sectors_type::const_iterator const sector_end = m_sectors.end();
				for ( spatial_sectors_type::const_iterator sector_it = m_sectors.begin(); sector_it != sector_end; ++sector_it )
				{
					sector_it->second.render( scene, r );
				}
			}
			if ( ms_draw_all_portals )
			{
				portals_type::const_iterator const portals_end = m_portals.end();
				for ( portals_type::const_iterator it = m_portals.begin(); it != portals_end; ++it )
				{
					if ( it->second.is_hanged() && !ms_draw_hanged_portals )
						continue;
					it->second.render( scene, r );
				}
			}
		}
}

void sectors_generator::generate_sectors()
{
	if ( this->look_for_incorrect_adjacency() )
		return;
	this->test_are_portals_hanged();
	this->initialize_delimited_edges();
	math::random32 rnd( reinterpret_cast<u32>( this ) );
	u32 sector_id = 0;
	triangle_ids_type::const_iterator const triangles_end = m_triangles.end();
	for ( triangle_ids_type::const_iterator triangle_it = m_triangles.begin(); triangle_it !=  triangles_end; ++triangle_it )
	{
		R_ASSERT( !m_mesh.data[ *triangle_it ].has_children() );
		if ( m_mesh.data[ *triangle_it ].sector_id == bsp_tree_triangle::msc_empty_id )
		{
			math::color const c( 64u + rnd.random( 127 ), 64u + rnd.random( 127 ), 64u + rnd.random( 127 ), 127u );
			m_sectors[ sector_id ] = spatial_sector( m_mesh, c, sector_id );
			this->add_triangle_to_sector( *triangle_it, m_sectors[ sector_id ] );
			++sector_id; 
		}
	}
	this->merge_portalless_sectors();
}

void sectors_generator::add_triangle_to_sector( u32 triangle_id, spatial_sector& sector )
{
	R_ASSERT( is_triangle_adjacency_correct( triangle_id, m_mesh ) );
	bsp_tree_triangle& current = m_mesh.data[ triangle_id ];
	R_ASSERT( current.sector_id == bsp_tree_triangle::msc_empty_id );
	R_ASSERT( !current.has_children() );
	
	sector.add_triangle( triangle_id );
	current.sector_id = sector.get_id();
	u32 const first_index		= m_mesh.indices[ triangle_id * 3 ];
	u32 const second_index	= m_mesh.indices[ triangle_id * 3 + 1 ];
	u32 const third_index		= m_mesh.indices[ triangle_id * 3 + 2 ];


	edge_to_u32_type::const_iterator const first_edge_it = m_portal_edges.find( edge( first_index, second_index ) );
	if ( first_edge_it != m_portal_edges.end() )
	{
		sector.add_portal( first_edge_it->second );
		m_portals[ first_edge_it->second ].add_sector( sector.get_id() );
	}
	else if ( current.neighbours[ 0 ] != bsp_tree_triangle::msc_empty_id )
	{
		u32 const neighbour = current.neighbours[ 0 ];
		if ( m_mesh.data[ neighbour ].sector_id == bsp_tree_triangle::msc_empty_id )
		{
			this->add_triangle_to_sector( neighbour, sector );
		}
	}

	edge_to_u32_type::const_iterator const second_edge_it = m_portal_edges.find( edge( second_index, third_index ) );
	if ( second_edge_it != m_portal_edges.end() )
	{
		sector.add_portal( second_edge_it->second );
		m_portals[ second_edge_it->second ].add_sector( sector.get_id() );
	}
	else if ( current.neighbours[ 1 ] != bsp_tree_triangle::msc_empty_id )
	{
		u32 const neighbour = current.neighbours[ 1 ];
		if ( m_mesh.data[ neighbour ].sector_id == bsp_tree_triangle::msc_empty_id )
		{
			this->add_triangle_to_sector( neighbour, sector );
		}
	}

	edge_to_u32_type::const_iterator const third_edge_it = m_portal_edges.find( edge( third_index, first_index ) );
	if ( third_edge_it != m_portal_edges.end() )
	{
		sector.add_portal( third_edge_it->second );
		m_portals[ third_edge_it->second ].add_sector( sector.get_id() );
	}
	else if ( current.neighbours[ 2 ] != bsp_tree_triangle::msc_empty_id )
	{
		u32 const neighbour = current.neighbours[ 2 ];
		if ( m_mesh.data[ neighbour ].sector_id == bsp_tree_triangle::msc_empty_id )
		{
			this->add_triangle_to_sector( neighbour, sector );
		}
	}
}

void sectors_generator::merge_small_sectors( float /*min_square*/ )
{

}

void sectors_generator::add_portal( sector_generator_portal const& portal )
{
	u32 const id = m_portals.size();
	m_portals[ id ] = ( portal );
	m_portals[ id ].set_id( id );
}

bool sectors_generator::look_for_incorrect_adjacency()
{
	edge_to_u32_type edge_count;
	triangle_ids_type::const_iterator const triangles_end = m_triangles.end();
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != triangles_end; ++it )
	{
		u32 const triangle_id = *it;
		++edge_count[ edge( m_mesh.indices[ triangle_id * 3 ],	   m_mesh.indices[ triangle_id * 3 + 1 ] ) ];
		++edge_count[ edge( m_mesh.indices[ triangle_id * 3 + 1 ], m_mesh.indices[ triangle_id * 3 + 2 ] ) ];
		++edge_count[ edge( m_mesh.indices[ triangle_id * 3 + 2 ], m_mesh.indices[ triangle_id * 3 ] ) ];
	}
	for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != triangles_end; ++it )
	{
		u32 const triangle_id = *it;
		for ( u8 i = 0; i < 3; ++i )
		{
			u32 const first_index	=	m_mesh.indices[ triangle_id * 3 + i ];
			u32 const second_index	=	m_mesh.indices[ triangle_id * 3 + ( i + 1 ) % 3 ];
			edge const current_edge ( std::min( first_index,second_index ), std::max( first_index,second_index ) );
			if ( edge_count[ current_edge ] > 2 )
			{
				m_bad_adjacencies[ current_edge ].push_back( bad_adjacency( triangle_id, i ) );
			}
		}
	}
	if ( !m_bad_adjacencies.empty() )
	{
		this->fix_incorrect_adjacency( m_bad_adjacencies );
		for ( triangle_ids_type::const_iterator it = m_triangles.begin(); it != triangles_end; ++it )
		{
			if ( bool incorrect =  !is_triangle_adjacency_correct( *it, m_mesh ) )
			{
				R_ASSERT( incorrect );
				return true;
			}
		}
		m_bad_adjacencies.clear();
	}
	return false;
}

void sectors_generator::fix_incorrect_adjacency( edge_to_bad_adjacencies_type const& bad_adjacencies )
{
	struct best_neighbour_for_triangle : public std::binary_function< bad_adjacency const&, bad_adjacency const&, bool >
	{
		best_neighbour_for_triangle( u32 triangle_id, triangles_mesh_type const& mesh )
			:m_mesh( &mesh ), m_triangle_id( triangle_id ){}
		bool operator()( bad_adjacency const& left, bad_adjacency const& right ) const
		{
			bsp_tree_triangle const& the_triangle	= m_mesh->data[ m_triangle_id ];
			bsp_tree_triangle const& left_triangle	= m_mesh->data[ left.triangle_id ];
			bsp_tree_triangle const& right_triangle = m_mesh->data[ right.triangle_id ];
			float const left_dp		= the_triangle.plane.normal | left_triangle.plane.normal;
			float const right_dp	= the_triangle.plane.normal | right_triangle.plane.normal;
			return !math::is_similar( left_dp, -1.0f, math::epsilon_3 ) && left_dp < right_dp;
		}
	private:
		triangles_mesh_type const*	m_mesh;
		u32							m_triangle_id;
	};
	edge_to_bad_adjacencies_type::const_iterator const bad_adjacencies_end = bad_adjacencies.end();
	for ( edge_to_bad_adjacencies_type::const_iterator it = bad_adjacencies.begin(); it != bad_adjacencies_end; ++it )
	{
		bad_adjacencies_type direct, inverse;
		edge const& current_edge = it->first;
		bad_adjacencies_type::const_iterator const triangles_end = it->second.end();
		for ( bad_adjacencies_type::const_iterator triangle_it = it->second.begin(); triangle_it != triangles_end; ++triangle_it )
		{
			u32 const triangle_id	= triangle_it->triangle_id;
			u32 const edge_id		= triangle_it->edge_id;
			u32 const first_index	= m_mesh.indices[ triangle_id * 3 + edge_id ];
			u32 const second_index	= m_mesh.indices[ triangle_id * 3 + ( edge_id + 1 ) % 3 ];
			if ( current_edge.vertex_index0 == first_index && current_edge.vertex_index1 == second_index )
				direct.push_back( *triangle_it );
			else if ( current_edge.vertex_index1 == first_index && current_edge.vertex_index0 == second_index )
				inverse.push_back( *triangle_it );
			else
				UNREACHABLE_CODE();
		}
		bad_adjacencies_type& outer = direct.size() < inverse.size() ? direct : inverse;
		bad_adjacencies_type& inner = direct.size() < inverse.size() ? inverse : direct;
		while ( !outer.empty() )
		{
			bad_adjacencies_type::iterator best_it = std::min_element( inner.begin(), inner.end(), best_neighbour_for_triangle( outer.back().triangle_id, m_mesh ) );
			R_ASSERT( best_it != inner.end() );
			m_mesh.data[ outer.back().triangle_id ].neighbours[ outer.back().edge_id ]	=	best_it->triangle_id;
			m_mesh.data[ best_it->triangle_id	  ].neighbours[ best_it->edge_id	 ]	=	outer.back().triangle_id;
			inner.erase( best_it );
			outer.pop_back();
		}
		bad_adjacencies_type::iterator const inner_end = inner.end();
		for ( bad_adjacencies_type::iterator inner_it = inner.begin(); inner_it != inner_end; ++inner_it )
		{
			m_mesh.data[ inner_it->triangle_id ].neighbours[ inner_it->edge_id ] = bsp_tree_triangle::msc_empty_id;
		}
	}
}

void sectors_generator::fill_property_container( xray::editor::wpf_controls::property_container^ cont )
{
	using namespace xray::editor::wpf_controls;
	typedef unmanaged_delegate<sectors_generator, u32> select_delegate;
	cont->properties->clear();
	u32 sectors_without_portals = 0;
	spatial_sectors_type::const_iterator const sectors_end = m_sectors.end();
	for ( spatial_sectors_type::const_iterator it = m_sectors.begin(); it != sectors_end; ++it )
	{
		property_container^	sub		= gcnew property_container;

		property_descriptor^ desc	= gcnew property_descriptor( "Triangles", gcnew object_property_value<int>( it->second.get_triangles().size() ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );

		desc						= gcnew property_descriptor( "Square", gcnew object_property_value<float>( it->second.get_square() ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );

		desc						= gcnew property_descriptor( "Portals", gcnew object_property_value<int>( it->second.get_portals().size() ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );

		if ( it->second.get_portals().empty() )
			++sectors_without_portals;
		select_delegate^ select_sector = gcnew select_delegate( this, &sectors_generator::set_active_sector, it->second.get_id() );
		
		desc = cont->properties->add_container(  
			System::String::Format( "Sector {0}", it->second.get_id() ),
			it->second.get_portals().empty() ? "Sectors without portals" : "Sectors",
			System::String::Format( 
				"Triangles {0}, Square {1}, Portals{2}", 
				it->second.get_triangles().size(), 
				it->second.get_square(),
				it->second.get_portals().size()
			),
			sub
		);
		desc->selection_changed += gcnew System::Action<System::Boolean>( select_sector, &select_delegate::invoke );
	}

	u32 portal_index = 0, hanged_count = 0;
	portals_type::const_iterator const portals_end = m_portals.end();
	for ( portals_type::const_iterator it = m_portals.begin(); it != portals_end; ++it )
	{
		property_container^	sub		= gcnew property_container;
		
		property_descriptor^ desc	= gcnew property_descriptor( "Square", gcnew object_property_value<float>( it->second.get_square() ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );

		desc						= gcnew property_descriptor( "Sector 1", gcnew object_property_value<u32>( it->second.get_sectors()[0] ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );
		
		desc	= gcnew property_descriptor( "Sector 2", gcnew object_property_value<u32>( it->second.get_sectors()[1] ) );
		desc->is_read_only			= true;
		desc->select_behavior		= select_behavior::select_parent;
		sub->properties->add( desc );

		select_delegate^ select_portal = gcnew select_delegate( this, &sectors_generator::set_active_portal, portal_index++ );
		desc = cont->properties->add_container( 
			System::String::Format( 
			"Portal {0}",
			it->second.get_id() ),
			!it->second.is_hanged() ? "Portals" : "Hanged portals", 
			System::String::Format( 
				"Square {0}, Sectors({1}, {2})",
				it->second.get_square(),
				it->second.get_sectors()[0],
				it->second.get_sectors()[1]
			), 
			sub
		);
		desc->selection_changed += gcnew System::Action<System::Boolean>( select_portal, &select_delegate::invoke );
		if ( it->second.is_hanged() )
			++hanged_count;
	}
	property_container^	total		= gcnew property_container;

	property_descriptor^ desc	= gcnew property_descriptor( "Portals count", gcnew object_property_value<u32>( m_portals.size() ) );
	desc->is_read_only			= true;
	total->properties->add( desc );

	desc	= gcnew property_descriptor( "Hanged portals count", gcnew object_property_value<u32>( hanged_count ) );
	desc->is_read_only			= true;
	total->properties->add( desc );

	desc	= gcnew property_descriptor( "Sectors count", gcnew object_property_value<u32>( m_sectors.size() ) );
	desc->is_read_only			= true;
	total->properties->add( desc );

	desc	= gcnew property_descriptor( "Sectors without portals count", gcnew object_property_value<u32>( sectors_without_portals ) );
	desc->is_read_only			= true;
	total->properties->add( desc );

	cont->properties->add_container( "Total", "Total", "", total );
}

void sectors_generator::set_active_sector( u32 sector_id, bool active ) 
{ 
	if ( active )
	{
		m_active_sector = sector_id;
		m_active_portal = bsp_tree_triangle::msc_empty_id;
	}
	else if ( m_active_sector == sector_id )
		m_active_sector = bsp_tree_triangle::msc_empty_id;
}

void sectors_generator::set_active_portal( u32 portal_id, bool active )
{
	if ( active )
	{
		m_active_portal = portal_id;
		m_active_sector = bsp_tree_triangle::msc_empty_id;
	}
	else if ( m_active_portal == portal_id )
		m_active_portal = bsp_tree_triangle::msc_empty_id;
}

void sectors_generator::test_are_portals_hanged()
{
	portals_type::iterator const portals_end = m_portals.end();
	for ( portals_type::iterator it = m_portals.begin(); it != portals_end; ++it )
	{
		it->second.test_if_hanged( m_mesh );
		if ( it->second.is_hanged() )
			it->second.try_to_make_seal( m_mesh );
	}
}

static inline bool true_predicate( xray::collision::ray_triangle_result const& ) 
{
	return true;
}

sectors_generator::sector_ids_type sectors_generator::get_portalless_sectors() const
{
	struct sector_square_less: public std::binary_function<u32, u32, bool>
	{
		sector_square_less( spatial_sectors_type const& sectors )
			:m_sectors( &sectors ){}
		bool operator()( u32 left, u32 right ) const
		{
			spatial_sectors_type::const_iterator left_it	= m_sectors->find( left );
			spatial_sectors_type::const_iterator right_it	= m_sectors->find( right );
			return left_it->second.get_square() < right_it->second.get_square();
		}
	private:
		spatial_sectors_type const* m_sectors;
	};

	sector_ids_type portalless_sectors;
	portalless_sectors.reserve( m_sectors.size() );
	spatial_sectors_type::const_iterator const sectors_end = m_sectors.end();
	for ( spatial_sectors_type::const_iterator sector_it = m_sectors.begin(); sector_it != sectors_end; ++sector_it )
	{
		if ( sector_it->second.get_portals().empty() )
			portalless_sectors.push_back( sector_it->second.get_id() );
	}
	std::sort( portalless_sectors.begin(), portalless_sectors.end(), sector_square_less( m_sectors ) );
	return portalless_sectors;
}


void sectors_generator::merge_portalless_sectors()
{
	sector_ids_type const& portalless_sectors = this->get_portalless_sectors();
	sector_ids_type::const_iterator const portalless_end = portalless_sectors.end();
	for ( sector_ids_type::const_iterator sector_it = portalless_sectors.begin(); sector_it != portalless_end; ++sector_it )
	{
		u32 const best_neighbour_to_merge	= this->get_best_neighbour_to_merge( *sector_it );
		if ( best_neighbour_to_merge == bsp_tree_triangle::msc_empty_id )
			continue;
		m_sectors[ best_neighbour_to_merge ].merge( m_sectors[ *sector_it ] );
	}
	this->remove_empty_sectors();
}

u32 sectors_generator::get_best_neighbour_to_merge( u32 sector_id )
{
	struct neigbour_sector
	{
		neigbour_sector()
			:sector_id( bsp_tree_triangle::msc_empty_id ), distance( std::numeric_limits<float>::max() ), has_portals( false ){}
		u32		sector_id;
		float	distance;
		bool	has_portals;
	};

	struct neigbour_result_better : public std::binary_function<neigbour_sector const&, neigbour_sector const&, bool>
	{
		bool operator()( neigbour_sector const& left, neigbour_sector const& right ) const
		{

			if ( left.has_portals && !right.has_portals )
				return true;
			if ( !left.has_portals && right.has_portals )
				return false;
			return ( left.distance < right.distance );
		}
	};

	float const c_max_distance = 10.0f;
	math::float3 const directions[] = {
		math::float3 (  1.0f,  0.0f,  0.0f ),
		math::float3 (  0.0f,  1.0f,  0.0f ),
		math::float3 (  0.0f,  0.0f,  1.0f ),
		math::float3 ( -1.0f,  0.0f,  0.0f ),
		math::float3 (  0.0f, -1.0f,  0.0f ),
		math::float3 (  0.0f,  0.0f, -1.0f )
	};

	spatial_sector& current_sector = m_sectors[ sector_id ];
	math::float3 const& mass_center = current_sector.get_mass_center();
	float distance = std::numeric_limits<float>::max();
	u32 const c_directions_count = sizeof( directions ) / sizeof( directions[ 0 ] );
	neigbour_sector neighbour_results[ c_directions_count ];
	for ( u32 i = 0; i < c_directions_count; ++i )
	{
		collision::ray_triangles_type ray_triangles( g_allocator );
		if ( m_mesh.spatial_tree->ray_query ( NULL, mass_center, directions[ i ], c_max_distance, distance, ray_triangles, true_predicate ) )
		{
			collision::ray_triangles_type::const_iterator const ray_triangles_end = ray_triangles.end();
			collision::ray_triangles_type::const_iterator triangle_it = ray_triangles.begin();
			for ( ; triangle_it != ray_triangles_end; ++triangle_it )
			{
				u32 triangle_id = triangle_it->triangle_id;
				if ( m_mesh.data[ triangle_id ].has_children() )
				{
					math::float3 const& intersection_point = mass_center + directions[ i ] * triangle_it->distance;
					triangle_id = get_actual_triangle( triangle_id, intersection_point, m_mesh );
				}
				bsp_tree_triangle const& triangle = m_mesh.data[ triangle_id ];
				R_ASSERT( triangle.sector_id != bsp_tree_triangle::msc_empty_id );
				if ( triangle.sector_id == current_sector.get_id() )
					continue;
				neighbour_results[ i ].sector_id = triangle.sector_id;
				neighbour_results[ i ].distance	= triangle_it->distance;
				neighbour_results[ i ].has_portals = !m_sectors[ triangle.sector_id ].get_portals().empty();
				if ( neighbour_results[ i ].has_portals )
					break;
			}
		}
	}
	neigbour_sector const* best_neighbour = std::min_element( neighbour_results, neighbour_results + c_directions_count, neigbour_result_better() );
	return best_neighbour->sector_id;
}

void sectors_generator::remove_empty_sectors()
{
	struct is_empty_sector : public std::unary_function<spatial_sectors_type::value_type const&, bool>
	{
		bool operator()( spatial_sectors_type::value_type const& value ) const
		{
			return value.second.is_empty();
		}
	};
	m_sectors.erase(
		std::remove_if( m_sectors.begin(), m_sectors.end(), is_empty_sector() ),
		m_sectors.end()
	);
	R_ASSERT( std::count_if( m_sectors.begin(), m_sectors.end(), is_empty_sector() ) == 0 );
}


} // namespace model_editor
} // namespace xray


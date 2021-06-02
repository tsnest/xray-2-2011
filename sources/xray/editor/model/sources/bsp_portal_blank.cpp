////////////////////////////////////////////////////////////////////////////
//	Created		: 21.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bsp_portal_blank.h"
#include "float3_similar.h"
#include "bsp_tree_utilities.h" 
#pragma managed(push, off)
#include <xray/render/facade/debug_renderer.h>
#pragma managed(pop)

namespace xray {
namespace model_editor {


struct more_to_the_right: public std::binary_function<math::float3 const, math::float3 const, bool>
{
	more_to_the_right( math::float3 const& origin, math::float3 const& plane_normal)
		:m_origin( origin ), m_plane_normal( plane_normal ){}
	bool operator() ( float3 const& first, float3 const& second ) const
	{
		math::float3 const& cross_product = ( first - m_origin ) ^ ( second - m_origin );
        return ( cross_product | m_plane_normal ) < 0.0f;
	}
private:
	math::float3	m_origin;
	math::float3	m_plane_normal;
};


void get_plane_to_convex_intersection_points( math::plane const& p, math::convex const& c, math::convex::cache const& cache, math::convex::cache::vertices_type& result )
{
	math::convex::cache::vertices_type temp;

	typedef math::convex::planes_type planes_type;
	planes_type const& convex_planes = c.get_planes();
	planes_type::const_iterator plane_it = std::find_if( convex_planes.begin(), convex_planes.end(), std::bind2nd( math::convex::plane_similar(), p ) );
	if ( plane_it == convex_planes.end() )
		plane_it = std::find_if( convex_planes.begin(), convex_planes.end(), std::bind2nd( math::convex::plane_similar(),  math::plane( -p.normal, -p.d ) ) );
	if ( plane_it != convex_planes.end() )
	{
		typedef math::convex::adjacencies_type adjacencies_type;
		u32 const plane_id = std::distance( convex_planes.begin(), plane_it );
		R_ASSERT( plane_id < sizeof( adjacencies_type::value_type ) * CHAR_BIT );
		adjacencies_type adjacencies = c.get_adjacencies();
		u32 const plane_mask = 1 << plane_id;
		u32 const adjacency_count = adjacencies.size();
		for ( u32 i = 0; i < adjacency_count; ++i )
		{
			if ( adjacencies[ i ] & plane_mask )
				temp.push_back( cache.vertices[ i ] );
		}
	}
	else
	{
		math::float3 intersection;
		for ( math::convex::cache::edges_type::const_iterator it = cache.edges.begin(); it != cache.edges.end(); ++it )
		{
			if ( p.intersect_segment( cache.vertices[ it->first ], cache.vertices[ it->second ], intersection ) &&
				 std::find_if( temp.begin(), temp.end(), std::bind2nd( float3_similar(), intersection ) ) == temp.end() )
					temp.push_back( intersection );
		}
	}

	if ( !temp.empty() )
		std::sort( temp.begin() + 1, temp.end(), more_to_the_right( temp.front(), p.normal ) );
	u32 const vertex_count = temp.size();
	for ( u32 i = 0; i < vertex_count; ++i )
	{
		u32 const previous	= ( i + vertex_count - 1 ) % vertex_count;
		u32 const next		= ( i +  1 ) % vertex_count;
		if ( !is_between( temp[ i ], temp[ previous ], temp[ next ] ) )
			result.push_back( temp[ i ] );
	}
}

bsp_portal_blank::bsp_portal_blank():
m_square(0.0f)
{
}


void bsp_portal_blank::initialize( math::convex const& c, math::convex::cache const& cache, math::plane const& p )
{
	m_plane = p;
	get_plane_to_convex_intersection_points( m_plane, c, cache, m_vertices );
	this->update_square();
}

void bsp_portal_blank::split ( math::plane const& divider, bsp_portal_blank& positive, bsp_portal_blank& negative ) const
{
	R_ASSERT( divider.valid() );
	positive.m_plane = m_plane;
	negative.m_plane = m_plane;
	u32 const vertices_count = m_vertices.size();
	math::float3 intersection;
	for ( u32 i = 0; i < vertices_count; ++i )
	{
		math::float3 const& current	= m_vertices[ i ];
		math::float3 const& next	= m_vertices[ ( i + 1 ) % vertices_count ];
		float const dist	= divider.classify( current );
		float const next_dist = divider.classify( next );
		if ( math::is_zero( dist ) )
		{
			positive.m_vertices.push_back( current );
			negative.m_vertices.push_back( current );
			continue;
		}
		else if ( ( dist > 0.0f ) )
			positive.m_vertices.push_back( current );
		else 
			negative.m_vertices.push_back( current );
		
		if ( !math::is_zero( next_dist ) && divider.intersect_segment( current, next, intersection ) )
		{
			R_ASSERT( intersection.valid() );
			if ( !math::is_zero( divider.classify( intersection ) ) )
				intersection = divider.project( intersection );
			if ( !is_between( intersection, current, next ) )
				make_point_between( current, next, intersection );
			positive.m_vertices.push_back( intersection );
			negative.m_vertices.push_back( intersection );
		}
	}
	if ( !positive.m_vertices.empty() )
	{
		positive.m_vertices.erase( std::unique( positive.m_vertices.begin(), positive.m_vertices.end() ), positive.m_vertices.end() );
		if ( math::is_similar( positive.m_vertices.front(), positive.m_vertices.back() ) )
			positive.m_vertices.pop_back();
		positive.update_square();
	}
	if ( !negative.m_vertices.empty() )
	{
		negative.m_vertices.erase( std::unique( negative.m_vertices.begin(), negative.m_vertices.end() ), negative.m_vertices.end() );
		if ( math::is_similar( negative.m_vertices.front(), negative.m_vertices.back() ) )
			negative.m_vertices.pop_back();
		negative.update_square();
	}
}

void bsp_portal_blank::limit_by_positive_part( math::plane const& p )
{
	vertices_type positive_vertices;
	u32 const vertices_count = m_vertices.size();
	math::float3 intersection;
	for ( u32 i = 0; i < vertices_count; ++i )
	{
		math::float3 const& current	= m_vertices[ i ];
		math::float3 const& next	= m_vertices[ ( i + 1 ) % vertices_count ];
		float const dist	= p.classify( current );
		float const next_dist = p.classify( next );
		if ( math::is_zero( dist ) )
		{
			positive_vertices.push_back( current );
			continue;
		}
		else if ( ( dist > 0.0f ) )
			positive_vertices.push_back( current );
		
		if ( !math::is_zero( next_dist ) && p.intersect_segment( current, next, intersection ) )
		{
			R_ASSERT( intersection.valid() );
			if ( !math::is_zero( p.classify( intersection ) ) )
				intersection = p.project( intersection );
			if ( !is_between( intersection, current, next ) )
				make_point_between( current, next, intersection );
			positive_vertices.push_back( intersection );
		}
	}
	if ( !positive_vertices.empty() )
	{
		positive_vertices.erase( std::unique( positive_vertices.begin(), positive_vertices.end() ), positive_vertices.end() );
		if ( math::is_similar( positive_vertices.front(), positive_vertices.back() ) )
			positive_vertices.pop_back();
	}
	std::swap( m_vertices, positive_vertices );
	this->update_square();
}

void bsp_portal_blank::update_square()
{
	m_square = 0.0f;
	u32 const vertices_count = m_vertices.size();
	if ( vertices_count < 3 )
		return;
	for ( u32 i = 1; i < vertices_count - 1; ++i )
	{
		u32 const next = ( i + 1 ) % vertices_count;
		m_square += triangle_square( m_vertices.front(), m_vertices[ i ], m_vertices[ next ] );
	}
}

void draw_portal_blank ( render::scene_ptr const& scene, render::debug::renderer& r, bsp_portal_blank const& blank, math::color const& c  )
{
	math::color const plane_color  ( c.r, c.g, c.b, 128 );
	bsp_portal_blank::vertices_type const& vertices = blank.get_vertices();
	u32 const vertices_count = vertices.size();
	if ( vertices_count < 3 )
		return;
	for ( u32 i = 0; i < vertices_count; ++i )
	{
		u32 const next = ( i + 1 ) % vertices_count;
		r.draw_line( scene, vertices[ i ], vertices[ next ], c );
	}
	for ( u32 i = 1; i < vertices_count - 1; ++i )
	{
		math::float3 const& current	= vertices[ i ];
		math::float3 const& next	= vertices[ i + 1 ];
		r.draw_triangle( scene, vertices.front(), current, next, plane_color );
		r.draw_triangle( scene, vertices.front(), next, current, plane_color );
		r.draw_line( scene, vertices.front(), current, c );
	}
}



} // model_editor
} // namespace xray


////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
//  Description	: Convex is a solid figure bounded by several faces, forming a convex volume
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/math_convex.h>
#include <xray/linkage_helper.h>
#include <numeric>
#ifndef XRAY_STATIC_LIBRARIES
	XRAY_DECLARE_LINKAGE_ID(core_math_convex)
#endif // #ifndef XRAY_STATIC_LIBRARIES

using xray::math::convex;
using xray::math::plane;
using xray::math::float4x4;
using xray::math::aabb;
using xray::math::intersection;
using xray::math::float3;
using xray::math::float4;

struct edge_less : public std::binary_function< convex::cache::edges_type::value_type const&, convex::cache::edges_type::value_type const&, bool >
{
	bool operator()( convex::cache::edges_type::value_type const& left, convex::cache::edges_type::value_type const& right )
	{
		return left.first < right.first || ( left.first == right.first && left.second < right.second );
	}
};

static bool three_planes_intersection( plane const& first, plane const& second, plane const& third, float3& intersection )
{
	return
		try_solve_linear_equations_system (
			first.normal,
			second.normal,
			third.normal,
			float3( -first.d, -second.d, -third.d ),
			intersection
		);
}

static u32 get_lowerst_set_bit_index( u32 v, u32 start_from )
{
	u32 result = start_from;
	v >>= start_from;
	while( v && !( v & 1 ) )
	{
		++result;
		v >>= 1;
	}
	if ( !v )
	{
		R_ASSERT( false );
		result = u32( -1 );
	}
	return result;
}

static u32 bits_set_count( u32 v )
{
	R_ASSERT( sizeof( u32 ) == 4 );
	//http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count}
}

convex::convex()
{
}

convex::convex( aabb const& bbox )
{
	m_planes.resize( 6 );
	m_planes[0].plane = math::create_plane( bbox.vertex(0), bbox.vertex(1), bbox.vertex(2) );
	m_planes[1].plane = math::create_plane( bbox.vertex(0), bbox.vertex(2), bbox.vertex(4) );
	m_planes[2].plane = math::create_plane( bbox.vertex(0), bbox.vertex(4), bbox.vertex(1) );
	m_planes[3].plane = math::create_plane( bbox.vertex(7), bbox.vertex(5), bbox.vertex(6) );
	m_planes[4].plane = math::create_plane( bbox.vertex(7), bbox.vertex(3), bbox.vertex(5) );
	m_planes[5].plane = math::create_plane( bbox.vertex(7), bbox.vertex(6), bbox.vertex(3) );
	std::for_each( m_planes.begin(), m_planes.end(), std::mem_fun_ref( &aabb_plane::normalize ) );
	m_adjacency.resize( 8 );
	m_adjacency[0] = ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << 2 );
	m_adjacency[1] = ( 1 << 0 ) | ( 1 << 2 ) | ( 1 << 4 );
	m_adjacency[2] = ( 1 << 0 ) | ( 1 << 1 ) | ( 1 << 5 );
	m_adjacency[3] = ( 1 << 0 ) | ( 1 << 4 ) | ( 1 << 5 );
	m_adjacency[4] = ( 1 << 1 ) | ( 1 << 2 ) | ( 1 << 3 );
	m_adjacency[5] = ( 1 << 2 ) | ( 1 << 3 ) | ( 1 << 4 );
	m_adjacency[6] = ( 1 << 1 ) | ( 1 << 3 ) | ( 1 << 5 );
	m_adjacency[7] = ( 1 << 3 ) | ( 1 << 4 ) | ( 1 << 5 );
}

convex::convex( planes_type const& planes, adjacencies_type const& adjacency ):
m_planes( planes ),
m_adjacency( adjacency )
{

}
void convex::split( plane const& p, cache const& c, convex& positive, convex& negative ) const
{
	R_ASSERT( p.valid() );
	positive.m_adjacency.clear();
	negative.m_adjacency.clear();
	positive.m_planes.clear();
	negative.m_planes.clear();
	if ( std::find_if( m_planes.begin(), m_planes.end(), std::bind2nd( plane_similar(), p ) ) != m_planes.end() )
	{
		negative = *this;
		return;
	}
	if ( std::find_if( m_planes.begin(), m_planes.end(), std::bind2nd( plane_similar(), math::plane( -p.normal, - p.d ) ) ) != m_planes.end() )
	{
		positive = *this;
		return;
	}
	R_ASSERT( m_adjacency.size() == c.vertices.size() );
	R_ASSERT( sizeof( vertex_flags ) * CHAR_BIT >= m_adjacency.max_size() );
	vertex_flags pos_vertex_flags = 0;
	vertex_flags neg_vertex_flags = 0;
	u32 const vertex_count = c.vertices.size();
	for ( u32 i = 0; i < vertex_count; ++i )
	{
		float const dist = p.classify( c.vertices[ i ] );
		if ( math::is_zero( dist ) )
			continue;
		if ( dist > 0.0f ) 
		{
			positive.m_adjacency.push_back( m_adjacency[ i ] );
			pos_vertex_flags |= static_cast<vertex_flags>( 1 << i );
		}
		else
		{
			negative.m_adjacency.push_back( m_adjacency[ i ] );
			neg_vertex_flags |= static_cast<vertex_flags>( 1 << i );
		}
	}
	//R_ASSERT( !positive.m_adjacency.empty() && !negative.m_adjacency.empty() );

	math::float3 intersection;
	cache::edges_type::const_iterator const edge_end_it = c.edges.end();
	for ( cache::edges_type::const_iterator it = c.edges.begin(); it != edge_end_it; ++it )
	{
		math::float3 const& first	= c.vertices[ it->first  ];
		math::float3 const& second	= c.vertices[ it->second ];
		if ( math::is_zero( p.classify( first ) ) && math::is_zero( p.classify( second ) ) )
			continue;
		if ( p.intersect_segment( first, second, intersection ) )
		{
			u32 const edge_adjacency = m_adjacency[ it->first ] & m_adjacency[ it->second ];
			R_ASSERT( bits_set_count( edge_adjacency ) == 2 );
			vertex_flags const edge_vertex_flags = static_cast<vertex_flags>( ( 1 << it->first ) | ( 1 << it->second ) );
			if ( pos_vertex_flags & edge_vertex_flags )
			{
				positive.m_adjacency.push_back( edge_adjacency | ( 1 << m_planes.size() ) );
			}
			if ( neg_vertex_flags & edge_vertex_flags )
			{
				negative.m_adjacency.push_back( edge_adjacency | ( 1 << m_planes.size() ) );
			}
			//if ( ( pos_vertex_flags & edge_vertex_flags ) || ( neg_vertex_flags & edge_vertex_flags ) )
			//{
			//	u32 const first		= get_lowerst_set_bit_index( edge_adjacency, 0 );
			//	u32 const second	= get_lowerst_set_bit_index( edge_adjacency, first + 1 );
			//	math::float3 test_intersection;
			//	R_ASSERT( three_planes_intersection( m_planes[ first ].plane, m_planes[ second ].plane, p, test_intersection ) );
			//}
		}
	}
	u32 const pos_plane_flags = get_plane_flags( positive.m_adjacency );
	original_to_new_plane_id original_to_positive;
	build_original_to_new_adjacency_dict( pos_plane_flags, original_to_positive );

	u32 const neg_plane_flags = get_plane_flags( negative.m_adjacency );
	original_to_new_plane_id original_to_negative;
	build_original_to_new_adjacency_dict( neg_plane_flags, original_to_negative );

	u32 const planes_count = m_planes.size();
	for ( u32 i = 0, mask = 1; i < planes_count; ++i, mask <<= 1 )
	{
		if ( pos_plane_flags & mask )
			positive.m_planes.push_back( m_planes[ i ] );
		if ( neg_plane_flags & mask )
			negative.m_planes.push_back( m_planes[ i ] );
	}

	if ( !positive.empty() )
	{
		aabb_plane new_plane;
		new_plane.plane.vector = -math::float4( p.vector );//normals are directed outwards
		new_plane.normalize();
		original_to_positive[ m_planes.size() ] = static_cast<u8>( positive.m_planes.size() );
		positive.m_planes.push_back( new_plane );
		std::transform( 
			positive.m_adjacency.begin(), 
			positive.m_adjacency.end(), 
			positive.m_adjacency.begin(),
			std::bind1st( std::ptr_fun( &convex::original_to_new_adjacency ), original_to_positive )
		);
		R_ASSERT( std::find_if( positive.m_adjacency.begin(), positive.m_adjacency.end(), std::bind2nd( std::greater<u32>(), ( 1 <<  positive.m_planes.size() ) - 1 ) ) == positive.m_adjacency.end() );
	}

	if ( !negative.empty() )
	{
		aabb_plane new_plane;
		new_plane.plane = p;
		new_plane.normalize();
		original_to_negative[ m_planes.size() ] = static_cast<u8>( negative.m_planes.size() ); 
		negative.m_planes.push_back( new_plane );
		std::transform( 
			negative.m_adjacency.begin(), 
			negative.m_adjacency.end(), 
			negative.m_adjacency.begin(),
			std::bind1st( std::ptr_fun( &convex::original_to_new_adjacency ), original_to_negative )
		);
		R_ASSERT( std::find_if( negative.m_adjacency.begin(), negative.m_adjacency.end(), std::bind2nd( std::greater<u32>(), ( 1 <<  negative.m_planes.size() ) - 1 ) ) == negative.m_adjacency.end() );
	}
}

u32	convex::get_plane_flags( adjacencies_type const& adjacency_vector )
{
	u32 plane_flags = 0;
	adjacencies_type::const_iterator const end_it = adjacency_vector.end();
	for ( adjacencies_type::const_iterator it = adjacency_vector.begin(); it != end_it; ++it )
		plane_flags |= *it;
	return plane_flags;
}

u32 convex::original_to_new_adjacency( original_to_new_plane_id const& dict, u32 original )
{
	R_ASSERT( bits_set_count( original ) >= 3 );
	u32 new_adjacency = 0;
	for ( u32 i = 0, mask = 1; i < msc_max_plane_count && mask <= original; ++i, mask <<= 1 )
		if ( original & mask )
		{
			R_ASSERT( dict[ i ] != u8( -1 ) );
			new_adjacency |= ( 1 << dict[ i ] );
		}
	return new_adjacency;
}

void convex::build_original_to_new_adjacency_dict( u32 plane_flags, original_to_new_plane_id& dict )
{
	for ( u32 i = 0, zero_bit_count = 0; i < msc_max_plane_count; ++i )
	{
		if ( !( plane_flags & ( 1 << i ) ) )
		{
			++zero_bit_count;
			dict[ i ] = u8( -1 );
		}
		else
		{
			dict[ i ] = u8( i - zero_bit_count );
		}
	}
}

intersection convex::test_inexact( aabb const& aabb) const
{
	u32 inside_count				= 0;
	planes_type::const_iterator const end_it = m_planes.end();
	for ( planes_type::const_iterator it = m_planes.begin(); it != end_it; ++it )
	{
		switch ( it->test( aabb ) )
		{
			case intersection_outside	:
				return				intersection_outside;
			case intersection_intersect :
				continue;
			case intersection_inside	: {
				++inside_count;
				continue;
			}
			default	:		NODEFAULT();
		}
	}

	if ( inside_count < m_planes.size() )
		return	intersection_intersect;

	R_ASSERT	( inside_count == m_planes.size() );
	return		intersection_inside;
}

void convex::fill_cache	( cache& c ) const
{
	c.edges.clear();
	c.vertices.clear();
	c.bounding_radius = 0.0f;
	if ( this->empty() )
		return;
	typedef std::set<cache::edge, edge_less> edge_set_type;
	edge_set_type edges;
	u32 const vertex_count = m_adjacency.size();
	for ( u32 i = 0; i < vertex_count; ++i )
	{
		u32 const adjacency = m_adjacency[ i ];
		u32 const first		= get_lowerst_set_bit_index( adjacency, 0 );
		u32 const second	= get_lowerst_set_bit_index( adjacency, first + 1 );
		u32 const third		= get_lowerst_set_bit_index( adjacency, second + 1 );
		R_ASSERT( first != second && first != third && second != third );
		float3 intersection;
		bool success = three_planes_intersection( m_planes[ first ].plane, m_planes[ second ].plane, m_planes[ third ].plane, intersection );
		R_ASSERT( success );
		c.vertices.push_back( intersection );
		for ( u32 j = 0; j < vertex_count; ++j )
		{
			u32 const edge_adjacency = m_adjacency[ i ] & m_adjacency[ j ];
			if ( !edge_adjacency || !( ( edge_adjacency ) & ( edge_adjacency - 1 ) ) || i == j )//0 or 1 bits set or the same vertex
				continue;
			R_ASSERT( bits_set_count( edge_adjacency ) >= 2 );
			edges.insert( std::make_pair( std::min( i, j ), std::max( i, j ) ) );
		}
	}
	c.edges.reserve( edges.size() );
	c.edges.insert( c.edges.end(), edges.begin(), edges.end() );
	math::float3 center_point = std::accumulate( c.vertices.begin(), c.vertices.end(), math::float3( 0.f, 0.f, 0.f ) ) / static_cast<float>( c.vertices.size() );
	cache::vertices_type::const_iterator const end_it = c.vertices.end();
	for ( cache::vertices_type::const_iterator it = c.vertices.begin(); it != end_it; ++it )
	{
		float const sqr_len = ( *it - center_point ).squared_length();
		if ( sqr_len > c.bounding_radius )
			c.bounding_radius = sqr_len;
	}
	c.bounding_radius = math::sqrt( c.bounding_radius );
}

bool convex::inside( float3 const& point ) const
{
	R_ASSERT( !m_planes.empty() );
	planes_type::const_iterator const end_it = m_planes.end();
	for ( planes_type::const_iterator it = m_planes.begin(); it != end_it; ++it )
	{
		float const dist = it->plane.classify( point );
		if ( !math::is_zero( dist ) && dist > 0.f )
			return false;
	}
	return true;
}


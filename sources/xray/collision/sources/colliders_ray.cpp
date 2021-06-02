////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_ray.h"
#include "opcode_include.h"
#include "cell.h"

using xray::collision::colliders::ray;
using xray::collision::detail::cell;
using xray::collision::Predicate;
using xray::collision::Results;
using xray::float3;
using IceMaths::Point;
using xray::collision::query_type;

static inline float invert	( float const value )
{
	if ( xray::math::is_zero( value ) )
		return				( 0.f );

	return					( 1.f / value );
}

ray::ray					(
		query_type const query_type,
		cell const& cell,
		float3 const& origin,
		float3 const& direction,
		float const max_range,
		Results& results,
		Predicate const& predicate
	) :
	m_cell					( cell ),
	m_direction				( direction ),
	m_max_range				( max_range ),
	m_results				( results ),
	m_predicate				( predicate )
{
	( float3& )m_origin		= origin;
	m_origin.padding		= 0.f;

	m_direction.normalize	( );

	// division by zero here is acceptable!
	(float3&)m_inverted_direction	= float3( invert( direction.x ), invert( direction.y ), invert( direction.z ) );
	m_inverted_direction.padding	= 0.f;
}

bool ray::query				()
{
	m_results.clear			( );

	if ( math::abs( m_direction.y ) != 1.f)
		query				( m_cell.root( ) );
	else
		query_vertical		( m_cell.root( ) );

	if ( m_results.empty( ) )
		return				( false );

	struct distance {
		static inline bool	predicate	( result const& left, result const& right )
		{
			return			( left.distance < right.distance );
		}
	};

	std::sort				( m_results.begin( ), m_results.end( ), &distance::predicate );

	Results::const_iterator	i = m_results.begin( );
	Results::const_iterator	e = m_results.end( );
	for ( ; i != e; ++i ) {
		if ( m_predicate( *i ) )
			return			( true );
	}

	return					( false );
}

bool ray::intersects_aabb	(Opcode::AABBNoLeafNode const* node, float& distance)
{
	Point const&			node_center = node->mAABB.mCenter;
	float3					center;
	center					= (float3&)node_center;

	Point const&			node_extents = node->mAABB.mExtents;
	float3					extents;
	extents					= (float3&)node_extents;

	_MM_ALIGN16 sse::aabb_a16	aabb;

	(float3&)aabb.min		= center - extents;
	aabb.min.padding		= 0.f;

	(float3&)aabb.max		= center + extents;
	aabb.max.padding		= 0.f;

	return					( intersects_aabb_sse( aabb, distance ) );
}

bool ray::intersects_aabb_vertical	(Opcode::AABBNoLeafNode const* node)
{
	Point const&			node_center = node->mAABB.mCenter;
	Point const&			node_extents = node->mAABB.mExtents;
	if (node_center.x + node_extents.x < m_origin.x)
		return				(false);  			   	
												   
	if (node_center.z + node_extents.z < m_origin.z)
		return				(false);
	
	if (node_center.x - node_extents.x > m_origin.x)
		return				(false);  			   	
												   
	if (node_center.z - node_extents.z > m_origin.z)
		return				(false);

	return					(true);
}

bool ray::test_triangle		(
		float3 const& v0,
		float3 const& v1,
		float3 const& v2,
		float3 const& position,
		float3 const& direction,
		float const max_range,
		float& range
	)
{
	float3 const edge1		= v1 - v0;
	float3 const edge2		= v2 - v0;
	float3 normal			= direction ^ edge2;
	float const determinant	= edge1 | normal;

	// not culling branch
	if ( math::is_zero( determinant, math::epsilon_5 ) )
		return				( false );

	float3 const			temp = position - v0;
	float const inverted_determinant = 1.f/determinant;
	float const u			= temp.dot_product(normal)*inverted_determinant;
	if ( u < 0.f )
		return				( false );

	if ( u > 1.f )
		return				( false );

	normal					= temp ^ edge1;
	float const v			= direction.dot_product(normal)*inverted_determinant;
	if ( v < 0.f )
		return				( false );

	if ( u + v > 1.f )
		return				( false );

	range					= (edge2 | normal) * inverted_determinant;
	if ( range <= 0.f )
		return				( false );

	if ( range > max_range )
		return				( false );

	return					( true );
}

bool ray::test_triangle		(u32 const& triangle, float& range)
{
	u32 const* const indices		= m_cell.indices(triangle);
	float3 const* const vertices	= m_cell.vertices();
	float3 const& v0				= vertices[indices[0]];
	float3 const& v1				= vertices[indices[1]];
	float3 const& v2				= vertices[indices[2]];
	return							(
		test_triangle(
			v0,
			v1,
			v2,
			m_origin,
			m_direction,
			m_max_range,
			range
		)
	);
}

void ray::test_primitive	(u32 const& triangle)
{
	float					range;
	if ( !test_triangle( triangle, range ) )
		return;

	result const result		= {
		&m_cell,
		triangle,
		range
	};
	m_results.push_back		( result );
}

void ray::query				(Opcode::AABBNoLeafNode const* const node)
{
	_MM_ALIGN16 float		range;
	if ( !intersects_aabb( node, range ) )
		return;

	if ( range > m_max_range )
		return;

	if ( node->HasPosLeaf( ) )
		test_primitive		( node->GetPosPrimitive( ) );
	else
		query				( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		test_primitive		( node->GetNegPrimitive( ) );
	else
		query				( node->GetNeg( ) );
}

void ray::query_vertical	( Opcode::AABBNoLeafNode const* const node )
{
	if ( !intersects_aabb_vertical( node ) )
		return;

	if ( node->HasPosLeaf( ) )
		test_primitive		( node->GetPosPrimitive( ) );
	else
		query_vertical		( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		test_primitive		( node->GetNegPrimitive( ) );
	else
		query_vertical		( node->GetNeg( ) );
}
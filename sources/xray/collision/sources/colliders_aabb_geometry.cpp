////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "colliders_aabb_geometry.h"
#include "opcode_include.h"
#include "triangle_mesh_geometry.h"

using xray::collision::colliders::aabb_geometry;
using xray::collision::triangle_mesh_geometry;
using xray::float3;
using IceMaths::Point;
using xray::collision::object;
using xray::collision::triangles_type;
using xray::collision::query_type;

aabb_geometry::aabb_geometry				(
		triangle_mesh_geometry const& geometry,
		object const* object,
		xray::math::aabb const& aabb,
		triangles_type& triangles
	) :
	super					( aabb ),
	m_object				( object ),
	m_geometry				( geometry ),
	m_triangles				( triangles )
{
	m_center				= ( m_aabb.min + m_aabb.max ) * .5f;
	m_extents				= m_aabb.max - m_center;

	u32 const size_before	= m_triangles.size( );
	query					( m_geometry.root() );
	m_result				= m_triangles.size( ) > size_before;
}

#if XRAY_USE_OLD_TRIANGLE_AABB_TEST
bool aabb_geometry::test_triangle_planes	( float3 const& e0, float3 const& e1, float3 const& v0 ) const
{
	float3 const normal		= e0 ^ e1;
	float const	distance	= -normal | v0;

	float3 min, max;

	if ( normal.x > 0.f ) {
		min.x				= -m_extents.x;
		max.x				=  m_extents.x;
	}
	else {
		min.x				=  m_extents.x;
		max.x				= -m_extents.x;
	}

	if ( normal.y > 0.f ) {
		min.y				= -m_extents.y;
		max.y				=  m_extents.y;
	}
	else {
		min.y				=  m_extents.y;
		max.y				= -m_extents.y;
	}

	if ( normal.z > 0.f ) {
		min.z				= -m_extents.z;
		max.z				=  m_extents.z;
	}
	else {
		min.z				=  m_extents.z;
		max.z				= -m_extents.z;
	}

	if ( ( normal.dot_product( min ) + distance ) > 0.f )
		return				( false );

	if ( ( normal.dot_product( max ) + distance ) >= 0.f )
		return				(true);

	return					( false);
}

#define AXISTEST_X01(a, b, fa, fb)								\
	min = a*v0.y - b*v0.z;										\
	max = a*v2.y - b*v2.z;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.y + fb * m_extents.z;					\
	if( min>rad || max<-rad) return false;


#define AXISTEST_X2(a, b, fa, fb)								\
	min = a*v0.y - b*v0.z;										\
	max = a*v1.y - b*v1.z;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.y + fb * m_extents.z;					\
	if( min>rad || max<-rad) return false;


#define AXISTEST_Y02(a, b, fa, fb)								\
	min = b*v0.z - a*v0.x;										\
	max = b*v2.z - a*v2.x;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.x + fb * m_extents.z;					\
	if( min>rad || max<-rad) return false;


#define AXISTEST_Y1(a, b, fa, fb)								\
	min = b*v0.z - a*v0.x;										\
	max = b*v1.z - a*v1.x;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.x + fb * m_extents.z;					\
	if( min>rad || max<-rad) return false;


#define AXISTEST_Z12(a, b, fa, fb)								\
	min = a*v1.x - b*v1.y;										\
	max = a*v2.x - b*v2.y;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.x + fb * m_extents.y;					\
	if( min>rad || max<-rad) return false;


#define AXISTEST_Z0(a, b, fa, fb)								\
	min = a*v0.x - b*v0.y;										\
	max = a*v1.x - b*v1.y;										\
	if( min>max ) {float const tmp=max; max=min; min=tmp;	}	\
	rad = fa * m_extents.x + fb * m_extents.y;					\
	if( min>rad || max<-rad) return false;

bool aabb_geometry::test_triangle_axis	( float3 const& e0, float3 const& e1, float3 const& v0, float3 const& v1, float3 const& v2) const
{
	float rad;
	float min, max;

	float const fey0 = math::abs(e0.y );
	float const fez0 = math::abs(e0.z );
	AXISTEST_X01(e0.z, e0.y, fez0, fey0);
	float const fex0 = math::abs(e0.x );
	AXISTEST_Y02(e0.z, e0.x, fez0, fex0);
	AXISTEST_Z12(e0.y, e0.x, fey0, fex0);

	float const fey1 = math::abs(e1.y );
	float const fez1 = math::abs(e1.z );
	AXISTEST_X01(e1.z, e1.y, fez1, fey1);
	float const fex1 = math::abs(e1.x );
	AXISTEST_Y02(e1.z, e1.x, fez1, fex1);
	AXISTEST_Z0(e1.y, e1.x, fey1, fex1);

	float3 const e2 = m_leaf_vertices[0] - m_leaf_vertices[2];
	float const fey2 = math::abs(e2.y );
	float const fez2 = math::abs(e2.z );
	AXISTEST_X2(e2.z, e2.y, fez2, fey2);
	float const fex2 = math::abs(e2.x );
	AXISTEST_Y1(e2.z, e2.x, fez2, fex2);
	AXISTEST_Z12(e2.y, e2.x, fey2, fex2);

	return					( true );
}

bool aabb_geometry::test_triangle		( u32 const triangle_id ) const
{
	u32 const* const indices = m_geometry.indices( triangle_id );
	float3 const* const	vertices = m_geometry.vertices( );
	float3 v0				= vertices[ indices[0] ];
	float3 v1				= vertices[ indices[1] ];
	float3 v2				= vertices[ indices[2] ];

	float temp;

	// check x
	temp					= math::min( v0.x, math::min( v1.x, v2.x ) );
	if ( temp > m_aabb.max.x )
		return				( false );

	temp					= math::max( v0.x, math::max( v1.x, v2.x ) );
	if ( temp < m_aabb.min.x )
		return				( false );

	// check y
	temp					= math::min( v0.y, math::min( v1.y, v2.y ) );
	if ( temp > m_aabb.max.y )
		return				( false );

	temp					= math::max( v0.y, math::max( v1.y, v2.y ) );
	if ( temp < m_aabb.min.y )
		return				( false );

	// check z
	temp					= math::min( v0.z, math::min( v1.z, v2.z ) );
	if ( temp > m_aabb.max.z )
		return				( false );

	temp					= math::max( v0.z, math::max( v1.z, v2.z ) );
	if ( temp < m_aabb.min.z )
		return				( false );

	m_leaf_vertices[ 0 ]	= v0;
	m_leaf_vertices[ 1 ]	= v1;
	m_leaf_vertices[ 2 ]	= v2;

	v0						-= m_center;
	v1						-= m_center;
	v2						-= m_center;

	float3 const e0			= v1 - v0;
	float3 const e1			= v2 - v1;
	if ( !test_triangle_planes( e0, e1, v0 ) )
		return				( false );

	return					( test_triangle_axis( e0, e1, v0, v1, v2 ) );
}
#endif // #if XRAY_USE_OLD_TRIANGLE_AABB_TEST

void aabb_geometry::add_triangle			( u32 const triangle_id ) const
{
	m_triangles.push_back	( triangle_result(m_object,triangle_id) );
}

void aabb_geometry::test_primitive		( u32 const triangle_id ) const
{
	if ( !test_triangle( triangle_id ) )
		return;

	add_triangle			( triangle_id );
}

void aabb_geometry::add_triangles		( Opcode::AABBNoLeafNode const* const node ) const
{
	if (!node)
		return;

	if ( node->HasPosLeaf( ) )
		add_triangle		( node->GetPosPrimitive( ) );
	else
		add_triangles		( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		add_triangle		( node->GetNegPrimitive( ) );
	else
		add_triangles		( node->GetNeg( ) );
}

void aabb_geometry::query				( Opcode::AABBNoLeafNode const* const node) const
{
	if ( out_of_bounds( m_aabb, ( float3 const& )node->mAABB.mCenter, ( float3 const& )node->mAABB.mExtents ) )
		return;

	if ( completely_inside( m_aabb, ( float3 const& )node->mAABB.mCenter, ( float3 const& )node->mAABB.mExtents ) ) {
		add_triangles		( node );
		return;
	}

	if ( node->HasPosLeaf( ) )
		test_primitive		( node->GetPosPrimitive( ) );
	else
		query				( node->GetPos( ) );

	if ( node->HasNegLeaf( ) )
		test_primitive		( node->GetNegPrimitive( ) );
	else
		query				( node->GetNeg( ) );
}
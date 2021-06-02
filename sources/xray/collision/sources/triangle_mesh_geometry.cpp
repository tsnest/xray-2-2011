////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_mesh_geometry.h"
#include <xray/render/facade/debug_renderer.h>
#include "opcode_include.h"
#include "colliders_ray_query_geometry.h"
#include "colliders_ray_test_geometry.h"
#include "colliders_aabb_geometry.h"
#include "colliders_cuboid_geometry.h"
#include "colliders_convex_geometry.h"

using xray::math::aabb;
using xray::math::float3;
using xray::math::float4x4;
using xray::math::frustum;
using xray::math::cuboid;
using xray::collision::object;
using xray::collision::query_type;
using xray::collision::ray_triangles_type;
using xray::collision::triangles_type;
using xray::collision::triangles_predicate_type;
using xray::collision::triangle_mesh_geometry;

namespace xray {
namespace collision {

triangle_mesh_geometry::triangle_mesh_geometry	( memory::base_allocator* allocator ) :
	m_triangle_data			( *allocator ),
	m_mesh					( 0 ),
	m_model					( 0 ),
	m_root					( 0 ),
	m_bounding_aabb			( math::create_zero_aabb() )
{
}

void triangle_mesh_geometry::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER	(triangles);
}

void triangle_mesh_geometry::initialize		( memory::base_allocator* allocator, float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count )
{
	ASSERT					( (index_count % 3) == 0 );

	m_mesh					= XRAY_NEW_IMPL ( allocator, Opcode::MeshInterface )( allocator );
	m_mesh->SetNbTriangles	( index_count / 3 );
	m_mesh->SetNbVertices	( vertex_count );
	m_mesh->SetPointers		( ( IndexedTriangle const* )indices, ( Point const* )vertices );

	Opcode::OPCODECREATE	options;
	options.mIMesh			= m_mesh;
	options.mSettings.mLimit= 1;
	options.mSettings.mRules= Opcode::SPLIT_SPLATTER_POINTS | Opcode::SPLIT_GEOM_CENTER;
	options.mKeepOriginal	= false;
	options.mNoLeaf			= true;
	options.mQuantized		= false;
	options.mCanRemap		= false;

	m_model					= XRAY_NEW_IMPL ( allocator, Opcode::Model )( allocator );
	m_model->Build			( options );

	m_root					= static_cast_checked<Opcode::AABBNoLeafTree const*>( m_model->GetTree( ) )->GetNodes( );

	calculate_aabb			( vertices, vertex_count, indices, index_count );
}

void triangle_mesh_geometry::destroy	( memory::base_allocator* allocator )
{
	XRAY_DELETE_IMPL		( allocator, m_model );
	XRAY_DELETE_IMPL		( allocator, m_mesh );
}

triangle_mesh_geometry::~triangle_mesh_geometry		( )
{
}

Opcode::AABBNoLeafNode const* triangle_mesh_geometry::root	( ) const
{
	ASSERT					( m_root );
	return					( m_root );
}

bool triangle_mesh_geometry::aabb_test		( aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER	(aabb);
	return true;
}

bool triangle_mesh_geometry::cuboid_test	( cuboid const& cuboid ) const
{
	XRAY_UNREFERENCED_PARAMETER	(cuboid);
	return true;
}

struct helper : private boost::noncopyable {
	inline	helper	( float& distance, bool& result ) :
		m_distance	( distance ),
		m_result	( result )
	{
	}

	bool predicate	( xray::collision::ray_triangle_result const& triangle ) const
	{
		if ( triangle.distance > m_distance )
			return	false; 

		m_distance	= triangle.distance;
		m_result	= true;
		return		false;
	}

private:
	float&			m_distance;
	bool&			m_result;
};

bool triangle_mesh_geometry::ray_test		(
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance
	) const
{
	bool result				= false;
	distance				= max_distance;
	helper					distance_helper( distance, result );
	colliders::ray_test_geometry(
		*this,
		origin,
		direction,
		max_distance,
		triangles_predicate_type( &distance_helper, &helper::predicate )
	);
	return					result;
}

void triangle_mesh_geometry::render			( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer, float4x4 const& matrix ) const
{
	//u32 const vertex_buffer_size	= (m_mesh->GetNbVertices())*sizeof(vertex_colored);
	//u32 const index_buffer_size		= (m_mesh->GetNbIndices())*sizeof(u16);
	//bool allocated_in_stack			= false;
	//vertex_colored* vertices		= 0;
	//u16* indices					= 0;
	//if ( vertex_buffer_size + index_buffer_size < 900*1024 ) {
	//	allocated_in_stack			= true;
	//	vertex_colored* vertices	= pointer_cast<vertex_colored*>( ALLOCA(vertex_buffer_size) );
	//	u16* indices				= pointer_cast<u16*>( ALLOCA(index_buffer_size) );
	//}
	//else {
	//	UNREACHABLE_CODE();
	//}

	IceMaths::Point const* const	vertices = m_mesh->GetVerts( );
	IndexedTriangle const* const b	= ( IndexedTriangle const* )m_mesh->GetTris( );
	IndexedTriangle const* i		= b;
	IndexedTriangle const* const e	= b + m_mesh->GetNbTriangles( );
	for ( ; i != e; ++i ) {
		//for ( u32 j=0; j<3; ++j ) {

		//}
		float3 const positions[]	= {
			matrix.transform_position( float3(
				vertices[ (*i).mVRef[0] ].x,
				vertices[ (*i).mVRef[0] ].y,
				vertices[ (*i).mVRef[0] ].z
			)),
			matrix.transform_position( float3(
				vertices[ (*i).mVRef[1] ].x,
				vertices[ (*i).mVRef[1] ].y,
				vertices[ (*i).mVRef[1] ].z
			)),
			matrix.transform_position( float3(
				vertices[ (*i).mVRef[2] ].x,
				vertices[ (*i).mVRef[2] ].y,
				vertices[ (*i).mVRef[2] ].z
			)),
		};

		math::color clr(0, 127, 0, 127);
		using xray::render::vertex_colored;
		vertex_colored const draw_vertices[]=
		{
			vertex_colored(
				positions[0],
				clr
			),
			vertex_colored(
				positions[1],
				clr
			),
			vertex_colored(
				positions[2],
				clr
			)
		};
		renderer.draw_triangle	( scene, draw_vertices );
	}
}

bool triangle_mesh_geometry::aabb_query		( object const* object, aabb const& aabb, triangles_type& triangles ) const
{
	return					colliders::aabb_geometry( *this, object, aabb, triangles ).result( );
}

bool triangle_mesh_geometry::cuboid_query	( object const* object, cuboid const& cuboid, triangles_type& triangles ) const
{
	return					colliders::cuboid_geometry	( *this, object, cuboid, triangles ).result( );
}

u32 triangle_mesh_geometry::get_triangles_count		( math::convex const& bounding_convex ) const
{
	return					colliders::convex_geometry( *this, bounding_convex ).result();
}


bool triangle_mesh_geometry::ray_query		(
		object const* object,
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance,
		ray_triangles_type& triangles,
		triangles_predicate_type const& predicate
	) const
{
	XRAY_UNREFERENCED_PARAMETER	(distance);

	bool const result		= colliders::ray_query_geometry( *this, object, origin, direction, max_distance, triangles, predicate ).result( );
	if ( !result )
		return				( false );

	return					( true );
}

void triangle_mesh_geometry::calculate_aabb	( float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count )
{
	XRAY_UNREFERENCED_PARAMETERS	(indices, index_count);

	ASSERT					( vertex_count );
	if ( !vertex_count )
		return;

	float3 const* i			= vertices;
	float3 const* const e	= vertices + vertex_count;

	float3 min				= *i;
	float3 max				= *i;
	for ( ++i; i != e; ++i ) {
		min					= math::min( min, *i );
		max					= math::max( max, *i );
	}

	m_bounding_aabb.min		= min;
	m_bounding_aabb.max		= max;
}

aabb triangle_mesh_geometry::get_aabb( ) const
{
	return					m_bounding_aabb;
}

float3 triangle_mesh_geometry::get_random_surface_point ( math::random32& randomizer ) const
{
	XRAY_UNREFERENCED_PARAMETER	( randomizer );
	UNREACHABLE_CODE			( return float3() );
}

float triangle_mesh_geometry::get_surface_area	( ) const
{
	// TODO: calculate real area as sum of triangles
	NOT_IMPLEMENTED				( return 0.f );
}

float3 const* triangle_mesh_geometry::vertices				( ) const
{
	return					( ( float3 const* )m_mesh->GetVerts( ) );
}

u32 triangle_mesh_geometry::vertex_count					( ) const
{
	return					( m_mesh->GetNbVertices( ) );
}

u32 const* triangle_mesh_geometry::indices					( ) const
{
	return					&m_mesh->GetTris( )->mVRef[0];
}

u32 const* triangle_mesh_geometry::indices					( u32 const triangle_id ) const
{
	return					( m_mesh->GetTris( )[ triangle_id ].mVRef );
}

u32 triangle_mesh_geometry::index_count						( ) const
{
	return					( 3*m_mesh->GetNbTriangles( ) );
}

//void triangle_mesh_geometry::accept	( geometry_double_dispatcher& dispatcher, geometry const& node ) const
//{
//	XRAY_UNREFERENCED_PARAMETERS		( &dispatcher, &node );
//	NOT_IMPLEMENTED( );
//}
//

//void triangle_mesh_geometry::visit	( geometry_double_dispatcher& dispatcher, box_geometry const& node ) const
//{
//	XRAY_UNREFERENCED_PARAMETERS		( &dispatcher, &node );
//	NOT_IMPLEMENTED( );
//}
//
//void triangle_mesh_geometry::visit	( geometry_double_dispatcher& dispatcher, sphere_geometry const& node ) const
//{
//	XRAY_UNREFERENCED_PARAMETERS		( &dispatcher, &node );
//	NOT_IMPLEMENTED( );
//}
//
//void triangle_mesh_geometry::visit	( geometry_double_dispatcher& dispatcher, cylinder_geometry const& node ) const
//{
//	XRAY_UNREFERENCED_PARAMETERS		( &dispatcher, &node );
//	NOT_IMPLEMENTED( );
//}

} // namespace collision
} // namespace xray
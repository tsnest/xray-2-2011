////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "triangle_mesh_base.h"
#include "opcode_include.h"
#include "colliders_ray_query_geometry.h"
#include "colliders_ray_test_geometry.h"
#include "colliders_aabb_geometry.h"
#include "colliders_cuboid_geometry.h"
#include "sphere_geometry.h"

#include "triangle_sphere_contact_tests.h"
#include "triangle_box_contact_tests.h"
#include "triangle_cylinder_contact_tests.h"

#include <xray/collision/contact_info.h>
#include <xray/render/base/debug_renderer.h>

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
using xray::collision::triangle_mesh_base;
using xray::memory::base_allocator;

triangle_mesh_base::triangle_mesh_base	( base_allocator* allocator )	:
	m_allocator				( allocator ),
	m_triangle_data			( *allocator ),
	m_mesh					( 0 ),
	m_model					( 0 ),
	m_root					( 0 )
{
}

void triangle_mesh_base::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER	(triangles);
}

void triangle_mesh_base::initialize		( float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count )
{
	ASSERT					( (index_count % 3) == 0 );

	m_mesh					= XRAY_NEW_IMPL ( m_allocator, Opcode::MeshInterface )( m_allocator );
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

	m_model					= XRAY_NEW_IMPL ( m_allocator, Opcode::Model )( m_allocator );
	m_model->Build			( options );

	m_root					= dynamic_cast<Opcode::AABBNoLeafTree const*>( m_model->GetTree( ) )->GetNodes( );

	calculate_aabb			( vertices, vertex_count, indices, index_count );
}

triangle_mesh_base::~triangle_mesh_base		( )
{
	XRAY_DELETE_IMPL		( m_allocator, m_model );
	XRAY_DELETE_IMPL		( m_allocator, m_mesh );
}

Opcode::AABBNoLeafNode const* triangle_mesh_base::root	( ) const
{
	ASSERT					( m_root );
	return					( m_root );
}

float3 const* triangle_mesh_base::vertices				( ) const
{
	return					( ( float3 const* )m_mesh->GetVerts( ) );
}

u32 const* triangle_mesh_base::indices					( u32 const triangle_id ) const
{
	return					( m_mesh->GetTris( )[ triangle_id ].mVRef );
}

bool triangle_mesh_base::aabb_test		( aabb const& aabb ) const
{
	XRAY_UNREFERENCED_PARAMETER	(aabb);
	return true;
}

bool triangle_mesh_base::cuboid_test	( cuboid const& cuboid ) const
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
		//m_distance	= xray::math::min( m_distance, triangle.distance );
		if( triangle.distance > m_distance )
			return	false; 

		m_distance	= triangle.distance;
		m_result	= true;
		return		false;
	}

private:
	float&			m_distance;
	bool&			m_result;
};

bool triangle_mesh_base::ray_test		(
		float3 const& origin,
		float3 const& direction,
		float max_distance,
		float& distance
	) const
{
	bool result				= false;
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

void triangle_mesh_base::render			( xray::render::debug::renderer& renderer, float4x4 const& matrix ) const
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

		float const step			= 10.f;
		float const inverted_step	= 1.f / step;

		using xray::render::vertex_colored;
		vertex_colored const draw_vertices[]=
		{
			{
				positions[0],
				math::color(
					0*( math::clamp_r( ( positions[0].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					1*( math::clamp_r( ( positions[0].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					0*( math::clamp_r( ( positions[0].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					255.0f
				).get_d3dcolor()
			},
			{
				positions[1],
				math::color(
					0*( math::clamp_r( ( positions[1].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					1*( math::clamp_r( ( positions[1].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					0*( math::clamp_r( ( positions[1].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					255.0f
				).get_d3dcolor()
			},
			{
				positions[2],
				math::color(
					0*( math::clamp_r( ( positions[2].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					1*( math::clamp_r( ( positions[2].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					0*( math::clamp_r( ( positions[2].y - step*.5f) * inverted_step * 127.f + 127, 0.f, 255.f) ),
					255.0f
				).get_d3dcolor()
			}
		};
		renderer.draw_triangle	( draw_vertices );
	}
}

bool triangle_mesh_base::aabb_query		( object const* object, aabb const& aabb, triangles_type& triangles ) const
{
	return					colliders::aabb_geometry( *this, object, aabb, triangles ).result( );
}

bool triangle_mesh_base::cuboid_query	( object const* object, cuboid const& cuboid, triangles_type& triangles ) const
{
	return					colliders::cuboid_geometry	( *this, object, cuboid, triangles ).result( );
}

bool triangle_mesh_base::ray_query		(
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

void triangle_mesh_base::calculate_aabb	( float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count )
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

aabb&	triangle_mesh_base::get_aabb( aabb& result ) const
{
	result					= m_bounding_aabb;
	return					result;
}
void		triangle_mesh_base::generate_contacts( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const xray::collision::geometry& og )const
{
	og.generate_contacts( c, transform, self_transform, *this );
}
void	triangle_mesh_base::generate_contacts( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const xray::collision::sphere_geometry& og ) const 
{
	t_generate_contact_primitive<detail::triangle_sphere_contact_tests>( c, self_transform,  transform, og );
}

void triangle_mesh_base::generate_contacts( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const xray::collision::box_geometry& og )const
{
	t_generate_contact_primitive<detail::triangle_box_contact_tests>( c, self_transform,  transform, og );
}

void triangle_mesh_base::generate_contacts( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const xray::collision::cylinder_geometry& og )const
{
	t_generate_contact_primitive<detail::triangle_cylinder_contact_tests>( c, self_transform,  transform, og );
}

void triangle_mesh_base::generate_contacts ( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const xray::collision::triangle_mesh_base& og ) const
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

void triangle_mesh_base::generate_contacts ( xray::collision::on_contact& c, const float4x4 &self_transform, const float4x4 &transform,  const xray::collision::compound_geometry& og ) const	
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
}

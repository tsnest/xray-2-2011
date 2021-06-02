////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "compound_geometry.h"

using xray::memory::base_allocator;

namespace xray {
namespace collision {

compound_geometry::compound_geometry		( memory::base_allocator* allocator, vectora<collision::geometry_instance*> &instances ):
m_allocator(allocator), m_geometry_instances( instances )
{
	
}

compound_geometry::~compound_geometry		( )			
{

}

void	compound_geometry::add_geometry			( non_null<collision::geometry_instance>::ptr g_instance  )
{
	m_geometry_instances.push_back( &(*g_instance) );
}

math::aabb&	compound_geometry::get_aabb( math::aabb& result ) const
{
	result.invalidate();
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		result.modify( (*i)->get_aabb() );
	return result;
}

void compound_geometry::render( render::debug::renderer& renderer, float4x4 const& matrix ) const
{
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
	{
		float4x4 m = (*i)->get_matrix() * matrix;
		(*i)->get_geometry()->render( renderer, m );
	}
}
math::float3 const* compound_geometry::vertices	( ) const 
{ 
	NOT_IMPLEMENTED(return 0);
}

u32 const*	compound_geometry::indices( u32 triangle_id ) const 
{
	XRAY_UNREFERENCED_PARAMETER (triangle_id);
	NOT_IMPLEMENTED(return 0);
}

bool compound_geometry::aabb_query	( object const* object, math::aabb const& aabb, triangles_type& triangles ) const
{ 
	bool res = false;
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		res = (*i)->aabb_query( object, aabb, triangles ) || res;
	return res; 
}

bool compound_geometry::cuboid_query( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const
{ 
	bool res = false;
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		res = (*i)->cuboid_query( object, cuboid, triangles ) || res;
	return res; 
};

bool compound_geometry::ray_query(
						object const* object,
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						float& distance,
						ray_triangles_type& triangles,
						triangles_predicate_type const& predicate
					) const				
{ 
	bool res = false;
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		res = (*i)->ray_query( object, origin, direction, max_distance, distance, triangles, predicate ) || res;
	return res; 
}

bool compound_geometry::aabb_test( math::aabb const& aabb ) const
{
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		if( (*i)->aabb_test( aabb ) )
			return true;
	return false;
}
bool compound_geometry::cuboid_test			( math::cuboid const& cuboid ) const
{
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		if( (*i)->cuboid_test( cuboid ) )
			return true;
	return false;

}
bool compound_geometry::ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const
{
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
		if( (*i)->ray_test( origin, direction, max_distance, distance ) )
			return true;
	return false;
}

void compound_geometry::add_triangles		( triangles_type& triangles ) const
{
	XRAY_UNREFERENCED_PARAMETER (triangles);
}

void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )		const 
{ 
	//og.generate_contacts( c, transform, self_transform, this
	vectora<collision::geometry_instance*>::const_iterator i = m_geometry_instances.begin(), e = m_geometry_instances.end();
	for( ; i!=e; ++i )
	{
		const geometry &g = *((*i)->get_geometry());
		og.generate_contacts( c, transform, (*i)->get_matrix() * self_transform, g ); 
	}

};

void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const sphere_geometry& og )			const
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
	UNREACHABLE_CODE();
}

void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )				const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
	UNREACHABLE_CODE();
};
void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og )		const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
	UNREACHABLE_CODE();
};
void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const triangle_mesh_base& og )	const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
	UNREACHABLE_CODE();
};
void compound_geometry::generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const compound_geometry& og )		const 
{
	XRAY_UNREFERENCED_PARAMETERS	(&c, &self_transform, &transform, &og);
	UNREACHABLE_CODE();
}

//math::float3 const* compound_geometry::vertices	( ) const
//{
//	NOT_IMPLEMENTED(return 0);
//}
//
//u32 const* compound_geometry::indices			( u32 triangle_id ) const
//{
//	NOT_IMPLEMENTED(return 0);
//}

}//collision
}//xray
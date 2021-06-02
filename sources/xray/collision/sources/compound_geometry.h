////////////////////////////////////////////////////////////////////////////
//	Created		: 26.05.2009
//	Author		: Konstantin Slipchenko
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRY_COMPOUND_H_INCLUDED
#define GEOMETRY_COMPOUND_H_INCLUDED

#include <xray/collision/geometry.h>
#include <xray/collision/geometry_instance.h>
namespace xray {
namespace collision {

class compound_geometry:
public geometry,
private boost::noncopyable
{
public:
						compound_geometry		( memory::base_allocator* allocator, vectora<collision::geometry_instance*> &instances );
	virtual				~compound_geometry		( )	;

	virtual	math::aabb&	get_aabb				( math::aabb& result ) const;
			void		add_geometry			( non_null<collision::geometry_instance>::ptr g_instance  );
private:
	virtual	void		render					( render::debug::renderer& renderer, float4x4 const& matrix ) const;
	virtual	math::float3 const* vertices		( ) const;
	virtual	u32 const*	indices					( u32 triangle_id ) const;

	virtual	bool		aabb_query				( object const* object, math::aabb const& aabb, triangles_type& triangles ) const ;
	virtual	bool		cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool		ray_query				(
							object const* object,
							float3 const& origin,
							float3 const& direction,
							float max_distance,
							float& distance,
							ray_triangles_type& triangles,
							triangles_predicate_type const& predicate
						) const				;

	virtual	bool		aabb_test			( math::aabb const& aabb ) const;
	virtual	bool		cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool		ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void		add_triangles		( triangles_type& triangles ) const;

private:
	virtual	memory::base_allocator&	get_allocator ( ) const { return *m_allocator; }

public:
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const sphere_geometry& og )			const;

private:
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )		const ;
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )				const ;
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og )		const ;
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const triangle_mesh_base& og )	const ;
	virtual	void		generate_contacts	( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const compound_geometry& og )		const ;
private:
mutable memory::base_allocator*				m_allocator;
vectora<collision::geometry_instance*>		m_geometry_instances;
}; // class compound_geometry

}//collision
}//xray
#endif // #ifndef GEOMETRY_COMPOUND_H_INCLUDED
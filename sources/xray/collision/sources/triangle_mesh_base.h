////////////////////////////////////////////////////////////////////////////
//	Created		: 20.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef TRIANGLE_MESH_BASE_H_INCLUDED
#define TRIANGLE_MESH_BASE_H_INCLUDED

#include <xray/collision/geometry.h>
#include <xray/collision/object.h>
#include "colliders_aabb_geometry.h"

namespace Opcode {
	class AABBNoLeafNode;
	class MeshInterface;
	class Model;
} // namespace Opcode

namespace xray {
namespace collision {

class triangle_mesh_base :
	public collision::geometry,
	private boost::noncopyable
{
public:
						triangle_mesh_base		( memory::base_allocator* allocator );
	virtual				~triangle_mesh_base		( );
			void		initialize				( float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count );
	
	virtual	math::float3 const* vertices		( ) const;
	virtual	u32 const*			indices			( u32 triangle_id ) const;
	
	Opcode::AABBNoLeafNode const* root			( ) const;
	inline Opcode::MeshInterface* mesh			( ) const { return m_mesh; }

	virtual	void		add_triangles			( triangles_type& triangles ) const;

	virtual u32 		get_custom_data			( u32 triangle_id ) const { return m_triangle_data[triangle_id]; }

	virtual	bool		aabb_query				( object const* object, math::aabb const& aabb, triangles_type& triangles ) const;
	virtual	bool		cuboid_query			( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const;
	virtual	bool		ray_query				(
							object const* object,
							float3 const& origin,
							float3 const& direction,
							float max_distance,
							float& distance,
							ray_triangles_type& triangles,
							triangles_predicate_type const& predicate
						) const;

	virtual	bool		aabb_test				( math::aabb const& aabb ) const;
	virtual	bool		cuboid_test				( math::cuboid const& cuboid ) const;
	virtual	bool		ray_test				( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void		render					( render::debug::renderer& renderer, float4x4 const& matrix ) const;
	virtual	math::aabb&	get_aabb				( math::aabb& result ) const;
	virtual	memory::base_allocator&	get_allocator ( ) const { return *m_allocator; }

public:
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::geometry& og )					const;
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::sphere_geometry& og )			const;
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const box_geometry& og )						const;
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const cylinder_geometry& og )					const;
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const collision::triangle_mesh_base& og )		const;
	virtual	void		generate_contacts		( on_contact& c, const float4x4 &self_transform, const float4x4 &transform,  const compound_geometry& og )					const;

private:
	template < class ContactTests >
			void	t_generate_contact_primitive( on_contact& c, const float4x4 &self_transform, const float4x4 &transform, const typename ContactTests::geometry_type& og )const;

private:
			void		calculate_aabb			( float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count );

protected:
	vectora<u32>		m_triangle_data;

private:
	math::aabb 						m_bounding_aabb;
	mutable memory::base_allocator*	m_allocator;
	Opcode::MeshInterface*			m_mesh;
	Opcode::Model*					m_model;
	Opcode::AABBNoLeafNode const*	m_root;
}; // class world

} // namespace collision
} // namespace xray

#include "triangle_mesh_base_inline.h"
#endif // #ifndef TRIANGLE_MESH_BASE_H_INCLUDED
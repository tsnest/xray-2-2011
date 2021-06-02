////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_TRIANGLE_MESH_GEOMETRY_H_INCLUDED
#define XRAY_COLLISION_TRIANGLE_MESH_GEOMETRY_H_INCLUDED

#include <xray/collision/geometry.h>
#include <xray/collision/common_types.h>
#include <xray/render/engine/base_classes.h>

namespace Opcode {
	class AABBNoLeafNode;
	class MeshInterface;
	class Model;
} // namespace Opcode

namespace xray {

namespace math {
	class convex;
	class random32;
} // namespace math

namespace render {
namespace debug {
	class render;
} // namespace debug
} // namespace render

namespace collision {

class triangle_mesh_geometry : public geometry
{
public:
						triangle_mesh_geometry	( memory::base_allocator* allocator );
	virtual				~triangle_mesh_geometry	( );
	
	virtual	void		destroy					( memory::base_allocator* allocator );

			void		initialize				( memory::base_allocator* allocator, float3 const* vertices, u32 vertex_count, u32 const* indices, u32 index_count );
	
	virtual	math::float3 const* vertices		( ) const;
	virtual	u32			vertex_count			( ) const;
	virtual	u32 const*	indices					( ) const;
	virtual	u32 const*	indices					( u32 triangle_id ) const;
	virtual	u32			index_count				( ) const;
	
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

	virtual	u32			get_triangles_count		( math::convex const& bounding_convex ) const;

	virtual	void		render					( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& matrix ) const;
	virtual	math::aabb	get_aabb				( ) const;
	virtual	float3		get_random_surface_point( math::random32& randomizer ) const;
	virtual	float		get_surface_area		( ) const;

	//virtual void		accept					( geometry_double_dispatcher& dispatcher, geometry const& node ) const;
	//virtual void		visit					( geometry_double_dispatcher& dispatcher, box_geometry const& node ) const;
	//virtual void		visit					( geometry_double_dispatcher& dispatcher, sphere_geometry const& node ) const;
	//virtual void		visit					( geometry_double_dispatcher& dispatcher, cylinder_geometry const& node ) const;

public:
	virtual	void		enumerate_primitives	( enumerate_primitives_callback&  ) const {};
	virtual	void		enumerate_primitives	( float4x4 const&, enumerate_primitives_callback& ) const {};

private:
			void		calculate_aabb			( float3 const* const vertices, u32 const vertex_count, u32 const* const indices, u32 const index_count );

protected:
	vectora<u32>		m_triangle_data;

private:
	math::aabb 						m_bounding_aabb;
	Opcode::MeshInterface*			m_mesh;
	Opcode::Model*					m_model;
	Opcode::AABBNoLeafNode const*	m_root;
}; // class triangle_mesh_geometry

//typedef	resources::resource_ptr<
//	triangle_mesh_geometry,
//	resources::unmanaged_intrusive_base
//> triangle_mesh_geometry_ptr;

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_TRIANGLE_MESH_GEOMETRY_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef AI_COLLISION_OBJECT_H_INCLUDED
#define AI_COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/collision_object.h>
#include <xray/ai/collision_object.h>

namespace stalker2 {

class ai_collision_object :
	public collision::object,
	public ai::collision_object,
	private boost::noncopyable
{
public:
					ai_collision_object	(
						non_null< collision::geometry >::ptr geometry,
						float4x4 const& transform,
						ai::game_object& ai_object,
						u32 object_type,
						pvoid user_data
					);

					ai_collision_object	(
						non_null< collision::geometry_instance >::ptr geometry,
						ai::game_object& ai_object,
						u32 object_type,
						pvoid user_data
					);

	virtual	void	render				( xray::render::scene_ptr const& scene, xray::render::debug::renderer& renderer ) const;

	virtual	bool	aabb_query			( math::aabb const& aabb, collision::triangles_type& triangles ) const;
	virtual	bool	cuboid_query		( math::cuboid const& cuboid, collision::triangles_type& triangles ) const;
	virtual	bool	ray_query			(
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						float& distance,
						collision::ray_triangles_type& triangles,
						collision::triangles_predicate_type const& predicate
					) const;

	virtual	bool	aabb_test			( math::aabb const& aabb ) const;
	virtual	bool	cuboid_test			( math::cuboid const& cuboid ) const;
	virtual	bool	ray_test			(
						float3 const& origin,
						float3 const& direction,
						float max_distance,
						float& distance
					) const;

	virtual	void	add_triangles		( collision::triangles_type& triangles ) const;
	virtual	math::aabb	update_aabb		( float4x4 const& local_to_world );

	virtual	ai::game_object&		get_game_object	( )	const	{ return m_ai_object; }
	virtual	float3					get_origin		( ) const	{ return get_aabb().center(); }

	static	non_null< ai_collision_object >::ptr	new_ai_geometry_object	(
														memory::base_allocator* allocator,
														ai::game_object* ai_game_object,
														u32 const object_type,
														float4x4 const& matrix,
														non_null< collision::geometry >::ptr geometry,
														pvoid user_data
													);

	static	non_null< ai_collision_object >::ptr	new_ai_geometry_object	(
														memory::base_allocator* allocator,
														ai::game_object* ai_game_object,
														u32 const object_type,
														non_null< collision::geometry_instance >::ptr geometry,
														pvoid user_data
													);

	static	void	delete_ai_collision_object		( memory::base_allocator* allocator, ai_collision_object* object );

	collision::geometry_instance const&	get_geom_instance( ) const { return m_geometry_instance; }
private:
	collision::geometry_instance&	m_geometry_instance;
	ai::game_object&				m_ai_object;
}; // class ai_collision_object

} // namespace stalker2

#endif // #ifndef AI_COLLISION_OBJECT_H_INCLUDED

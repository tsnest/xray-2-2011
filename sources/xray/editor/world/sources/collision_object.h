////////////////////////////////////////////////////////////////////////////
//	Created		: 19.02.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_COLLISION_OBJECT_H_INCLUDED
#define EDITOR_COLLISION_OBJECT_H_INCLUDED

#pragma unmanaged
#include <xray/collision/object.h>
#include <xray/collision/geometry_instance.h>
#pragma managed

#include "transform_control_base.h"

namespace xray {

namespace collision {
	struct engine;
	struct geometry;
} // namespace collision

namespace editor {

class object;

enum	collision_objects_priority{
	collision_objects_priority_highest	=	0,
	collision_objects_priority_controls	=	10,
	collision_objects_priority_dynamics	=	20
};

enum enum_collision_object_type {
//	collision_type_base			= u8(1<<0),
	collision_type_dynamic		= (1<<1),
	collision_type_control		= (1<<2),
	collision_type_terrain		= (1<<3),
	collision_type_ALL			= u32(-1)
};


class collision_object : public xray::collision::object
{
public:
					collision_object	( collision::geometry const* geometry );
	virtual			~collision_object	( );

private:
					collision_object	( collision_object const& );
	collision_object& operator =		( collision_object const& );

public:
	virtual	void	render				( xray::render::debug::renderer& renderer ) const;
	virtual	bool	aabb_query			( math::aabb const& aabb, xray::collision::triangles_type& results ) const;	
	virtual	bool	cuboid_query		( math::cuboid const& cuboid, xray::collision::triangles_type& results ) const;
	virtual	bool	ray_query			(
											math::float3 const& origin, 
											math::float3 const& direction, 
											float max_distance, 
											float& distance, 
											xray::collision::ray_triangles_type& results, 
											xray::collision::triangles_predicate_type const& predicate
										) const;

	virtual	bool	aabb_test			( math::aabb const& aabb ) const;	
	virtual	bool	cuboid_test			( math::cuboid const& cuboid ) const;	
	virtual	bool	ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const;

	virtual	void	add_triangles		( collision::triangles_type& triangles ) const;

	virtual	xray::collision::geometry const* get_object_geometry	( ) const;


	virtual	non_null<xray::collision::geometry const>::ptr	get_geometry	( ) const;

	virtual	void							set_matrix		( math::float4x4 const& matrix );
	virtual	math::float4x4 const&			get_matrix		( ) const;

public:
			math::aabb const&				get_aabb			( ) const;
			math::float3					get_center			( ) const;
			math::float3					get_extents			( ) const;

			void							set_priority		( u32 priority );
			u32								get_priority		( ) const;

private:
	math::float4x4			m_transform;
	xray::collision::geometry_instance* const	m_geometry;
	u32						m_priority;

}; // class editor_collision_object



}// namespace editor
}// namespace xray

#endif // #ifndef EDITOR_COLLISION_OBJECT_H_INCLUDED
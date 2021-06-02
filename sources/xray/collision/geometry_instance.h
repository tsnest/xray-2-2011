////////////////////////////////////////////////////////////////////////////
//	Created		: 17.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_GEOMETRY_INSTANCE_H_INCLUDED
#define XRAY_COLLISION_GEOMETRY_INSTANCE_H_INCLUDED

#include <xray/collision/common_types.h>
#include <xray/render/engine/base_classes.h>

namespace xray {

namespace math {
	class random32;
	class convex;
} // namespace math

namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace collision {

class object;
class geometry;
struct on_contact;
struct geometry_double_dispatcher;

class box_geometry_instance;
class capsule_geometry_instance;
class cylinder_geometry_instance;
class sphere_geometry_instance;
class truncated_sphere_geometry_instance;


class geometry_instance //: public resources::unmanaged_resource
{
public:
									geometry_instance	( bool delete_by_collision_object = true ) : m_delete_by_collision_object( delete_by_collision_object ) {}
	virtual void					destroy				( memory::base_allocator* allocator ) = 0;

	virtual	void					set_matrix			( math::float4x4 const& matrix ) = 0;
	virtual	math::float4x4 const&	get_matrix			( ) const = 0;

	virtual	math::float3 const*		vertices			( ) const = 0;
	virtual	u32						vertex_count		( ) const = 0;
	virtual	u32 const*				indices				( ) const = 0;
	virtual	u32 const*				indices				( u32 triangle_id ) const = 0;
	virtual	u32						index_count			( ) const = 0;

	virtual	float					get_surface_area	( ) const = 0;

	virtual	bool					aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const = 0;
	virtual	bool					cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const = 0;
	virtual	bool					ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const = 0;

	virtual	bool					aabb_test			( math::aabb const& aabb ) const = 0;
	virtual	bool					cuboid_test			( math::cuboid const& cuboid ) const = 0;
	virtual	bool					ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const = 0;

	virtual	math::aabb				get_aabb			( ) const = 0;
	virtual	math::aabb				get_geometry_aabb	( ) const = 0;
	virtual	bool					is_valid			( ) const = 0;
	virtual	void					render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const = 0;
	virtual	void					render				( render::scene_ptr const& scene, render::debug::renderer& renderer, float4x4 const& transform ) const = 0;
	virtual	void					enumerate_primitives( enumerate_primitives_callback& cb ) const = 0;
	virtual	void					enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const = 0;

	virtual	void					add_triangles		( triangles_type& triangles ) const = 0;

	virtual void					accept				( geometry_double_dispatcher& dispatcher, geometry_instance const& node ) const					= 0;
	virtual void					visit				( geometry_double_dispatcher& dispatcher, box_geometry_instance const& node ) const				= 0;
	virtual void					visit				( geometry_double_dispatcher& dispatcher, capsule_geometry_instance const& node ) const			= 0;
	virtual void					visit				( geometry_double_dispatcher& dispatcher, cylinder_geometry_instance const& node ) const		= 0;
	virtual void					visit				( geometry_double_dispatcher& dispatcher, sphere_geometry_instance const& node ) const			= 0;
	virtual void					visit				( geometry_double_dispatcher& dispatcher, truncated_sphere_geometry_instance const& node ) const= 0;

	virtual	float3					get_random_surface_point( math::random32& randomizer ) const = 0;
	virtual float3					get_closest_point_to( float3 const& point, float4x4 const& origin = float4x4().identity() ) const = 0;

	inline	bool					delete_by_collision_object	( ) const { return m_delete_by_collision_object; }
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( geometry_instance )
private:
	bool							m_delete_by_collision_object;
}; // struct geometry_instance

#define XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS	\
	virtual void accept	( geometry_double_dispatcher& dispatcher, geometry_instance const& instance ) const				{ instance.visit( dispatcher, *this );	} \
	virtual void visit	( geometry_double_dispatcher& dispatcher, box_geometry_instance const& node ) const				{ dispatcher.dispatch( node, *this );	} \
	virtual void visit	( geometry_double_dispatcher& dispatcher, sphere_geometry_instance const& node ) const			{ dispatcher.dispatch( node, *this );	} \
	virtual void visit	( geometry_double_dispatcher& dispatcher, cylinder_geometry_instance const& node ) const		{ dispatcher.dispatch( node, *this );	} \
	virtual void visit	( geometry_double_dispatcher& dispatcher, capsule_geometry_instance const& node ) const			{ dispatcher.dispatch( node, *this );	} \
	virtual void visit	( geometry_double_dispatcher& dispatcher, truncated_sphere_geometry_instance const& node ) const{ dispatcher.dispatch( node, *this );	}

#define XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS_NOT_IMPLEMENTED	\
	virtual void accept	( geometry_double_dispatcher& , geometry_instance const& ) const								{ NOT_IMPLEMENTED( ); } \
	virtual void visit	( geometry_double_dispatcher& , box_geometry_instance const& ) const							{ NOT_IMPLEMENTED( ); } \
	virtual void visit	( geometry_double_dispatcher& , sphere_geometry_instance const& ) const							{ NOT_IMPLEMENTED( ); } \
	virtual void visit	( geometry_double_dispatcher& , cylinder_geometry_instance const& ) const						{ NOT_IMPLEMENTED( ); } \
	virtual void visit	( geometry_double_dispatcher& , capsule_geometry_instance const& ) const						{ NOT_IMPLEMENTED( ); } \
	virtual void visit	( geometry_double_dispatcher& , truncated_sphere_geometry_instance const& ) const				{ NOT_IMPLEMENTED( ); }

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_GEOMETRY_INSTANCE_H_INCLUDED
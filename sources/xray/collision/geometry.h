////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.12.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_GEOMETRY_H_INCLUDED
#define XRAY_COLLISION_GEOMETRY_H_INCLUDED

#include <xray/collision/common_types.h>
#include <xray/collision/primitives.h>
#include <xray/render/engine/base_classes.h>
#include <xray/collision/geometry_double_dispatcher.h>

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

struct geometry_double_dispatcher;
struct contact_info;
struct on_contact;
class sphere_geometry;
class box_geometry;
class capsule_geometry;
class cylinder_geometry;
class truncated_sphere_geometry;
class triangle_mesh_base;
class composite_geometry;

class /*XRAY_NOVTABLE*/ geometry : public resources::unmanaged_resource
{
public:
//								geometry			( ) : m_matrix ( float4x4().identity() ){}
	virtual	void				render				( render::scene_ptr const& scene, render::debug::renderer& renderer, math::float4x4 const& matrix ) const = 0;
	virtual	void				destroy				( memory::base_allocator* allocator ) = 0;

	//virtual	void					set_matrix			( math::float4x4 const& matrix ) { m_matrix = matrix; }
	//virtual	math::float4x4 const&	get_matrix			( ) const { return m_matrix; }

	virtual	math::float3 const* vertices			( ) const = 0;
	virtual	u32					vertex_count		( ) const = 0;
	virtual	u32 const*			indices				( ) const = 0;
	virtual	u32 const*			indices				( u32 triangle_id ) const = 0;
	virtual	u32					index_count			( ) const = 0;
	virtual u32					get_custom_data		( u32 triangle_id ) const { XRAY_UNREFERENCED_PARAMETER(triangle_id); return 0; };

	virtual	bool				aabb_query			( object const* object, math::aabb const& aabb, triangles_type& triangles ) const = 0;
	virtual	bool				cuboid_query		( object const* object, math::cuboid const& cuboid, triangles_type& triangles ) const = 0;
	virtual	bool				ray_query			( object const* object, math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const = 0;

	virtual	bool				aabb_test			( math::aabb const& aabb ) const = 0;
	virtual	bool				cuboid_test			( math::cuboid const& cuboid ) const = 0;
	virtual	bool				ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const = 0;

	virtual	u32					get_triangles_count	( math::convex const& bounding_convex ) const = 0;

	virtual	void				add_triangles		( triangles_type& triangles ) const = 0;

	virtual	math::aabb			get_aabb			( ) const = 0;
	virtual	float3			get_random_surface_point( math::random32& randomizer ) const = 0;
	//virtual	bool			is_inside			( float3 const& position ) const = 0;
	virtual	float				get_surface_area	( ) const = 0;

	//virtual	void				enumerate_primitives( enumerate_primitives_callback& cb ) const = 0;
	//virtual	void				enumerate_primitives( float4x4 const& transform, enumerate_primitives_callback& cb ) const = 0;

	//virtual void				accept				( geometry_double_dispatcher& dispatcher, geometry const& node ) const = 0;
	//virtual void				visit				( geometry_double_dispatcher& dispatcher, box_geometry const& node ) const = 0;
	//virtual void				visit				( geometry_double_dispatcher& dispatcher, sphere_geometry const& node ) const = 0;
	//virtual void				visit				( geometry_double_dispatcher& dispatcher, cylinder_geometry const& node ) const = 0;
	//virtual void				visit				( geometry_double_dispatcher& dispatcher, capsule_geometry const& node ) const = 0;
	//virtual void				visit				( geometry_double_dispatcher& dispatcher, truncated_sphere_geometry const& node ) const = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR			( geometry )
private:
//	float4x4				m_matrix;
}; // class geometry

//#define XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS	
//	virtual void accept	( geometry_double_dispatcher& dispatcher, geometry_instance const& instance ) const				{ geometry.visit( dispatcher, *this );	}

//	virtual void visit	( geometry_double_dispatcher& dispatcher, box_geometry_instance const& node ) const				{ dispatcher.dispatch( node, *this );	}	
//	virtual void visit	( geometry_double_dispatcher& dispatcher, sphere_geometry_instance const& node ) const			{ dispatcher.dispatch( node, *this );	}	
//	virtual void visit	( geometry_double_dispatcher& dispatcher, cylinder_geometry_instance const& node ) const		{ dispatcher.dispatch( node, *this );	}	
//	virtual void visit	( geometry_double_dispatcher& dispatcher, capsule_geometry_instance const& node ) const			{ dispatcher.dispatch( node, *this );	}	
//	virtual void visit	( geometry_double_dispatcher& dispatcher, truncated_sphere_geometry_instance const& node ) const{ dispatcher.dispatch( node, *this );	}

//#define XRAY_IMPLEMENT_COLLISION_GEOMETRY_VISIT_FUNCTIONS_NOT_IMPLEMENTED	
//	virtual void accept	( geometry_double_dispatcher& , geometry_instance const& ) const								{ NOT_IMPLEMENTED( ); }	

//	virtual void visit	( geometry_double_dispatcher& , box_geometry_instance const& ) const							{ NOT_IMPLEMENTED( ); }	
//	virtual void visit	( geometry_double_dispatcher& , sphere_geometry_instance const& ) const							{ NOT_IMPLEMENTED( ); }	
//	virtual void visit	( geometry_double_dispatcher& , cylinder_geometry_instance const& ) const						{ NOT_IMPLEMENTED( ); }	
//	virtual void visit	( geometry_double_dispatcher& , capsule_geometry_instance const& ) const						{ NOT_IMPLEMENTED( ); }	
//	virtual void visit	( geometry_double_dispatcher& , truncated_sphere_geometry_instance const& ) const				{ NOT_IMPLEMENTED( ); }

typedef	resources::resource_ptr<
	geometry,
	resources::unmanaged_intrusive_base
> geometry_ptr;

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_GEOMETRY_H_INCLUDED

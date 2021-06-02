////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_COMMON_TYPES_H_INCLUDED
#define XRAY_COLLISION_COMMON_TYPES_H_INCLUDED

namespace xray {
namespace render {
namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace collision {

class object;

struct triangle_result {
	object const*	object;
	u32				triangle_id;

	inline triangle_result	( collision::object const* object, u32 const triangle_id ) :
		object		( object ),
		triangle_id	( triangle_id )
	{
	}
}; // struct triangle_result

typedef object const*								object_result;

struct ray_triangle_result {
	object const*	object;
	u32				triangle_id;
	float			distance;

	inline ray_triangle_result	( collision::object const* object, u32 const triangle_id, float const distance ) :
		object		( object ),
		triangle_id	( triangle_id ),
		distance	( distance )
	{
	}
}; // struct triangle_result

struct ray_object_result {
	object const*	object;
	float			distance;

	inline ray_object_result	( collision::object const* object, float const distance ) :
		object		( object ),
		distance	( distance )
	{
	}
}; // struct object_result

struct primitive;
struct XRAY_NOVTABLE enumerate_primitives_callback{
	virtual void enumerate( float4x4 const &transform, primitive const &p ) = 0;
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR			( enumerate_primitives_callback )
}; // struct enumerate_primitives_callback

typedef vectora< object const* >											objects_type;
typedef vectora< triangle_result >											triangles_type;
typedef vectora< ray_object_result >										ray_objects_type;
typedef vectora< ray_triangle_result >										ray_triangles_type;

typedef boost::function< void ( object const& ) >							results_callback_type;

typedef fastdelegate::FastDelegate< bool ( ray_object_result const& ) >		objects_predicate_type;
typedef fastdelegate::FastDelegate< bool ( ray_triangle_result const& ) >	triangles_predicate_type;

typedef u32																	object_type;
typedef u32																	query_type;

typedef memory::base_allocator*												allocator_type;

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_COMMON_TYPES_H_INCLUDED
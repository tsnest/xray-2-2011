////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_OBJECT_H_INCLUDED
#define XRAY_COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/api.h>
#include <xray/render/engine/base_classes.h>

namespace xray {
namespace render {

namespace debug {
	class renderer;
} // namespace debug
} // namespace render

namespace physics {
	class space_item;
} // namespace physics

namespace collision {

class loose_oct_tree;
struct oct_node;

class XRAY_COLLISION_API object {
public:
	virtual	void				render				( render::scene_ptr const& scene, render::debug::renderer& renderer ) const = 0;

	virtual	bool				aabb_query			( math::aabb const& aabb, triangles_type& triangles ) const = 0;
	virtual	bool				cuboid_query		( math::cuboid const& cuboid, triangles_type& triangles ) const = 0;
	virtual	bool				ray_query			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance, ray_triangles_type& triangles, triangles_predicate_type const& predicate ) const = 0;

	
	virtual	bool				aabb_test			( math::aabb const& aabb ) const = 0;
	virtual	bool				cuboid_test			( math::cuboid const& cuboid ) const = 0;
	virtual	bool				ray_test			( math::float3 const& origin, math::float3 const& direction, float max_distance, float& distance ) const = 0;

	virtual	void				add_triangles		( triangles_type& triangles ) const = 0;
	virtual bool				touch				( ) const {return false;} // editor support( tmp ?)

	virtual	math::aabb			update_aabb			( float4x4 const& local_to_world ) = 0;

	virtual	physics::space_item* cast_space_item	( ) { return 0; }

	inline	math::aabb const&	get_aabb			( ) const { return m_aabb; }

	inline	object*				get_next			( ) const { return m_next; }
	inline	oct_node const*		get_node			( ) const { ASSERT( m_node ); return m_node; }
	inline	u32					get_type			( ) const { ASSERT( m_type != memory::uninitialized_value<u32>() ); return m_type; }
	inline	bool				is_type_suitable	( u32 const mask ) const { ASSERT( m_type != memory::uninitialized_value<u32>() ); return ((m_type & mask) != 0); }
	inline	pvoid				user_data			( ) const { return m_user_data; }

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR			( object )
protected:

	inline						object				( ) :
		m_next	( 0 ),
		m_node	( 0 ),
		m_type	( memory::uninitialized_value<u32>() ),
		m_aabb	( math::create_zero_aabb() )
	{
	}
	//inline					~object					( ) { }
//	inline	void			set_type				( u8 const less_than_32_value ) { ASSERT( m_type == memory::uninitialized_value<u32>() ); ASSERT( less_than_32_value < 32, "each value is a bit, therefore, we doesn't support more than 32 different object types in the single tree"); m_type = u32(1) << less_than_32_value; }
	inline	void			set_type				( u32 const type ) { ASSERT( type ); ASSERT( m_type == memory::uninitialized_value<u32>() ); m_type = type; }

protected:
	math::aabb		m_aabb;

private:
	object*			m_next;
	oct_node*		m_node;

protected:
	pvoid			m_user_data;

private:
	u32				m_type;

	friend class loose_oct_tree;
}; // class object

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_OBJECT_H_INCLUDED

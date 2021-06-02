////////////////////////////////////////////////////////////////////////////
//	Created		: 25.06.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_OBJECT_H_INCLUDED
#define COLLISION_OBJECT_H_INCLUDED

#include <xray/collision/object.h>

namespace xray {
namespace render {

//struct editor_visual;

// simple aabb collision object
template<typename T>
class collision_object : public collision::object 
{
public:
	collision_object( T* owner) : m_owner (owner) {  ASSERT (owner); set_type( 1 ); }


	virtual	void			render					( render::scene_ptr const&, render::debug::renderer& ) const { NOT_IMPLEMENTED(); }

	virtual	bool			aabb_query				( math::aabb const& , collision::triangles_type& ) const { return true; }
	virtual	bool			cuboid_query			( math::cuboid const& , collision::triangles_type& ) const { return true; }
	virtual	bool			ray_query				( math::float3 const& , math::float3 const& , float , float& , collision::ray_triangles_type& , collision::triangles_predicate_type const& ) const { return true; }

	virtual	bool			aabb_test				( math::aabb const& ) const { return true; }
	virtual	bool			cuboid_test				( math::cuboid const& ) const { return true; }
	virtual	bool			ray_test				( math::float3 const& , math::float3 const& , float , float& ) const { return true; }

	virtual	void			add_triangles			( collision::triangles_type& ) const { }
	virtual	math::aabb		update_aabb				( float4x4 const& local_to_world ) { return m_owner->get_aabb().modify(local_to_world); }

	inline	T*				owner					( ) const { return m_owner; }

private:
	T* m_owner;
}; // class collision_object


} // namespace render
} // namespace xray

#endif // #ifndef COLLISION_OBJECT_H_INCLUDED
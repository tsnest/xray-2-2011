////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_OBJECT_COLLISION_H_INCLUDED
#define EDITOR_OBJECT_COLLISION_H_INCLUDED

#include <xray/collision/collision_object_geometry.h>

namespace xray {
namespace editor_base {

ref class editor_object_collision;

public ref class collision_owner
{
public:
	virtual bool			on_touch		( /*collision_object_dynamic* object*/ ){ return false; }
};

class collision_object_internal : public collision::collision_object_geometry
{
	typedef collision::collision_object_geometry	super;
public:
							collision_object_internal(	collision::geometry const* geometry,
														editor_object_collision^ receiver, 
														collision::object_type t );
//	object_base^			get_owner_object		( ) const;
	virtual bool			touch					( ) const;

	gcroot<editor_object_collision^>	m_receiver;
}; //editor_object_collision

public ref class editor_object_collision
{
private:
	collision::geometry*			m_user_geometry;
	collision_object_internal*		m_collision_obj;
	bool							m_b_active; // inserted into tree
public:
									editor_object_collision	( collision::space_partitioning_tree* tree,
																collision_owner^ owner );
	bool							on_touch_collision	( /*collision_object_dynamic* object*/ );

	void							create_from_geometry( bool user_geom, collision::geometry* geom, xray::collision::object_type t );
	aabb							get_aabb			( );

	void							destroy				( memory::base_allocator* allocator );
	void							insert				( );
	void							remove				( );
	void							set_matrix			( float4x4 const* m );
	bool							initialized			( );

	collision::space_partitioning_tree* m_collision_tree;
	collision::object*				get_collision_object( );
	collision_owner^				m_owner;
};

} // namespace editor_base
} // namespace xray

#endif // #ifndef EDITOR_OBJECT_COLLISION_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 29.09.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef OBJECT_COLLISION_H_INCLUDED
#define OBJECT_COLLISION_H_INCLUDED

#pragma managed( push, off )
#	include <xray/collision/common_types.h>
#pragma managed( pop )

namespace xray {

namespace collision { class geometry_instance; struct space_partitioning_tree;}

namespace editor {

class collision_object_dynamic;
ref class level_editor;
ref class object_base;
ref struct object_collision;

public ref struct object_collision
{
private:
	collision::geometry_instance*		m_user_geometry;
	editor::collision_object_dynamic*	m_collision_obj;
	bool								m_b_active; // inserted into tree
public:
									object_collision	( collision::space_partitioning_tree* tree );
	void							create_from_geometry( bool user_geom, object_base^ o, collision::geometry_instance* geom, xray::collision::object_type t );
	aabb							get_aabb			( );

	void							destroy				( memory::base_allocator* allocator );
	void							insert				( float4x4 const* m );
	void							remove				( );
	void							set_matrix			( float4x4 const* m );
	bool							initialized			( );
	collision::space_partitioning_tree* m_collision_tree;
	collision::object*					get_collision_object( );
};

} // namespace editor
} // namespace xray

#endif // #ifndef OBJECT_COLLISION_H_INCLUDED
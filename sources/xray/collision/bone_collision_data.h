////////////////////////////////////////////////////////////////////////////
//	Created		: 14.04.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_COLLISION_BONE_COLLISION_DATA_H_INCLUDED
#define XRAY_COLLISION_BONE_COLLISION_DATA_H_INCLUDED

#include <xray/animation/type_definitions.h>

namespace xray {
namespace collision {

class composite_geometry_instance;

class bone_collision_data
{
public:
	inline	bone_collision_data	( pcstr name, geometry_instance* instance ) :
		bone_name				( name ),
		bone_geometry_instance	( instance ),
		skeleton_bone_index		( u32( -1 ) ) 
	{
	}

private:
	friend struct memory::detail::call_constructor_helper< bone_collision_data, false >;

	inline	bone_collision_data	( )
	{
		UNREACHABLE_CODE( );
	}

public:
	fixed_string<64>				bone_name;
	animation::bone_index_type		skeleton_bone_index;
	geometry_instance*				bone_geometry_instance;
};

} // namespace collision
} // namespace xray

#endif // #ifndef XRAY_COLLISION_BONE_COLLISION_DATA_H_INCLUDED

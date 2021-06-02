////////////////////////////////////////////////////////////////////////////
//	Created		: 28.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COLLISION_OBJECT_TYPES_H_INCLUDED
#define COLLISION_OBJECT_TYPES_H_INCLUDED

namespace stalker2 {

enum collision_object_types
{
	// for consistency with corresponding enum in editor_base
	collision_object_type_terrain	= ( 1 << 0 ),
	collision_object_type_ai		= ( 1 << 1 ),
	collision_object_static_model	= ( 1 << 2 )
}; // enum collision_object_types

} // namespace stalker2

#endif // #ifndef COLLISION_OBJECT_TYPES_H_INCLUDED
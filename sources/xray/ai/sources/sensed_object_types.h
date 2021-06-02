////////////////////////////////////////////////////////////////////////////
//	Created		: 06.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SENSED_OBJECT_TYPES_H_INCLUDED
#define SENSED_OBJECT_TYPES_H_INCLUDED

namespace xray {
namespace ai {
namespace sensors {

enum sensed_object_types
{
	sensed_object_type_visual,	
	sensed_object_type_sound,
	sensed_object_type_smell,
	sensed_object_type_hit,
	sensed_object_type_interaction
}; // enum sensed_object_types

} // namespace sensors
} // namespace ai
} // namespace xray

#endif // #ifndef SENSED_OBJECT_TYPES_H_INCLUDED
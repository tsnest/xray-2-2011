////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PERCEPT_MEMORY_OBJECT_TYPES_H_INCLUDED
#define PERCEPT_MEMORY_OBJECT_TYPES_H_INCLUDED

namespace xray {
namespace ai {

enum percept_memory_object_types
{	
	percept_memory_object_type_threat		= 0,
	percept_memory_object_type_enemy,
	percept_memory_object_type_order,
	percept_memory_object_type_disturbance,
	percept_memory_object_type_path_info,
	percept_memory_object_type_pickup_item,

	// all the new types must be inserted BEFORE this one
	percept_memory_object_types_count
}; // enum percept_memory_object_types

typedef std::pair< percept_memory_object_types, pcstr > percept_memory_object_type_caption;

static percept_memory_object_type_caption memory_object_types_captions[] =
{
	std::make_pair( percept_memory_object_type_threat,		"threat: "		),
	std::make_pair( percept_memory_object_type_enemy,		"enemy: "		),
	std::make_pair( percept_memory_object_type_order,		"order: "		),
	std::make_pair( percept_memory_object_type_disturbance,	"disturbance: "	),
	std::make_pair( percept_memory_object_type_path_info,	"path: "		),
	std::make_pair( percept_memory_object_type_pickup_item,	"pick-up item: ")
};

} // namespace ai
} // namespace xray

#endif // #ifndef PERCEPT_MEMORY_OBJECT_TYPES_H_INCLUDED
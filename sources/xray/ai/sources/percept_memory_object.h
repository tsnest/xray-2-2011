////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PERCEPT_MEMORY_OBJECT_H_INCLUDED
#define PERCEPT_MEMORY_OBJECT_H_INCLUDED

#include "percept_memory_object_types.h"
#include <xray/ai/game_object.h>

namespace xray {
namespace ai {

/*
template < typename FactType >
struct fact_attribute
{
	fact_attribute	( FactType const& fact_value, float fact_confidence ) :
		value		( fact_value ),
		confidence	( fact_confidence )
	{
	}
	
	FactType		value;
	float			confidence;
};
*/
	
struct percept_memory_object : private boost::noncopyable
{
	percept_memory_object		( percept_memory_object_types knowledge_type ) :
		object					( 0 ),
		owner_position			( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		target_position			( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		update_time				( memory::uninitialized_value< u32 >() ),
		type					( knowledge_type ),
		confidence				( 0.f ),
		next					( 0 )
	{
	}

	game_object_ptr				object;

	float3						owner_position;
	float3						target_position;

	u32							update_time;
	
	percept_memory_object_types	type;
	float						confidence;

	percept_memory_object*		next;
}; // struct percept_memory_object

} // namespace ai
} // namespace xray

#endif // #ifndef PERCEPT_MEMORY_OBJECT_H_INCLUDED
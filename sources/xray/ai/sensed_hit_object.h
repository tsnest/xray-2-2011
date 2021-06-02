////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SENSED_HIT_OBJECT_H_INCLUDED
#define XRAY_AI_SENSED_HIT_OBJECT_H_INCLUDED

#include <xray/ai/game_object.h>

namespace xray {
namespace ai {

struct sensed_hit_object : private boost::noncopyable
{
	inline sensed_hit_object	( ) :
		direction				( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		own_position			( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		object					( 0 ),
		bone_index				( memory::uninitialized_value< u16 >() ),
		extent_of_damage		( memory::uninitialized_value< float >() )
	{
	}

	float3						direction;
	float3						own_position;
	game_object_ptr				object;
	u16							bone_index;
	float						extent_of_damage;
}; // struct sensed_hit_object

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SENSED_HIT_OBJECT_H_INCLUDED
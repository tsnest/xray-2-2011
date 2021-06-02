////////////////////////////////////////////////////////////////////////////
//	Created		: 26.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SENSED_SOUND_OBJECT_H_INCLUDED
#define XRAY_AI_SENSED_SOUND_OBJECT_H_INCLUDED

#include <xray/ai/game_object.h>
#include <xray/ai/sound_collection_types.h>

namespace xray {
namespace ai {

struct sensed_sound_object : private boost::noncopyable
{
	inline sensed_sound_object	( ) :
		position				( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		object					( 0 ),
		type					( memory::uninitialized_value< sound_collection_types >() ),
		power					( memory::uninitialized_value< u32 >() )
	{
	}

	float3						position;
	game_object_ptr				object;
	sound_collection_types		type;
	u32							power;
}; // struct sensed_sound_object

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SENSED_SOUND_OBJECT_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SENSED_VISUAL_OBJECT_H_INCLUDED
#define XRAY_AI_SENSED_VISUAL_OBJECT_H_INCLUDED

#include <xray/ai/game_object.h>

namespace xray {
namespace ai {

struct sensed_visual_object : private boost::noncopyable
{
	inline sensed_visual_object		( ) :
		own_position				( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		local_point					( memory::uninitialized_value< float >(), memory::uninitialized_value< float >(), memory::uninitialized_value< float >() ),
		object						( 0 ),
		next						( 0 ),
		distance					( memory::uninitialized_value< float >() ),
		visibility_value			( 0 ),
		was_visible_last_time		( false ),
		is_in_frustum				( memory::uninitialized_value< bool >() ),
		newly_added_to_frustum		( memory::uninitialized_value< bool >() ),
		was_updated					( memory::uninitialized_value< bool >() )
	{
	}
	
	float3							own_position;
	float3							local_point;
	game_object_ptr					object;
	sensed_visual_object*			next;
	float							distance;
	float							visibility_value;
	bool							was_visible_last_time;
	bool							is_in_frustum;
	bool							newly_added_to_frustum;
	bool							was_updated;
}; // struct sensed_visual_object

typedef boost::function< void ( game_object const& ) > update_frustum_callback_type;

} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SENSED_VISUAL_OBJECT_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_ANIMATION_ANIMATION_CALLBACK_H_INCLUDED
#define XRAY_ANIMATION_ANIMATION_CALLBACK_H_INCLUDED

#include <xray/animation/skeleton_animation.h>

namespace xray {
namespace animation {

enum callback_return_type_enum {
	callback_return_type_call_me_again,
	callback_return_type_dont_call_me_anymore,
	callback_return_type_hasnt_called,
}; // enum callback_return_type_enum

typedef boost::function< callback_return_type_enum ( skeleton_animation_ptr const& animation, pcstr channel_id, u32 callback_time_in_ms, u8 domain_data ) >	callback_type;

struct animation_callback : private boost::noncopyable {
	inline	explicit animation_callback	( callback_type const& callback, u8 const callback_id ) :
		callback				( callback ),
		next					( 0 ),
		callback_id				( callback_id ),
		enabled					( true )
	{
		R_ASSERT				( callback );
	}

	callback_type				callback;
	animation_callback*			next;
	u8							callback_id;
	bool						enabled;
}; // struct animation_callback

struct subscribed_channel {
	pcstr						channel_id;
	subscribed_channel*			next;
	animation_callback*			first_callback;
}; // struct subscribed_channel

} // namespace animation
} // namespace xray

#endif // #ifndef XRAY_ANIMATION_ANIMATION_CALLBACK_H_INCLUDED
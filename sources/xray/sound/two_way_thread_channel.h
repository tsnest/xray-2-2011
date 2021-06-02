////////////////////////////////////////////////////////////////////////////
//	Created		: 05.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_TWO_WAY_THREAD_CHANNEL_H_INCLUDED
#define XRAY_SOUND_TWO_WAY_THREAD_CHANNEL_H_INCLUDED

#include <xray/one_way_threads_channel.h>
#include <xray/sound/sound_object_commands.h>
#include <xray/sound/functor_command.h>

namespace xray {
namespace sound {

typedef one_way_threads_channel<
	sound_responses_queue_type,
	sound_responses_queue_type
>	responses_channel_type;

typedef one_way_threads_channel<
	sound_orders_queue_type,
	sound_orders_queue_type
>	orders_channel_type;

struct XRAY_SOUND_API two_way_threads_channel : private sound::noncopyable {
	inline two_way_threads_channel	(
			memory::base_allocator& responses_allocator,
			memory::base_allocator& orders_allocator
		) :
		responses	( responses_allocator ),
		orders		( orders_allocator )
	{
	}

	responses_channel_type	responses;
	orders_channel_type		orders;
}; // struct two_way_threads_channel

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_TWO_WAY_THREAD_CHANNEL_H_INCLUDED
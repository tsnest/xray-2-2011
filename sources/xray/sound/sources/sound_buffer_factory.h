////////////////////////////////////////////////////////////////////////////
//	Created		: 10.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_BUFFER_FACTORY_H_INCLUDED
#define SOUND_BUFFER_FACTORY_H_INCLUDED

#include <xray/memory_single_size_buffer_allocator.h>
#include <xray/unmanaged_allocation_resource.h>
#include "sound_buffer.h"

namespace xray {
namespace sound {

template < int initial_value, int align_value >
struct align_helper {
	enum {
		result	= ((initial_value - 1)/align_value + 1)*align_value,
	};
};

enum {
	real_sound_buffer_size = sizeof(sound_buffer) + sound_buffer_size,
	real_sound_buffer_size_aligned = align_helper< real_sound_buffer_size, XRAY_DEFAULT_ALIGN_SIZE >::result,
};

class sound_buffer_factory : private boost::noncopyable
{
public:
						sound_buffer_factory		( u8* buffer, u32 size, u32 max_buffers );
						~sound_buffer_factory		( );

			sound_buffer*	new_sound_buffer		(	encoded_sound_ptr const& encoded_sound,
														u32 pcm_offset,
														u32& next_pcm_offset
													);
			void			delete_sound_buffer		( sound_buffer* buffer );

			sound_buffer*	get_mute_sound_buffer	( encoded_sound_ptr const& encoded_sound );

			sound_buffer*	new_no_cahceable_sound_buffer(	encoded_sound_ptr const& encoded_sound,
															u32 pcm_offset,
															u32& next_pcm_offset
														);

			void			clear_cache				( );
private:
	typedef memory::single_size_buffer_allocator< real_sound_buffer_size_aligned,
												  threading::single_threading_policy > 
												  sound_buffers_allocator;

		sound_buffer*	find_least_recently_used( );

private:
	sound_buffer_list_type		m_free_sound_buffers;
	sound_buffer_list_type		m_lru_sound_buffers;
	sound_buffers_set_type		m_cached_sound_buffers;

	sound_buffers_allocator 	m_sound_buffers_allocator;
	sound_buffer*				m_mute_buffer;

}; // class sound_buffer_factory

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_BUFFER_FACTORY_H_INCLUDED

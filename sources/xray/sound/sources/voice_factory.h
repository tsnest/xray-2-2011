////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VOICE_FACTORY_H_INCLUDED
#define VOICE_FACTORY_H_INCLUDED

#include <boost/array.hpp>
#include <xray/memory_single_size_buffer_allocator.h>
#include <xray/unmanaged_allocation_resource.h>
#include "voice_format.h"
#include "voice_bridge.h"


namespace xray {
namespace sound {

class sound_world;

class voice_factory : private boost::noncopyable
{
public:
					voice_factory			( u8* buffer, u32 buffer_size, sound_world const& world, pool_parametrs const& params );
					~voice_factory			( );

	voice_bridge*	new_voice				( voice_callback_handler* callback_handler, channels_type type, u32 sample_rate );
			void	delete_voice			( voice_bridge* voice_to_be_deleted );

			void	set_frequency_ratio		( float ratio );

	inline	float	get_min_frequency_ratio	( ) const { return m_min_frequency_ratio; }
	inline	float	get_max_frequency_ratio ( ) const { return m_max_frequency_ratio; }
private:
	//typedefs
	typedef boost::array<voice_list_type, channels_type_count>		voice_container;

	typedef memory::single_size_buffer_allocator< sizeof( voice_bridge ),
												  threading::single_threading_policy > 
												  voices_allocator;

	
private:
	pool_parametrs			m_pool_params;
	voice_container			m_voices;
	voices_allocator		m_voices_allocator;
	float const				m_min_frequency_ratio;
	float const				m_max_frequency_ratio;
}; // voice_factory

} // namespace sound
} // namespace xray

#endif // #ifndef VOICE_FACTORY_H_INCLUDED

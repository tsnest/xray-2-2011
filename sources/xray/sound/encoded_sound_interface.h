////////////////////////////////////////////////////////////////////////////
//	Created		: 28.03.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_SOUND_ENCODED_SOUND_INTERFACE_H_INCLUDED
#define XRAY_SOUND_ENCODED_SOUND_INTERFACE_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/sound/channels_type.h>

namespace xray {
namespace sound {

class XRAY_SOUND_API encoded_sound_interface :
	public resources::unmanaged_resource,
	private sound::noncopyable
{
public:
							encoded_sound_interface		( );

	inline	u64				get_length_in_pcm			( ) const { return m_length_in_pcm;		}
	inline	u64				get_length_in_msec			( ) const { return m_length_in_msec;	}
	inline	u32				get_samples_per_sec			( ) const { return m_samples_per_sec;	}
	inline	channels_type	get_channels_type			( ) const { return m_channels_type;		}
	inline	u32				get_bytes_per_sample		( ) const { return m_bytes_per_sample;	}

	virtual	u32				decompress					(	pbyte dest,
															u32 pcm_pointer,
															u32& pcm_pointer_after_decompress,
															u32 bytes_needed ) = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR				( encoded_sound_interface );
protected:
	u64					m_length_in_pcm;
	u64					m_length_in_msec;
	u32					m_samples_per_sec;
	u32					m_bytes_per_sample;
	channels_type		m_channels_type;
}; // class encoded_sound_interface

typedef	resources::resource_ptr < encoded_sound_interface, resources::unmanaged_intrusive_base > encoded_sound_ptr;
typedef	resources::child_resource_ptr < encoded_sound_interface, resources::unmanaged_intrusive_base > encoded_sound_child_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef XRAY_SOUND_ENCODED_SOUND_INTERFACE_H_INCLUDED
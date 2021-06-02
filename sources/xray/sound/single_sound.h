////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SINGLE_SOUND_H_INCLUDED
#define SINGLE_SOUND_H_INCLUDED

#include <xray/sound/world.h>
#include <xray/sound/sound_emitter.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/encoded_sound_with_qualities.h>
#include <xray/sound/encoded_sound_interface.h>
#include <xray/sound/sound_rms.h>
#include <xray/sound/sound_spl.h>

namespace xray {
namespace sound {

class sound_instance;

class single_sound :
	public sound_emitter,
	public sound_propagator_emitter
{
public:
					single_sound			( encoded_sound_with_qualities_ptr const& encoded_sound, sound_rms_ptr const& rms, 	configs::binary_config_ptr const& spl, sound_spl_ptr const& spl_curve );
	virtual			~single_sound			( );

	virtual	void	emit_sound_propagators	(
						sound_instance_proxy_internal& proxy,
						playback_mode mode,
						u32 before_playing_offset = 0,
						u32 after_playing_offset = 0,
						sound_producer const* const producer = 0,
						sound_receiver const* const ignorable_receiver = 0
					) const;

	virtual encoded_sound_child_ptr const&		get_quality_for_resource	( ) const;
	virtual encoded_sound_child_ptr const&		get_quality_for_resource	( world& world, resources::positional_unmanaged_resource const* const resource ) const { return m_encoded_sound->get_encoded_sound( world, resource ); }
	virtual encoded_sound_child_ptr const&		dbg_get_encoded_sound		( u32 quality )  const { return m_encoded_sound->dbg_get_encoded_sound( quality );  }
	virtual	sound_spl_ptr const&				get_sound_spl				( ) const { return m_spl; }

	inline	sound_rms_ptr const&				get_sound_rms				( ) const { return m_rms; }
	inline	configs::binary_config_ptr const&	get_spl						( ) const { return m_spl_config; }

	virtual void								serialize					( memory::writer& w ) const;
	virtual void								deserialize					( memory::reader& r );

	virtual	emitter_type						get_type					( ) const { return single; }
protected:
	virtual	sound_propagator_emitter const*		get_sound_propagator_emitter( ) const;
	virtual sound_propagator_emitter const*		get_sound_propagator_emitter( u64 address ) const { return m_old_address == address ? this : 0;} //{ return (u64)get_sound_propagator_emitter( ) == address ? this : 0;}

private:
	encoded_sound_with_qualities_child_ptr	m_encoded_sound;
	sound_rms_ptr							m_rms;
	configs::binary_config_ptr				m_spl_config;
	sound_spl_ptr							m_spl;
}; // class single_sound

typedef	resources::resource_ptr < single_sound, resources::unmanaged_intrusive_base > single_sound_ptr;

} // namespace sound
} // namespace xray

#endif // #ifndef SINGLE_SOUND_H_INCLUDED
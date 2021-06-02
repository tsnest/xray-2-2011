////////////////////////////////////////////////////////////////////////////
//	Created		: 29.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_PROPAGATOR_EMITTER_H_INCLUDED
#define SOUND_PROPAGATOR_EMITTER_H_INCLUDED

#include <xray/sound/encoded_sound_interface.h>

namespace xray {
namespace sound {

class world_user;
class sound_instance;
class sound_instance_proxy_internal;
class sound_spl;
class sound_rms;

typedef	resources::resource_ptr < sound_spl, resources::unmanaged_intrusive_base > sound_spl_ptr;
typedef	resources::managed_resource_ptr sound_rms_ptr;

class XRAY_NOVTABLE sound_propagator_emitter {
public:
	virtual	void	emit_sound_propagators	(
						sound_instance_proxy_internal& proxy,
						playback_mode mode,
						u32 before_playing_offset = 0,
						u32 after_playing_offset = 0,
						sound_producer const* const producer = 0,
						sound_receiver const* const ignorable_receiver = 0
					) const = 0;

	virtual sound_propagator_emitter const* get_sound_propagator_emitter( u64 address ) const = 0;

	virtual encoded_sound_child_ptr const&		get_quality_for_resource	( ) const = 0;
	virtual encoded_sound_child_ptr const&	get_quality_for_resource( world& world, resources::positional_unmanaged_resource const* const resource ) const = 0;
	virtual encoded_sound_child_ptr const&		dbg_get_encoded_sound		( u32 quality )  const = 0;
	virtual sound_spl_ptr const&			get_sound_spl			( ) const = 0;	
	virtual sound_rms_ptr const&			get_sound_rms			( ) const = 0;
protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR	( sound_propagator_emitter );
}; // class sound_propagator_emitter



} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_PROPAGATOR_EMITTER_H_INCLUDED
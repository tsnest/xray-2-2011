////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef COMPOSITE_SOUND_H_INCLUDED
#define COMPOSITE_SOUND_H_INCLUDED

#include <xray/sound/sound_emitter.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/math_randoms_generator.h>

namespace xray {
namespace sound {

class composite_sound :	
	public sound_emitter,
	private sound_propagator_emitter
{
public:
	typedef std::pair< sound_emitter_ptr, std::pair< u32, u32 > > sounds_type;

public:
					composite_sound			( pvoid buffer, size_t max_count, u32 last_time );
	virtual			~composite_sound		( );
	
	virtual	void	emit_sound_propagators	(
						sound_instance_proxy_internal& proxy,
						playback_mode mode,
						u32 before_playing_offset = 0,
						u32 after_playing_offset = 0,
						sound_producer const* const producer = 0,
						sound_receiver const* const ignorable_receiver = 0
					) const;

	
			void	add_sound				( sound_emitter_ptr sound, std::pair< u32, u32 > const& offsets );

	virtual	sound_propagator_emitter const*	get_sound_propagator_emitter( ) const;
	virtual sound_propagator_emitter const* get_sound_propagator_emitter( u64 address ) const;

	virtual encoded_sound_child_ptr const&	get_quality_for_resource( world& world, resources::positional_unmanaged_resource const* const resource ) const { return m_collection[0].first->get_sound_propagator_emitter( )->get_quality_for_resource( world, resource ); }
	virtual encoded_sound_child_ptr const&	get_quality_for_resource( ) const { return m_collection[0].first->get_sound_propagator_emitter( )->get_quality_for_resource( ); }
	virtual encoded_sound_child_ptr const&	dbg_get_encoded_sound( u32 quality )  const { XRAY_UNREFERENCED_PARAMETER(quality); return m_collection[0].first->get_sound_propagator_emitter( )->get_quality_for_resource( ); }
	virtual sound_spl_ptr const&			get_sound_spl			( ) const { return m_collection[0].first->get_sound_propagator_emitter( )->get_sound_spl( ); }
	virtual sound_rms_ptr const&			get_sound_rms			( ) const { return m_collection[0].first->get_sound_propagator_emitter( )->get_sound_rms( ); }

	virtual void							serialize					( memory::writer& w ) const;
	virtual void							deserialize					( memory::reader& r );

	virtual	emitter_type					get_type					( ) const { return composite; }
private:
	buffer_vector< sounds_type >			m_collection;
	mutable math::random32					m_random_number;
}; // class composite_sound

} // namespace sound
} // namespace xray

#endif // #ifndef COMPOSITE_SOUND_H_INCLUDED
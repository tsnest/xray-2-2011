////////////////////////////////////////////////////////////////////////////
//	Created		: 02.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_COLLECTION_H_INCLUDED
#define SOUND_COLLECTION_H_INCLUDED

#include <xray/sound/sound_emitter.h>
#include "xray/math_randoms_generator.h"

namespace xray {
namespace sound {

enum collection_playing_types
{
	collection_playing_type_random,
	collection_playing_type_sequential
};

class sound_world;

class sound_collection : public sound_emitter
{
public:
					sound_collection		(
						collection_playing_types type,
						bool can_repeat_successively,
						u16 cyclic_repeating_index,
						pvoid buffer,
						size_t max_count,
						u32 last_time
					);
	virtual			~sound_collection		( );

			void	add_sound				( sound_emitter_ptr sound );
		
	virtual sound_propagator_emitter const* get_sound_propagator_emitter( ) const;
	virtual	sound_propagator_emitter const*	get_sound_propagator_emitter( u64 address ) const;

	virtual void							serialize					( memory::writer& w ) const;
	virtual void							deserialize					( memory::reader& r );

	virtual	emitter_type					get_type					( ) const { return collection; }
private:
			sound_emitter const*			get_sound					( ) const;

private:
#pragma message ( XRAY_TODO( "Rocksteady 2 Lain: figure out how to replace buffer_vector" ) )	
	buffer_vector< sound_emitter_ptr >		m_sounds;
	collection_playing_types				m_type;
	mutable math::random32					m_random_number;
	u32										m_cyclic_repeating_from_index;
	mutable u32								m_previously_played_sound_index;
	bool									m_can_repeat_successively;
}; // class sound_collection

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_COLLECTION_H_INCLUDED

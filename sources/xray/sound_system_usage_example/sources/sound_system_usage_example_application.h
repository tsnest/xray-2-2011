////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_SYSTEM_USAGE_EXAMPLE_APPLICATION_H_INCLUDED
#define SOUND_SYSTEM_USAGE_EXAMPLE_APPLICATION_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/sound/sound_emitter.h>
#include "npc.h"

namespace xray {
namespace sound_system_usage_example {

class application {
public:
			void	initialize		( );
			void	execute			( );
			void	finalize		( );
	inline	u32		get_exit_code	( ) const	{ return m_exit_code; }

private:
			void	on_sound_scene_created		( resources::queries_result& data );
			void	on_sound_collection_created	( resources::queries_result& data );
			void	on_sound_emitter_created	( resources::queries_result& data );
			void	initialize_sound_world		( );
private:
	sound::world*				m_sound_world;
	sound::sound_scene_ptr		m_sound_scene;
	sound::sound_emitter_ptr	m_sound_emitter;
	npc*						m_npc_marsh;
	npc*						m_npc_broflovski;
	u32		m_exit_code;
}; // class application

} // namespace sound_system_usage_example
} // namespace xray

#endif // #ifndef SOUND_SYSTEM_USAGE_EXAMPLE_APPLICATION_H_INCLUDED
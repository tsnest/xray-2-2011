////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef AI_SOUND_PLAYER_H_INCLUDED
#define AI_SOUND_PLAYER_H_INCLUDED

#include <xray/ai/sound_player.h>
#include <xray/sound/sound.h>
#include <xray/math_randoms_generator.h>

namespace xray {

namespace input{
struct world;
} //namespace input

namespace sound {

	class world_user;
	class sound_emitter;
	class sound_instance_proxy;
	class sound_producer;
	class sound_receiver;

	typedef	resources::child_resource_ptr < sound_emitter, resources::unmanaged_intrusive_base > sound_emitter_child_ptr;
	typedef resources::resource_ptr < sound_emitter, resources::unmanaged_intrusive_base > sound_emitter_ptr;
	typedef intrusive_ptr <	sound_instance_proxy,
							sound_instance_proxy,
							threading::single_threading_policy > sound_instance_proxy_ptr;
} // namespace sound
} // namespace xray

namespace stalker2 {

class ai_sound_player :
	public ai::sound_player,
	private boost::noncopyable
{
public:

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: input::world only needed to test, remove this constructor after test") )

			ai_sound_player	(
				sound::sound_scene_ptr& scene,
				u32 sounds_count,
				sound::world_user& user,
				input::world* input_world,
				sound::sound_producer const* const producer = 0,
				sound::sound_receiver const* const ignorable_receiver = 0
			);

			ai_sound_player	(
				sound::sound_scene_ptr& scene,
				u32 sounds_count,
				sound::world_user& user,
				sound::sound_producer const* const producer = 0,
				sound::sound_receiver const* const ignorable_receiver = 0
			);

	virtual	~ai_sound_player( );
	
	virtual	void	play	(
						ai::sound_collection_types sound_type,
						bool sound_is_positioned = false,
						float3 const& position = float3( 0.f, 0.f, 0.f )
					);
	virtual	void	play	(
						resources::unmanaged_resource_ptr sound_to_be_played,
						ai::finish_playing_callback const& finish_callback,
						float3 const& position
					);
	virtual	void	play_once	(
						ai::sound_collection_types sound_type,
						bool sound_is_positioned = false,
						float3 const& position = float3( 0.f, 0.f, 0.f )
					);
	virtual	void	tick			( );
	virtual void	clear_resources	( );

			void	on_active_sound_serialized	( memory::writer* sound_thread_writer, memory::writer* current_thread_writer );
			void	on_active_sound_deserialized( memory::reader* reader, pvoid buf );
public:

	struct sounds_collection_type
	{
		sounds_collection_type	(	ai_sound_player* parent,
									ai::sound_collection_types collection_type,
									sound::sound_emitter_ptr emitter_ptr,
									u32 collection_priority );
		
		ai::sound_collection_types		type;
		sound::sound_emitter_child_ptr	emitter;
		u32								priority;
	};

private:
	sounds_collection_type const*	find				( ai::sound_collection_types sound_type ) const;
	sound::command_result_enum		on_finish_playing	( );
	inline sounds_collection_type const* sounds			( ) const
	{
		return pointer_cast<sounds_collection_type const*>( pointer_cast<pcbyte>( this ) + sizeof( *this ) );
	}

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: process_input only needed to test, remove this functio after test") )
#ifndef MASTER_GOLD
	void							process_input		( );
#endif // #ifndef MASTER_GOLD
	void					on_dbg_sound_scene_created	( resources::queries_result& data );

	void							serialize			( );
	void							deserialize			( );

private:
	sound::sound_instance_proxy_ptr		m_active_sound;
	sound::sound_scene_ptr&				m_scene;
	sound::world_user&					m_user;
	sound::sound_producer const*		m_sound_producer;
	sound::sound_receiver const* const	m_ignorable_receiver;
	u32	const							m_sounds_count;

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: input::world only needed to test, remove this pointer after test") )
	input::world*						m_dbg_input_world;
	math::random32						m_dbg_random;
	timing::timer						m_dbg_timer;
	u32									m_dbg_last_current_time_in_ms;
	u32									m_dbg_time_from_last_playing;
	u32									m_dbg_time_to_play_new_sound;
	sound::sound_scene_ptr				m_dbg_scene;
	sound::sound_instance_proxy_ptr		m_dbg_active_sound;

}; // class ai_sound_player

} // namespace stalker2

#endif // #ifndef AI_SOUND_PLAYER_H_INCLUDED

////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_PROPAGATOR_H_INCLUDED
#define SOUND_PROPAGATOR_H_INCLUDED

#include <xray/sound/sound.h>
#include <xray/sound/sound_producer.h>
#include <xray/sound/sound_instance_proxy.h>
#include <xray/memory_single_size_buffer_allocator.h>

namespace xray {
namespace sound {

struct play_params;
struct propagator_statistic;
class encoded_sound_interface;
class world_user;
class sound_instance_proxy_internal;
class sound_instance_emitter;
class sound_propagator_emitter;
class sound_voice;
class sound_spl;
class sound_rms;

typedef	resources::resource_ptr < encoded_sound_interface, resources::unmanaged_intrusive_base > encoded_sound_ptr;
typedef	resources::resource_ptr < sound_spl, resources::unmanaged_intrusive_base > sound_spl_ptr;
typedef	resources::managed_resource_ptr sound_rms_ptr;

struct create_sound_propagator_params
{	
	create_sound_propagator_params (	playback_mode							mode,
										sound_instance_proxy_internal&			proxy,
										sound_producer const* const				producer = 0,
										sound_receiver const* const				ignorable_receiver = 0
										) :
		m_mode				( mode ),
		m_proxy				( proxy ),
		m_producer			( producer ),
		m_ignorable_receiver( ignorable_receiver ),
		m_type				( point )
		{}

	create_sound_propagator_params (	source_params const&					params,
										playback_mode							mode,
										sound_instance_proxy_internal&			proxy,
										sound_producer const* const				producer = 0,
										sound_receiver const* const				ignorable_receiver = 0
										) :
		m_direction			( params.m_direction ),
		m_cone_inner_angle	( params.m_cone_inner_angle ),
		m_cone_outer_angle	( params.m_cone_outer_angle ),
		m_outer_gain		( params.m_outer_gain ),
		m_mode				( mode ),
		m_proxy				( proxy ),
		m_producer			( producer ),
		m_ignorable_receiver( ignorable_receiver ),
		m_type				( cone )
		{}

private:
	const create_sound_propagator_params& operator= ( const create_sound_propagator_params& );
public:
	float3									m_direction;
	float									m_cone_inner_angle;
	float									m_cone_outer_angle;
	float									m_outer_gain;
	sound_instance_proxy_internal&			m_proxy;
	sound_producer const* const				m_producer;
	sound_receiver const*					m_ignorable_receiver;
	playback_mode							m_mode;
	enum type
	{
		point,
		cone
	}										m_type;
};

class sound_propagator : private boost::noncopyable
{
public:
								sound_propagator		(	playback_mode mode,
															u32	playing_offset,
															u32 before_playing_offset,
															u32 after_playing_offset,
															sound_instance_proxy_internal& proxy,
															sound_propagator_emitter const& emitter,
															sound_producer const* const producer,
															sound_receiver const* const	ignorable_receiver = 0);

								~sound_propagator		( );
			void				tick					( u32 time_delta_in_msec );

			void	pause_produce_and_preserve_state_for_resume	( );
			void	clear_preservation_state_for_resume			( );
	inline	bool	is_state_preserved_for_resume				( ) const { return m_is_state_preserved_for_resume; }

			void				pause_propagation		( );
			void				resume_propagation		( );
	inline	bool				is_propagation_paused	( ) const { return m_is_pause_propagating_requested; }

			void				stop_produce_sound		( );
			void				stop_propagate_sound	( );

	inline	bool				is_sound_producing		( ) const { return m_is_sound_producing; }
	inline	bool				is_propagation_finished	( ) const { return m_is_propagation_finished; }
	inline	bool				can_be_deleted			( ) const { return	m_is_propagation_finished &&
																			!m_is_state_preserved_for_resume; }
	inline	u32					get_sound_length		( ) const { return m_sound_length; }
	inline	u32					get_playing_offset		( ) const { return m_playing_offset; }
	inline	u32				get_sound_length_original	( ) const { return m_sound_length_original;}
	inline	u32				get_before_playing_offset	( ) const { return m_before_playing_offset; }
			float				get_unheard_distance	( ) const;
	inline	playback_mode		get_playback_mode		( ) const { return m_mode; }

	inline	u32					get_after_palying_offset	( ) const { return m_after_playing_offset; }
	inline	u32					get_before_palying_offset	( ) const { return m_before_playing_offset; }

	inline	bool				is_callback_executer	( ) const { return m_is_callbak_executer; }
	inline	void				set_as_callback_executer( bool val ) { m_is_callbak_executer = val; }			

	inline sound_propagator_emitter const&	get_propagator_emitter	( ) const { return m_emitter; }

	inline	bool				has_sound_voice			( ) const { return m_sound_voice != 0; }
			void				attach_sound_voice		( u32 delta_time_from_last_tick );
			void				detach_sound_voice		( );

			float			get_propagation_outer_radius( ) const;
			float			get_propagation_inner_radius( ) const;

	inline	float3 const&	get_listener_position			( ) const { return m_listener_position; }

			float	get_propagation_outer_radius_for_listener	( ) const;
			float	get_propagation_inner_radius_for_listener	( ) const;


	inline	sound_producer const*			get_producer				( ) const { return m_producer; }
	inline	void							clear_producer				( ) { m_producer = 0; }
	inline	sound_receiver const* const		get_ignorable_receiver		( ) const { return m_ignorable_receiver; }
	inline	sound_instance_proxy_internal&	get_proxy					( ) const { return m_proxy; }
//	inline	sound_propagator_emitter const&	get_emitter					( ) const { return m_emitter; }

	// hdr audio functions
	inline	float					get_percived_loudness	( ) const		{ return m_perceived_loudness; }
	inline	void					set_percived_loudness	( float pl )	{ m_perceived_loudness = pl; }

	inline	float					get_attenuated_loudness	( ) const		{ return m_attenuated_loudness; }
	inline	void					set_attenuated_loudness	( float al )	{ m_attenuated_loudness = al; }

			float					get_sound_spl			( float distance ) const;
			float					get_sound_rms_value		( ) const;
			sound_rms_ptr const&	get_sound_rms			( ) const;

			void					serialize				( memory::writer& w ) const;
			void					deserialize				( memory::reader& r );

			void					set_quality				( u32 quality );

#ifndef MASTER_GOLD
			void					dump_debug_snapshot		( configs::lua_config_value& val ) const;
			void					fill_statistic			( propagator_statistic& statistic ) const;
#endif // #ifndef MASTER_GOLD
private:			
			void					on_finish_playing_sound	( );
			void					on_finish_propagating	( );
			void					execute_user_callback	( ) const;
			void					stop_playing_sound		( );

			void					tick_for_once_mode		( u32 time_delta_in_msec );
			void					tick_for_looped_mode	( u32 time_delta_in_msec );

			float				get_outer_radius_internal	( ) const;

public:
	sound_propagator*		m_next_for_proxies;
private:
	// we can't update propagator position, but we update
	// m_current_proxy_position, so that we could put sound_voice in the right place
	// It's a fake, maybe we will save path. 
	float3 /*const*/					m_start_propagation_position;
	//float3								m_current_proxy_position;
	// we save listener position for avoiding calculation problems 
	// when listener and propagator come closer too fast or move
	// away one from another
	float3								m_listener_position;
	sound_instance_proxy_internal&		m_proxy;
	sound_propagator_emitter const&		m_emitter;
	sound_producer const*				m_producer;
	sound_receiver const*				m_ignorable_receiver;
	sound_voice*						m_sound_voice;
	float /*const*/						m_end_propagation_distance;
	u32									m_end_propagation_time;
	u32									m_propagating_time;
	u32									m_sound_length;
	u32 /*const*/						m_sound_length_original;
	u32									m_sound_playing_time;
	u32									m_playing_offset;
	u32 /*const*/						m_playing_offset_original;
	u32									m_before_playing_offset;
	u32									m_after_playing_offset;
	u32									m_tick_precision_for_listener;
	playback_mode						m_mode;
	bool								m_is_on_finish_playing_called;
	bool								m_is_propagation_finished;
	bool								m_is_callbak_executer;
	bool								m_is_pause_producing_sound_requested;
	bool								m_is_pause_propagating_requested;
	bool								m_is_stop_producing_sound_requested;
	bool								m_is_state_preserved_for_resume;
	bool								m_is_sound_producing;
	// hdr audio members
	float								m_perceived_loudness;
	float								m_attenuated_loudness;
public:
	u32									m_dbg_id;
}; // class sound_propagator

template class XRAY_SOUND_API intrusive_list <	sound_propagator,
												sound_propagator*,
												&sound_propagator::m_next_for_proxies,
												threading::single_threading_policy	>;

typedef intrusive_list	<	sound_propagator,
							sound_propagator*,
							&sound_propagator::m_next_for_proxies,
							threading::single_threading_policy,
							size_policy
						>	propagators_list;

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_PROPAGATOR_H_INCLUDED
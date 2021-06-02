////////////////////////////////////////////////////////////////////////////
//	Created		: 20.06.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_VOICE_H_INCLUDED
#define SOUND_VOICE_H_INCLUDED

#include "voice_callback_handler.h"
#include <xray/sound/sound.h>
#include <xray/sound/channels_type.h>

namespace xray {
namespace sound {

class voice_bridge;
class sound_instance_proxy_internal;
class encoded_sound_interface;
class sound_spl;
class sound_scene;
class sound_propagator_emitter;
class sound_buffer;
class world_user;

typedef	resources::resource_ptr < encoded_sound_interface, resources::unmanaged_intrusive_base > encoded_sound_ptr;
typedef	resources::resource_ptr < sound_spl, resources::unmanaged_intrusive_base > sound_spl_ptr;

enum conversion_state
{
	conversion_not_requested = 0,
	conversion_requested,
	conversion_buffer_preparing,
	conversion_buffer_ready,
	conversion_buffer_submited
};

class sound_voice : private boost::noncopyable
{
public:
									sound_voice				( s32 playing_offset_in_msec, // can be negative, playing a quiet
															  u32 before_playing_quiet,
															  u32 after_playing_quiet,
															  sound_instance_proxy_internal& proxy,
															  sound_propagator_emitter const& emitter,
															  sound_spl_ptr const& spl 
															  );

									~sound_voice			( );

			
//			void					tick					( u32 time_delta_msec );

			void					play					( playback_mode mode = once );
			void					stop					( );
			
			bool					can_be_deleted			( ) const;

			void					set_output_matrix			( float const* level_matrix );
			void					set_low_pass_filter_params	( float coeff );

	inline	sound_instance_proxy_internal const& get_proxy	( ) const { return m_proxy; }

	inline	sound_spl_ptr const&	get_sound_spl			( ) const { return m_sound_spl; }

			sound_scene const&		get_sound_scene			( ) const;
			void					set_quality				( u32 quality );
// voice_callback_handler stuff
// must be called from voice_bridge class only
public:
			void		on_voice_processing_pass_start		( u32 bytes_required );
			void		on_voice_processing_pass_end		( );
			void		on_stream_end						( );
			void		on_buffer_start						( pvoid buffer_context );
			void		on_buffer_end						( pvoid buffer_context );
			void		on_loop_end							( pvoid buffer_context );
			void		on_voice_error						( pvoid buffer_context, HRESULT error );

// in sound thread callback implementation
private:
	void	on_stream_end_impl					( );
	void	on_buffer_end_impl					( pvoid pcontext );
	void	on_buffer_start_impl				( pvoid pcontext );
	void	on_buffer_error_impl				( pvoid pcontext );
private:
			void		refill_buffers								( );
			void		sumbmit_muted_sound_buffers_on_first_playing( );
			u32			find_nearest_adjective_pcm_offset			( ) const;
			void		on_finish_playing							( );
			void		submit_muted_buffers						( u32 msec );
public:
	sound_voice*					m_next_for_delete;
	sound_voice*					m_next_for_active;
private:
	sound_propagator_emitter const&	m_emitter;
	sound_spl_ptr const&			m_sound_spl;
	world_user&						m_world_user;
	voice_bridge*					m_voice;
	sound_instance_proxy_internal&	m_proxy;
	s32								m_playing_offset;
	u32								m_stream_cursor_pcm;
	u32								m_buffer_playing_offset;
	threading::atomic32_type		m_buffers_queued;
	u32								m_before_playing_quiet;
	u32								m_after_playing_quiet;
	u32								m_current_quality;
	u32								m_target_quality;

	channels_type					m_channels_type;

	sound_buffer*					m_conversion_buffer;
	sound_buffer*					m_conversion_buffer_test;

	bool							m_is_playing;
	threading::atomic32_type		m_is_quality_changing_requested;
	threading::atomic32_type		m_is_conversion_buffer_ready;
	bool							m_is_conversion_buffer_submited;
	playback_mode					m_mode;

	

	threading::atomic32_type		m_conv_state;
	u32								m_dbg_id;
	s16								tmp_buf[4];
	s16								m_test_buf[2];

	encoded_sound_ptr				m_current_sound_quality;
	encoded_sound_ptr				m_target_sound_quality;
}; // class sound_voice

} // namespace sound
} // namespace xray

#endif // #ifndef SOUND_VOICE_H_INCLUDED
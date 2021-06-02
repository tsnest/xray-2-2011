////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef SOUND_WORLD_H_INCLUDED
#define SOUND_WORLD_H_INCLUDED

#include <xray/sound/world.h>
#include <xray/sound/sound_instance_proxy.h>
#include <xray/memory_single_size_buffer_allocator.h>
#include <xray/unmanaged_allocation_resource.h>
#include <xray/sound/two_way_thread_channel.h>
#include "sound_scene.h"
#include "panning_lut.h"
#include "sound_voice.h"
#include "sound_instance_proxy_internal.h"
#include "console_commands.h"

struct IXAudio2Voice;
struct IXAudio2MasteringVoice;

namespace xray {

namespace collision {
	struct space_partitioning_tree;
} // namespace collision

namespace sound {

struct engine;
class world_user;
class sound_instance_emitter;
class sound_propagator_emitter;
class single_sound;
class voice_factory;
class sound_buffer_factory;
class sound_buffer;
class sound_instance_proxy_order;
class destroy_sound_instance_order;
class destroy_sound_instance_proxy_order;
struct play_params;

class atomic_float : private sound::noncopyable
{
public:
	inline	void	set	( float val )
	{
		atomic_float tmp;
		tmp.m_data.m_val				= val;
		threading::interlocked_exchange	( m_data.m_atomic, tmp.m_data.m_atomic );
	}
	inline	float	get	( ) const
	{
		atomic_float tmp;
		threading::interlocked_exchange	( tmp.m_data.m_atomic, m_data.m_atomic );
		return tmp.m_data.m_val;
	}
private:
	union {
		threading::atomic32_type	m_atomic;
		float						m_val;
	}								m_data;
}; // class atomic_float

class sound_world :
	public sound::world,
	private boost::noncopyable
{
public:
							sound_world				( engine& engine, base_allocator_type& logic_allocator, base_allocator_type* editor_allocator );
	virtual					~sound_world			( );
	
	virtual	void			tick					( );
	virtual	void			clear_resources			( );

	virtual	world_user&		get_logic_world_user	( ) const;
#ifndef MASTER_GOLD
	virtual	world_user&		get_editor_world_user	( ) const;
#endif // #ifndef MASTER_GOLD

			void			set_time_scale_factor	( float factor );
			float			get_time_scale_factor	( ) const;

	virtual	void			start_destruction		( );

	virtual void				set_calculation_type( calculation_type type );
	virtual calculation_type	get_calculation_type( ) const	{ return (calculation_type)m_calc_type; };

	inline	u32		get_last_time					( ) const { return m_last_current_time_in_ms; }
	inline	bool	is_destroying					( ) const { return m_is_destroying; }

			IXAudio2*		xaudio_engine			( ) const	{ return m_xaudio; }
	IXAudio2MasteringVoice*	xaudio_master_voice		( ) const	{ return m_master_voice; }
			channels_type	master_channel_type		( ) const;
	IXAudio2SubmixVoice*	create_submix_voice		( ) const;
			void			free_submix_voice		( IXAudio2SubmixVoice* voice ) const;

	inline	panning_lut_ptr	get_panning_lut			( ) const { return m_panning_lut; }

	inline	bool			is_audio_device_exist	( ) const	{ return m_is_audio_device_exist; }

			voice_bridge*	get_voice				( voice_callback_handler* callback_handler, IXAudio2SubmixVoice* submix_voice, channels_type type, u32 sample_rate  );
			void			free_voice				( voice_bridge* voice );
			sound_buffer*	get_sound_buffer		( encoded_sound_ptr const& encoded_sound, u32 pcm_offset, u32& next_pcm_offset );
			sound_buffer*	get_no_cahceable_sound_buffer( encoded_sound_ptr const& encoded_sound, u32 pcm_offset, u32& next_pcm_offset );
			void			free_sound_buffer		( sound_buffer* buffer );
			sound_buffer*	get_mute_sound_buffer	( encoded_sound_ptr const& encoded_sound );

			void			add_xaudio_order		( sound_order* order );
						
			void			set_listener_properties_impl		( sound_scene& scene, float4x4 const& inv_view_matrix );

			void			try_process_order					( sound_instance_proxy_order* order );
			void			process_destroy_sound_instance_proxy( destroy_sound_instance_proxy_order* order );

//			void	calculate_3d_sound							( sound_scene const& scene, sound_voice& voice );
//			void	calculate_channel_matrix					( sound_scene const& scene, sound_voice& voice, float* channel_matrix );
//			void	calculate_channel_matrix_with_pass_filters	( sound_scene const& scene, sound_voice& voice, float* channel_matrix );
//			void	calculate_volumetric_channel_matrix			( sound_scene const& scene, sound_voice& voice, float* channel_matrix );

			sound_voice*	create_sound_voice		(	sound_scene& scene,
														s32 playing_offset,
														u32 before_playing_offset,
														u32 after_playing_offset,
													    sound_instance_proxy_internal* proxy,
														sound_propagator_emitter const& emitter,
														sound_spl_ptr const& sound_spl );

			void			delete_sound_voice		( sound_scene& scene, sound_voice* voice );

			//serialization
			void	serialize_proxy					( sound_instance_proxy_internal* proxy, boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer* ) const;
			//void	serialize_proxy					( sound_instance_proxy_internal* proxy, boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer& writer );
			void	deserialize_proxy				( sound_instance_proxy_internal* proxy, memory::reader* r );
			void	on_proxy_serialized				( memory::writer* writer );

			// sound_scenes
			void	add_scene_to_active				( sound_scene& scene );
			void	remove_scene_from_active		( sound_scene& scene );


			void	set_active_sound_scene_impl		(	sound_scene& scene,
														u32 fade_in_time,
														u32 fade_out_old_scene_time  );

			void	remove_sound_scene_impl			( sound_scene_ptr scene );

			void	enable_current_scene_stream_writing		( );
			void	disable_current_scene_stream_writing	( );
			void	dump_current_scene_stream_writing		( );
			bool	is_current_scene_stream_writing_enabled	( ) const;
private:


	typedef memory::single_size_buffer_allocator< sizeof( sound_voice ),
												  threading::single_threading_policy > 
												  sound_voices_allocator;

	typedef intrusive_list	<	sound_voice,
								sound_voice*,
								&sound_voice::m_next_for_delete > voices_to_delete;

private:

			void	process_orders					( );
	
			void	register_sound_cooks			( );
			void	on_unmanaged_resources_allocated( resources::queries_result& queries );
			void	on_panning_lut_loaded			( resources::queries_result& queries );
			void	free_sound_instance_proxy		( sound_instance_proxy* proxy );

			void	hdr_audio_test					( u32 time_delta_in_msec );
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
			bool	initialize_xaudio				( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
			void	try_delete_stoping_voices		( );

private:
#ifndef MASTER_GOLD
	console_commands				m_console_commands;
#endif // #ifndef MASTER_GOLD
	timing::timer					m_timer;
	sound_orders_queue_type			m_xaudio_callback_orders;	// xaudio thread => sound thread


	resources::unmanaged_allocation_resource_ptr			m_unmanaged_resources_ptr;

	u32														m_sound_voices_count;
	uninitialized_reference< sound_voices_allocator >		m_sound_voices_allocator;

	world_user*						m_logic_world_user;
	world_user*						m_editor_world_user;

	voice_factory*					m_voice_factory;

	engine&							m_engine;
	// xaudio2
	IXAudio2*						m_xaudio;
	IXAudio2MasteringVoice*			m_master_voice;
//	X3DAUDIO_HANDLE					X3DInstance;	
	sound_buffer_factory*			m_sound_buffer_factory;

	panning_lut_ptr					m_panning_lut;

	voices_to_delete				m_voices_to_delete;

	u32								m_last_current_time_in_ms;
	atomic_float					m_time_factor;
	bool							m_is_destroying;
	bool							m_is_audio_device_exist;
	// for hdr audio test
	float							m_L_wintop;
	float							m_L_min;
	threading::atomic32_type		m_calc_type;

	sound_scenes_list				m_active_scenes;
	sound_scene*					m_current_scene;
	// debug stuff
	//X3DAUDIO_HANDLE					m_x3d_instance;
}; // class sound_world

} // namespace sound
} // namespace xray

#include "sound_world_inline.h"

#endif // #ifndef SOUND_WORLD_H_INCLUDED
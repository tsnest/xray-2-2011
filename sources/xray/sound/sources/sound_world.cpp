////////////////////////////////////////////////////////////////////////////
//	Created 	: 22.12.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_world.h"
#include <xray/collision/space_partitioning_tree.h>
#include <xray/sound/sound_receiver.h>
#include <xray/sound/single_sound.h>
#include <xray/sound/world_user.h>
#include "sound_propagator.h"
#include "ogg_source_cook.h"
#include "sound_collection_cook.h"
#include "composite_sound_cook.h"
#include "single_sound_cook.h"
#include "ogg_sound_cook.h"
#include "ogg_file_contents_cook.h"
#include "sound_rms_cook.h"
#include "encoded_sound_with_qualities_cook.h"
#include "ogg_encoded_sound_interface_cook.h"
#include "wav_encoded_sound_interface_cook.h"
#include "panning_lut_cook.h"
#include "spl_cook.h"
#include "sound_spl_cook.h"
#include "sound_scene_cook.h"
#include "voice_factory.h"
#include "sound_buffer_factory.h"
#include "sound_instance_proxy_order.h"
#include "sound_world_parameters.h"

namespace xray {
namespace sound {

static const float m_L_frame_fade_speed = 150.0f;

enum
{
	// this constant for playing sound earlier then needed (play quiet),
	// avoiding tick percission
	precalculation_time_for_propagators_in_msec			= 10,
};

#if !XRAY_PLATFORM_PS3
static void role_to_string ( XAUDIO2_DEVICE_ROLE const role, fixed_string2048& dest )
{
	if ( role == NotDefaultDevice )
		dest += "NotDefaultDevice ";
	else
	{
		if ( role & DefaultConsoleDevice )
			dest += "DefaultConsoleDevice ";

		if ( role & DefaultMultimediaDevice )
			dest += "DefaultMultimediaDevice ";

		if ( role & DefaultCommunicationsDevice )
			dest += "DefaultCommunicationsDevice ";

		if ( role & DefaultGameDevice )
			dest += "DefaultGameDevice ";

		if ( role & GlobalDefaultDevice )
			dest += "GlobalDefaultDevice ";
	}
}
#endif // #if !XRAY_PLATFORM_PS3

sound_world::sound_world	(
		xray::sound::engine& engine,
		base_allocator_type& logic_allocator,
		base_allocator_type* const editor_allocator
	) :
	m_engine					( engine ),
	m_logic_world_user			( 0 ),
	m_editor_world_user			( 0 ),
	m_last_current_time_in_ms	( 0 ),
	m_sound_voices_count		( 8 ),
	m_is_destroying				( false ),
	m_L_wintop					( 50.0f ),
	m_L_min						( 50.0f ),
	m_master_voice				( 0 ),
	m_is_audio_device_exist		( false ),
	m_panning_lut				( 0 ),
	m_calc_type					( 0 ),
	m_current_scene				( 0 )
#pragma warning( push )
#pragma warning( disable : 4355 )
#ifndef MASTER_GOLD
	,m_console_commands			( *this )
#endif // #ifndef MASTER_GOLD
#pragma warning( pop )
{
	m_time_factor.set			( 1.0f );

	m_logic_world_user			= XRAY_NEW_IMPL( g_allocator, world_user )( this, &logic_allocator );
	R_ASSERT					( m_logic_world_user );
#ifndef MASTER_GOLD
	if ( editor_allocator )
	{
		m_editor_world_user		= XRAY_NEW_IMPL( g_allocator, world_user )( this, editor_allocator );
		R_ASSERT				( m_editor_world_user );
	}
#endif // #ifndef MASTER_GOLD

	register_sound_cooks		( );

//	static console_commands::cc_float s_speed_of_sound_cc	("speed_of_sound", m_speed_of_sound, 0.001f, 1.0f, true, xray::console_commands::command_type_engine_internal);
	//static console_commands::cc_u32 s_test_timer_cc			("test_timer", m_test_timer, 0, 2, false, console_commands::command_type_engine_internal );
	//s_test_timer_cc.subscribe_on_change						( boost::bind( &sound_world::test_timer, this, _1 ));

//	static console_commands::cc_float s_test_time_factor_cc	("time_factor", (float&)m_time_factor, 0.0f, 5.0f, false, xray::console_commands::command_type_engine_internal);
//	s_test_time_factor_cc.subscribe_on_change				( boost::bind( &sound_world::test_timer, this, _1 ));

	u32 speed_of_sound_in_m_per_sec				= 0;
	if ( s_speed_of_sound.is_set_as_number( &speed_of_sound_in_m_per_sec ) )
		m_speed_of_sound					= m_speed_of_sound / 1000.0f;


	m_xaudio_callback_orders.set_pop_thread_id	( );
	m_xaudio_callback_orders.push_null_node		( MT_NEW( sound_order )() );

#if XRAY_PLATFORM_WINDOWS
	CoInitializeEx								( 0, COINIT_APARTMENTTHREADED );
#endif // #if XRAY_PLATFORM_WINDOWS

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_is_audio_device_exist						= initialize_xaudio	( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	pool_parametrs params;
	if ( m_is_audio_device_exist )
	{
		if(s_xaudio_mono_voices_count.is_set())
			s_xaudio_mono_voices_count.is_set_as_number(&params.mono_voices_count);
		else
			params.mono_voices_count	= xaudio_mono_voices_count_default_value;

		if(s_xaudio_stereo_voices_count.is_set())
			s_xaudio_stereo_voices_count.is_set_as_number(&params.stereo_voices_count);
		else
			params.stereo_voices_count	= xaudio_stereo_voices_count_default_value;
	}
	else
	{
		params.mono_voices_count		= 0;
		params.stereo_voices_count		= 0;
	}

	resources::creation_request lut_request	( 
		"panning_lut",
		sizeof( panning_lut ),
		resources::sound_panning_lut_class
	);

	channels_type type = stereo;
	resources::user_data_variant user_data;
	user_data.set( type );
	
	resources::query_resource_and_wait	(
		"panning_lut",
		resources::sound_panning_lut_class, 
		boost::bind( &sound_world::on_panning_lut_loaded, this, _1 ),
		g_allocator,
		&user_data 
	);

	if ( m_is_audio_device_exist )
	{
		u32 sound_voices_size			= m_sound_voices_count * sizeof( sound_voice ); 
		u32	voice_factory_size			= ( params.mono_voices_count + params.stereo_voices_count ) * sizeof( voice_bridge );
		u32 sound_buffer_count			= ( params.mono_voices_count + params.stereo_voices_count ) * 3;
		u32 sound_buffer_factory_size	= sound_buffer_count * real_sound_buffer_size_aligned;
		u32 allocation_size				= sound_voices_size + voice_factory_size + sound_buffer_factory_size;
		resources::creation_request request	(
			"unmanaged_sound_resources_allocation",
			allocation_size,
			resources::unmanaged_allocation_class
		);
		resources::query_create_resources_and_wait	(
			&request, 
			1,
			boost::bind( &sound_world::on_unmanaged_resources_allocated, this, _1 ), 
			g_allocator
		);


		u32 sound_voices_offset			= 0;
		XRAY_CONSTRUCT_REFERENCE		( m_sound_voices_allocator, sound_voices_allocator )( m_unmanaged_resources_ptr->buffer + sound_voices_offset, sound_voices_size );

		u32 voice_factory_offset		= sound_voices_offset + sound_voices_size;
		m_voice_factory					= NEW( voice_factory )( (u8*)( m_unmanaged_resources_ptr->buffer + voice_factory_offset ), voice_factory_size, *this, params );

		u32 sound_buffer_factory_offset	= voice_factory_offset + voice_factory_size;  
		m_sound_buffer_factory			= NEW( sound_buffer_factory )( (u8*)( m_unmanaged_resources_ptr->buffer + sound_buffer_factory_offset ), sound_buffer_factory_size, sound_buffer_count );
	}

	m_timer.start					( );
}

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
bool sound_world::initialize_xaudio		( )
{
	LOG_INFO									( "Sound initialization..." );

	u32 creation_flags = 0;
	if ( s_debug_audio )
		creation_flags	|= XAUDIO2_DEBUG_ENGINE;

	HRESULT res					= XAudio2Create( &m_xaudio, creation_flags, XAUDIO2_DEFAULT_PROCESSOR );

	if ( FAILED( res ) && s_debug_audio )
	{ // try load retail version of interface
		creation_flags			= 0;
		res						= XAudio2Create( &m_xaudio ,creation_flags, XAUDIO2_DEFAULT_PROCESSOR );
	}

	if ( FAILED( res ) )
	{
		LOG_ERROR				( "Sound initialization FAILED. Can not create XAudio2 interface. %d", res );
		R_ASSERT				( 0 );
	}

	u32 device_count			= 0;
	m_xaudio->GetDeviceCount	( &device_count );
	LOG_INFO					( "devices count: %d", device_count );


	if ( device_count == 0 )
	{
		LOG_ERROR				( "No audio device avalible" );
		return false;
	}

	if ( s_debug_audio )
	{
		XAUDIO2_DEBUG_CONFIGURATION dc;
		dc.TraceMask				= XAUDIO2_LOG_WARNINGS | XAUDIO2_LOG_FUNC_CALLS; // XAUDIO2_LOG_STREAMING;
		dc.BreakMask				= 0;
		dc.LogThreadID				= TRUE;
		dc.LogFileline				= TRUE;
		dc.LogFunctionName			= TRUE;
		dc.LogTiming				= 0;

		m_xaudio->SetDebugConfiguration(&dc);
	}

	XAUDIO2_DEVICE_DETAILS		deviceDetails;
	int preferred_device_id		= -1;
	fixed_string2048			device_role;

	for ( u32 i = 0; i < device_count; ++i )
	{
		m_xaudio->GetDeviceDetails	( i, &deviceDetails );
		device_role				=	"";
		LOG_INFO				( "%d: %S", i, deviceDetails.DisplayName );
		role_to_string			( deviceDetails.Role, device_role );
		LOG_INFO				( "role: %s", device_role.c_str() );
		if ( deviceDetails.Role & DefaultMultimediaDevice )
			preferred_device_id	= i;
	}
	
	if ( preferred_device_id == -1 )
	{
		LOG_ERROR				( "There is no Default Multimedia Device in system" );
		return false;
	}

	res							= m_xaudio->CreateMasteringVoice( 
										&m_master_voice, 
										XAUDIO2_DEFAULT_CHANNELS,
										XAUDIO2_DEFAULT_SAMPLERATE, 
										0, 
										preferred_device_id, 
										NULL 
										);
	ASSERT						( !FAILED( res ) );
	//// initialize X3DAudio
	m_xaudio->GetDeviceDetails		( preferred_device_id, &deviceDetails );
	//u32 channelMask					= deviceDetails.OutputFormat.dwChannelMask;
	//X3DAudioInitialize				( channelMask, X3DAUDIO_SPEED_OF_SOUND, m_x3d_instance );
	return true;
}

#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

void sound_world::on_unmanaged_resources_allocated	( resources::queries_result& queries )
{
	R_ASSERT					( queries[0].is_successful() );
	m_unmanaged_resources_ptr	= static_cast_resource_ptr<resources::unmanaged_allocation_resource_ptr>( queries[0].get_unmanaged_resource() );
}

void sound_world::on_panning_lut_loaded				( resources::queries_result& queries )
{
	R_ASSERT					( queries[0].is_successful() );
	m_panning_lut				= static_cast_resource_ptr<panning_lut_ptr>( queries[0].get_unmanaged_resource( ) );
}

sound_world::~sound_world		( )
{
	LOG_DEBUG					( "~sound_world" );
	R_ASSERT					( m_voices_to_delete.empty( ) );
	DELETE						( m_voice_factory );
	DELETE						( m_sound_buffer_factory );
#if !XRAY_PLATFORM_PS3
	if ( m_master_voice )
		m_master_voice->DestroyVoice( );
	m_xaudio->Release			( );
#endif // #if !XRAY_PLATFORM_PS3

	sound_order* order			= m_xaudio_callback_orders.pop_null_node();
	XRAY_DELETE_IMPL			( memory::g_mt_allocator, order );
}

channels_type sound_world::master_channel_type ( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAUDIO2_VOICE_DETAILS voice_details;
	m_master_voice->GetVoiceDetails( &voice_details );
	switch ( voice_details.InputChannels )
	{
	case 1:		return mono;
	case 2:		return stereo;
	default:	NODEFAULT( return channels_type_count );
	}
#else //  XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( return channels_type_count );
#endif //  XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

struct erase_unaudible_predicate
{
	erase_unaudible_predicate		( collision::objects_type const* objects ) :
		m_objects					( objects )
	{
		R_ASSERT					( m_objects );
	}
	
	inline bool operator			( ) ( collision::object const* obj )
	{
		return m_objects->end()		!= std::find( m_objects->begin(), m_objects->end(), obj );
	}

	collision::objects_type const*	m_objects;
};

void sound_world::tick			( )
{
	u32 const current_time_in_ms = m_timer.get_elapsed_msec( );
	R_ASSERT_CMP				( current_time_in_ms, >=, m_last_current_time_in_ms );
	u32 const time_delta		= current_time_in_ms - m_last_current_time_in_ms;
//	LOG_DEBUG					( "elapsed_ms: %u", current_time_in_ms );
//	LOG_DEBUG					( "time_delta: %u", time_delta );

	process_orders				( );

	sound_scene* scene			= m_active_scenes.front( );
	while ( scene )
	{
		scene->tick				( *this, time_delta );
		scene					= m_active_scenes.get_next_of_object( scene );
	}

	try_delete_stoping_voices	( );

	m_last_current_time_in_ms	= current_time_in_ms;

//	threading::yield			( 1000 );
}

IXAudio2SubmixVoice* sound_world::create_submix_voice	( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	if ( m_is_audio_device_exist )
	{
		XAUDIO2_VOICE_DETAILS voice_details;

		m_master_voice->GetVoiceDetails( &voice_details );

		IXAudio2SubmixVoice* submix_voice;
		HRESULT res			= m_xaudio->CreateSubmixVoice( &submix_voice, voice_details.InputChannels, voice_details.InputSampleRate ); 
		ASSERT				( !FAILED( res ) );
		res					= submix_voice->SetOutputVoices( NULL );
		ASSERT				( !FAILED( res ) );
		return submix_voice;
	}
#else	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED			( );
#endif	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	return 0;
}

void sound_world::free_submix_voice	( IXAudio2SubmixVoice* voice ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	if ( voice )
		voice->DestroyVoice		( );
#else	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED			( );
#endif	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
}

void sound_world::hdr_audio_test	( u32 time_delta_in_msec )
{
	XRAY_UNREFERENCED_PARAMETER		( time_delta_in_msec );

	//float l_n_sum					= 0.0f;
	//sound_instances_type::iterator it	= m_instances.begin();
	//sound_instances_type::iterator end	= m_instances.end();
	//for (; it != end; ++it)
	//{
	//	if ( (*it).is_hearable( ))
	//	{
	//		float dist		= get_distance_to_listener( (*it).get_position() );
	//		(*it).m_La		= 20* log10( 1.0f / ( (dist > 1.0f)? dist : 1.0f)) + 50.0f;
	//		(*it).m_Lp		= (*it).m_La + (*it).get_rms( ) * 10;

	//		//LOG_DEBUG		("dist - %f, La - %f, Lp - %f", dist, La, Lp );
	//		l_n_sum			+= pow(10.0f, (*it).m_Lp / 10.0f);
	//	}
	//}

	//// calc L_current
	//if ( !math::is_zero ( l_n_sum ) )
	//{
	//	float L_current					= 10.0f * log10(l_n_sum);
	//	if(L_current > m_L_wintop)
	//		m_L_wintop = L_current;
	//	else 
	//		m_L_wintop					-= ( m_L_wintop - L_current );

	//	if( m_L_wintop < m_L_min )
	//		m_L_wintop = m_L_min;

	//	//LOG_DEBUG		("m_L_wintop - %f, m_L_min - %f", m_L_wintop, m_L_min );
	//}

	//{
	//	sound_instances_type::iterator it	= m_instances.begin();
	//	sound_instances_type::iterator end	= m_instances.end();
	//	for (; it != end; ++it)
	//	{
	//		//sound_instance_ptr ptr		= &(*it);
	//		//float k						= pow(5.0f, ((*it).m_Lp - m_L_wintop) / 20.0f);
	//		//float a_fin					= pow(10.0f, ((*it).m_La + k * (*it).get_rms( ) * 10 - m_L_wintop) / 20.0f);


	//		//LOG_DEBUG					( "a_fin %f, ", a_fin );

	//		//pE->m_A_fin				=	0.9f*pE->m_A_fin + 0.1f*a_fin;
	//	}
	//}

}

voice_bridge* sound_world::get_voice ( voice_callback_handler* callback_handler, IXAudio2SubmixVoice* submix_voice, channels_type type, u32 sample_rate  )
{
	voice_bridge* voice		= m_voice_factory->new_voice	( callback_handler, type, sample_rate );
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	voice->set_output_voice	( submix_voice );
#endif	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	
	return voice;
}

void sound_world::free_voice ( voice_bridge* voice )
{
	R_ASSERT							( voice );
	voice->set_output_voice				( 0 );
	m_voice_factory->delete_voice		( voice );
}

sound_buffer* sound_world::get_sound_buffer( encoded_sound_ptr const& encoded_sound, u32 pcm_offset, u32& next_pcm_offset )
{
	return m_sound_buffer_factory->new_sound_buffer(encoded_sound, pcm_offset, next_pcm_offset);
}

sound_buffer* sound_world::get_no_cahceable_sound_buffer( encoded_sound_ptr const& encoded_sound, u32 pcm_offset, u32& next_pcm_offset )
{
	return m_sound_buffer_factory->new_no_cahceable_sound_buffer(encoded_sound, pcm_offset, next_pcm_offset);
}

void sound_world::free_sound_buffer	( sound_buffer* buffer )
{
	R_ASSERT									( buffer );
	m_sound_buffer_factory->delete_sound_buffer	( buffer );
}

sound_buffer* sound_world::get_mute_sound_buffer	( encoded_sound_ptr const& encoded_sound )
{
	return m_sound_buffer_factory->get_mute_sound_buffer( encoded_sound );
}

void sound_world::register_sound_cooks	( )
{
	static ogg_source_cook						s_ogg_source_cook;
	static ogg_sound_cook						s_ogg_sound_cook;
	static sound_rms_cook						s_sound_rms_cook;
	static ogg_file_contents_cook				s_ogg_file_contents_cook;
	static sound_collection_cook				s_sound_cll_cook( *this );
	static composite_sound_cook					s_composite_sound_cook( *this );
	static single_sound_cook					s_single_sound_cook;
	static encoded_sound_with_qualities_cook	s_encoded_sound_with_qualities_cook;
	static ogg_encoded_sound_interface_cook		s_ogg_encoded_sound_interface_cook;
	static wav_encoded_sound_interface_cook		s_wav_encoded_sound_interface_cook;
	static spl_cook								s_spl_cook;
	static sound_spl_cook						s_sound_spl_cook;
	static panning_lut_cook						s_panning_lut_cook;
	static sound_scene_cook						s_sound_scene_cook( *this );

	register_cook								( &s_ogg_source_cook );
	register_cook								( &s_ogg_sound_cook );
	register_cook								( &s_ogg_file_contents_cook );
	register_cook								( &s_sound_rms_cook );
	register_cook								( &s_sound_cll_cook );
	register_cook								( &s_composite_sound_cook );
	register_cook								( &s_single_sound_cook );
	register_cook								( &s_encoded_sound_with_qualities_cook );
	register_cook								( &s_ogg_encoded_sound_interface_cook );
	register_cook								( &s_wav_encoded_sound_interface_cook );
	register_cook								( &s_spl_cook );
	register_cook								( &s_sound_spl_cook );
	register_cook								( &s_panning_lut_cook );
	register_cook								( &s_sound_scene_cook );
}
	
void sound_world::clear_resources	( )
{
	LOG_DEBUG						 ( "sound_world::clear_resources" );

	sound_scene* scene				= m_active_scenes.front( );
	while ( scene )
	{
		scene->stop					( );
		scene						= m_active_scenes.get_next_of_object( scene );
	}	

	tick							( );

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: avoid this") )
	static u32 i					= 0;
	while ( i < 5 && !m_voices_to_delete.empty( ) )
	{
		tick							( );
		threading::yield				( 10 );
		++i;
	}

	R_ASSERT						( m_voices_to_delete.empty( ) );
}

void sound_world::add_xaudio_order	( sound_order* const order )
{
	R_ASSERT						( order );

	static bool s_initialized		= false;
	if ( !s_initialized ) {
		s_initialized				= true;
		m_xaudio_callback_orders.set_push_thread_id	( );
	}

	m_xaudio_callback_orders.push_back	( order );
}

void sound_world::process_orders	( )
{
	// process xaudio2 orders
	while ( !m_xaudio_callback_orders.empty() )
	{
		sound_order*					to_delete;
		sound_order* const order		= m_xaudio_callback_orders.pop_front( to_delete );
		order->execute					( );
		MT_DELETE						( to_delete );
	}

	R_ASSERT							( m_logic_world_user );
	m_logic_world_user->process_orders	( );

#ifndef MASTER_GOLD
	if ( m_editor_world_user )
		m_editor_world_user->process_orders	( );	
#endif // #ifndef MASTER_GOLD
}

world_user& sound_world::get_logic_world_user ( ) const
{
	R_ASSERT							( m_logic_world_user );
	return *m_logic_world_user;
}

#ifndef MASTER_GOLD
world_user& sound_world::get_editor_world_user ( ) const
{
	R_ASSERT							( m_logic_world_user );
	return *m_editor_world_user;
}
#endif // #ifndef MASTER_GOLD

void sound_world::start_destruction			( )
{
	R_ASSERT						( !m_is_destroying );
	m_is_destroying					= true;

	//sound_instances_type::iterator it	= m_instances.begin();
	//sound_instances_type::iterator end	= m_instances.end();
	//for (; it != end; ++it)
	//{
	//	if ( it->is_playing_once ( ) )
	//		it->finish_playing_once	( );
	//}

	process_orders					( );
}

void sound_world::set_calculation_type	( calculation_type type )
{
	threading::interlocked_exchange ( m_calc_type, (u32)type );
}

//float3 sound_world::get_listener_position			( ) const
//{
//#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
//	return float3 ( m_listener.Position.x, m_listener.Position.y, m_listener.Position.z ); 
//#else	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
//	return float3 ( 0.0f, 0.0f, 0.0f );
//#endif	// #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
//}



sound_voice* sound_world::create_sound_voice(	sound_scene& scene,
												s32 playing_offset,
												u32 before_playing_offset,
												u32 after_playing_offset,
												sound_instance_proxy_internal* proxy,
												sound_propagator_emitter const& emitter,
												sound_spl_ptr const& sound_spl )
{
	if ( !m_is_audio_device_exist )
	{
		LOG_ERROR( "can't allocate sound_voice, audio device do not exist:(" );
		return 0;
	}

	if ( m_sound_voices_allocator->total_size() == m_sound_voices_allocator->allocated_size() )
	{
		LOG_ERROR( "can't allocate sound_voice, memory pool is empty :(" );
		return 0;
	}

	sound_voice* new_sound_voice		= XRAY_NEW_IMPL( m_sound_voices_allocator.c_ptr(), sound_voice )( playing_offset, before_playing_offset, after_playing_offset, *proxy, emitter, sound_spl ); 
	scene.add_active_voice				( *new_sound_voice );
	return new_sound_voice;
}

void sound_world::delete_sound_voice( sound_scene& scene, sound_voice* voice )
{
	R_ASSERT						( voice );
	scene.remove_active_voice		( *voice );
	if ( voice->can_be_deleted( ) )
		XRAY_DELETE_IMPL			( m_sound_voices_allocator.c_ptr(), voice );
	else
		m_voices_to_delete.push_back( voice );
}

void sound_world::try_delete_stoping_voices	( )
{
	sound_voice* voice				= m_voices_to_delete.front( );
	while ( voice )
	{
		sound_voice* next			= m_voices_to_delete.get_next_of_object( voice );
		if ( voice->can_be_deleted() )
		{
			m_voices_to_delete.erase	( voice );
			XRAY_DELETE_IMPL			( m_sound_voices_allocator.c_ptr(), voice );
		}
		voice						= next;
	}
}

void sound_world::add_scene_to_active		( sound_scene& scene )
{
	R_ASSERT						( !m_active_scenes.contains_object( &scene ) );
	m_active_scenes.push_back		( &scene );
}

void sound_world::remove_scene_from_active	( sound_scene& scene )
{
	R_ASSERT						( m_active_scenes.contains_object( &scene ) );
	m_active_scenes.erase			( &scene );
}

void sound_world::set_active_sound_scene_impl(	sound_scene& scene,
																u32 fade_in_time,
																u32 fade_out_time  )
{
	if ( m_current_scene )
	{
		m_current_scene->fade_out	( fade_out_time );

		#ifndef MASTER_GOLD
		if ( m_current_scene->is_debug_stream_writing_enabled( ) )
			m_current_scene->disable_debug_stream_writing( );
		#endif //#ifndef MASTER_GOLD
	}

	m_current_scene					= &scene;
	m_current_scene->fade_in		( *this, fade_in_time );
}

void sound_world::remove_sound_scene_impl	( sound_scene_ptr scene )
{
	sound_scene* scn		= static_cast_checked<sound_scene*>( scene.c_ptr( ) );
	scn->stop				( );
}


void sound_world::set_time_scale_factor		( float factor )
{
	R_ASSERT							( !( factor < 0.0f ) );

	float const previous_time_factor	= m_time_factor.get( );
	float const min_freq_ratio			= m_voice_factory->get_min_frequency_ratio( );

	// can't set time factor between 0.0 and min_freq_ratio
	factor = factor < min_freq_ratio ? 0.0f : factor;

	if ( math::is_similar( previous_time_factor, factor ) )
		return;

	m_time_factor.set					( factor );
	m_timer.set_time_factor				( factor );

	if ( math::is_zero( factor ) )
	{
		sound_scene* scene					= m_active_scenes.front( );
		while ( scene )
		{
			R_ASSERT							( !scene->is_paused( ) );
			scene->pause						( );
			scene								= m_active_scenes.get_next_of_object( scene );
		}
	}
	else
	{
		// maybe we need update time factor only for active and new voices
		m_voice_factory->set_frequency_ratio( factor );
		if ( !math::is_zero( previous_time_factor ) )
			return;

		sound_scene* scene					= m_active_scenes.front( );
		while ( scene )
		{
			R_ASSERT							( scene->is_paused( ) );
			scene->resume						( );
			scene								= m_active_scenes.get_next_of_object( scene );
		}
	}
}

float sound_world::get_time_scale_factor	( ) const
{
	return m_time_factor.get				( );
}

#ifndef MASTER_GOLD
void sound_world::enable_current_scene_stream_writing		( )
{
	R_ASSERT								( m_current_scene );

	if ( is_current_scene_stream_writing_enabled( ) )
	{
		LOG_ERROR	( "trying to enable current scene stream writing which already enabled" );
		return;
	}

	m_current_scene->enable_debug_stream_writing			( );
}

void sound_world::disable_current_scene_stream_writing		( )
{
	R_ASSERT								( m_current_scene );

	if ( !is_current_scene_stream_writing_enabled( ) )
	{
		LOG_ERROR	( "trying to disable current scene stream writing which currently not enabled" );
		return;
	}

	m_current_scene->disable_debug_stream_writing			( );
}

void sound_world::dump_current_scene_stream_writing			( )
{
	R_ASSERT								( m_current_scene );

	if ( !is_current_scene_stream_writing_enabled( ) )
	{
		LOG_ERROR	( "trying to dump current scene stream, but stream writing doesn't enabled" );
		return;
	}

	m_current_scene->dump_debug_stream_writing				( );
}

bool sound_world::is_current_scene_stream_writing_enabled	( ) const
{
	R_ASSERT								( m_current_scene );
	return m_current_scene->is_debug_stream_writing_enabled	( );
}
#endif //#ifndef MASTER_GOLD

} // namespace sound
} // namespace xray

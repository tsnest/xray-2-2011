////////////////////////////////////////////////////////////////////////////
//	Created		: 24.02.2011
//	Author		: Dmitriy Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "voice_factory.h"
#include "sound_world.h"

namespace xray {
namespace sound {

voice_factory::voice_factory( u8* buffer, u32 buffer_size, sound_world const& world, pool_parametrs const& params ) :
	m_pool_params				( params ),
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_min_frequency_ratio		( XAUDIO2_MIN_FREQ_RATIO ),
	m_max_frequency_ratio		( XAUDIO2_DEFAULT_FREQ_RATIO ),
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_min_frequency_ratio		( 1.0f ),
	m_max_frequency_ratio		( 1.0f ),
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	m_voices_allocator			( buffer, buffer_size )
{

	voice_bridge::creation_parametrs	voice_params;
	voice_params.xaudio_engine			= world.xaudio_engine( );
	voice_params.master_channels_type	= world.master_channel_type( );
	voice_params.type					= mono;
	voice_params.max_frequency_ratio	= m_max_frequency_ratio;

	for (u32 i = 0; i < params.mono_voices_count; ++i)
	{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		voice_bridge* new_voice	= XRAY_NEW_IMPL( m_voices_allocator, voice_bridge)( voice_params );
		m_voices[mono].push_back( new_voice );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	}

	voice_params.type					= stereo;

	for (u32 i = 0; i < params.stereo_voices_count; ++i)
	{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		voice_bridge* new_voice	= XRAY_NEW_IMPL( m_voices_allocator, voice_bridge)( voice_params );
		m_voices[stereo].push_back( new_voice );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED ( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	}
}

voice_factory::~voice_factory			( )
{
	for (u32 i = 0; i < channels_type_count; ++i)
	{
		voice_bridge* current_voice	= m_voices[i].front( );
		while( current_voice )
		{
			voice_bridge* object_to_be_deleted		= current_voice;
			
			current_voice							= voice_list_type::get_next_of_object( current_voice );
			XRAY_DELETE_IMPL						( m_voices_allocator, object_to_be_deleted );
		}
	}

	//FREE						( m_memory );
}

voice_bridge* voice_factory::new_voice ( voice_callback_handler* callback_handler, channels_type type, u32 sample_rate )
{
	//// for test's
	//{
	//	LOG_DEBUG				("new_voice test");
	//	voice_bridge* idle_voice	= m_voices[type].front( );
	//	while ( idle_voice )
	//	{
	//		LOG_DEBUG			("idle_voice->has_handler( ) %d", idle_voice->has_handler( ) );
	//		idle_voice			= voice_list_type::get_next_of_object( idle_voice );
	//	}
	//}

	R_ASSERT					(type < channels_type_count);
	voice_bridge* idle_voice	= m_voices[type].front( );
	while ( idle_voice )
	{
		if (!idle_voice->has_handler( ))
		{
			idle_voice->set_handler		( callback_handler );
			idle_voice->set_sample_rate	( sample_rate );
			return idle_voice;
		}

		idle_voice			= voice_list_type::get_next_of_object( idle_voice );
	}

	UNREACHABLE_CODE		( return 0 );
}

void voice_factory::delete_voice( voice_bridge* voice_to_be_deleted)
{
	LOG_DEBUG								("voice_factory::delete_voice");
	channels_type const type				= voice_to_be_deleted->get_channels_type( );
	R_ASSERT_U								( m_voices[type].contains_object( voice_to_be_deleted ));
	voice_to_be_deleted->set_handler		( 0 );
	voice_to_be_deleted->set_output_voice	( 0 );
}

void voice_factory::set_frequency_ratio				( float ratio )
{
	R_ASSERT										( !( ratio < m_min_frequency_ratio ) );
	R_ASSERT										( !( ratio > m_max_frequency_ratio ) );

	for ( u32 i = 0; i < channels_type_count; ++i )
	{
		voice_bridge* current_voice	= m_voices[i].front( );
		while( current_voice )
		{
			current_voice->set_frequency_ratio		( ratio );
			current_voice							= voice_list_type::get_next_of_object( current_voice );
		}
	}
}

} // namespace sound
} // namespace xray

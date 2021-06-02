////////////////////////////////////////////////////////////////////////////
//	Created		: 24.03.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_world.h"
#include <xray/sound/world_user.h>
#include <xray/sound/sound_instance_emitter.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/single_sound.h>
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/collision/object.h>
#include <xray/sound/sound_receiver.h>
#include "voice_bridge.h"
#include "sound_instance_proxy_order.h"

namespace xray {
namespace sound {

void sound_world::set_listener_properties_impl( sound_scene& scene, float4x4 const& inv_view_matrix )
{
	scene.set_listener_properties( inv_view_matrix );
}

//#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
//static X3DAUDIO_VECTOR float3_to_xaudio_vector( float3 const& src )
//{
//	X3DAUDIO_VECTOR vec;
//	vec.x					= src[0];
//	vec.y					= src[1];
//	vec.z					= src[2];
//	return vec;
//}
//#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

//void sound_world::calculate_3d_sound( sound_scene const& scene, sound_voice& voice )
//{
//	float channel_matrix[2];
//
//	float3 list_orient_front			= scene.get_listenet_orient_front();
//	float3 list_orient_top				= scene.get_listenet_orient_top();
//	float3 position						= scene.get_listenet_position();
//
//	X3DAUDIO_LISTENER test_listener1	= { 0 };
//	test_listener1.OrientFront			= float3_to_xaudio_vector( list_orient_front );
//    test_listener1.OrientTop			= float3_to_xaudio_vector( list_orient_top );
//    test_listener1.Position				= float3_to_xaudio_vector( position );
//    test_listener1.pCone				= NULL;
//
//	X3DAUDIO_EMITTER emitter1;
//    emitter1.pCone						= NULL;
//    emitter1.OrientFront				= float3_to_xaudio_vector( float3( 1.0f, 0.0f, 0.0f ) );
//    emitter1.OrientTop					= float3_to_xaudio_vector( float3( 0.0f, 1.0f, 0.0f ) );
//    emitter1.Position					= float3_to_xaudio_vector( voice.get_position( ) );
//	emitter1.Velocity					= float3_to_xaudio_vector( float3( 0.0f, 0.0f, 0.0f ) );
//    emitter1.InnerRadius				= 0.0f;
//    emitter1.InnerRadiusAngle			= 0.0f;
//    emitter1.ChannelCount				= 1;
//    emitter1.ChannelRadius				= 0.0f;
//    emitter1.pChannelAzimuths			= NULL;
//    emitter1.pVolumeCurve				= NULL;
//    emitter1.pLFECurve					= NULL;
//    emitter1.pLPFDirectCurve			= NULL;
//    emitter1.pLPFReverbCurve			= NULL;
//    emitter1.pReverbCurve				= NULL;
//    emitter1.CurveDistanceScaler		= 1.0f;
//    emitter1.DopplerScaler				= 1.0f;
//
//	float mtx_coeff[2];
//	X3DAUDIO_DSP_SETTINGS				dsp_settings = { 0 };
//	dsp_settings.pMatrixCoefficients	= mtx_coeff;
//	dsp_settings.SrcChannelCount		= 1;
//	dsp_settings.DstChannelCount		= 2;
//
//	X3DAudioCalculate					( 
//		m_x3d_instance,
//		&test_listener1,
//		&emitter1,
//		X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_LPF_DIRECT,
//		&dsp_settings
//	);
//
//	LOG_DEBUG							( "XAudio calc -----------------------------------------" );
//	LOG_DEBUG							( "left channel		: %f", dsp_settings.pMatrixCoefficients[0] );
//	LOG_DEBUG							( "right channel	: %f", dsp_settings.pMatrixCoefficients[1] );
//	LOG_DEBUG							( "lpf direct coeff	: %f", dsp_settings.LPFDirectCoefficient );
//
//	if ( sound_voice::positional ==	voice.get_voice_type() )
//	{
//		calculate_channel_matrix_with_pass_filters( scene, voice, channel_matrix );
//	//	calculate_channel_matrix	( scene, voice, channel_matrix );
//	}
//	else if ( sound_voice::volumetric == voice.get_voice_type() )
//		calculate_volumetric_channel_matrix	( scene, voice, channel_matrix );
//	else
//	{
//		UNREACHABLE_CODE	( );
//	}
//
//	voice.set_output_matrix		( channel_matrix );
//}

void sound_world::try_process_order	( sound_instance_proxy_order* order	)
{
	R_ASSERT						( order );
	return order->execute_functor	( );
}

void	sound_world::process_destroy_sound_instance_proxy (destroy_sound_instance_proxy_order* order )
{
	R_ASSERT											( order );
	LOG_DEBUG											( "process_destroy_sound_instance_proxy" );

	sound_instance_proxy_internal* proxy				= &order->get_proxy();
	proxy->get_sound_scene().stop_propagate_sound		( *proxy );
	R_ASSERT											( proxy->get_propagators().empty() );
	proxy->get_sound_scene().free_sound_instance_proxy	( proxy );
}


void	sound_world::serialize_proxy	( sound_instance_proxy_internal* proxy, boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer* w )  const
{
	R_ASSERT									( proxy );
	memory::writer* sound_thread_writer			= XRAY_NEW_IMPL( g_allocator, memory::writer )( g_allocator );
	sound_thread_writer->write_u32				( proxy->get_propagators( ).size( ) );

	sound_propagator* prop			= proxy->get_propagators( ).front( );
	while ( prop )
	{
		prop->serialize				( *sound_thread_writer );
		prop						= proxy->get_propagators( ).get_next_of_object( prop );
	}

	functor_response* response		= XRAY_NEW_IMPL ( proxy->get_world_user().get_channel().responses.owner_allocator(), functor_response )
		( boost::bind( &sound_instance_proxy_internal::on_propagators_serialized, proxy, fn, sound_thread_writer, w ) );
	proxy->get_world_user().add_response	( response );
}

void	sound_world::on_proxy_serialized	( memory::writer* writer )
{
	R_ASSERT					( writer );
	XRAY_DELETE_IMPL			( g_allocator, writer );
}

void	sound_world::deserialize_proxy	( sound_instance_proxy_internal* proxy, memory::reader* r )
{
	R_ASSERT							( proxy );
	R_ASSERT							( r );
	XRAY_UNREFERENCED_PARAMETERS		( proxy, r );
	//sound_propagator* prop			= proxy->get_propagators( ).front( );
	//while ( prop )
	//{
	//	sound_propagator* next		= proxy->get_propagators( ).get_next_of_object( prop );
	//	delete_sound_propagator		( proxy, prop );
	//	prop						= next;
	//}

	//u32 props_count			= r->r_u32( );
	//for ( u32 i = 0; i < props_count; ++i )
	//{
	//	u64 emitter_address		= r->r_u64( );

	//	sound_propagator_emitter const* emt	= proxy->get_sound_propagator_emitter( ).get_sound_propagator_emitter( emitter_address );
	//	R_ASSERT						( emt != 0 );
	//	sound_propagator* prop	= get_sound_propagator( 
	//		*emt, 
	//		proxy,
	//		float3( .0f, .0f, .0f ),
	//		once,
	//		0,
	//		0,
	//		0 );
	//	prop->deserialize	( *r );
	//	proxy->get_propagators( ).push_back( prop );
	//}
}

} // namespace sound
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 02.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_scene.h"
#include <xray/collision/api.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/sound_receiver.h>
#include <xray/sound/sound_spl.h>
#include <xray/sound/world_user.h>
#include <xray/sound/single_sound.h>
#include <xray/sound/sound_debug_stats.h>
#include <xray/core_entry_point.h>
#include "sound_instance_proxy_order.h"
#include "sound_world.h"

namespace xray {
namespace sound {

enum
{
	// this constant for playing sound earlier then needed (play quiet),
	// avoiding tick percission
	precalculation_time_for_propagators_in_msec			= 10,
};

receiver_collision::receiver_collision	( sound_receiver* receiver, atomic_half3* pos ) :
	m_receiver	( receiver ),
	m_next		( 0 ),
	m_position	( pos )
{
	float r					= 0.3f;
	m_collision				= &*collision::new_aabb_object
	( 
		g_allocator,
		1,
		m_position->get( ),
		float3( r, r, r ),
		this
	);
}

receiver_collision::~receiver_collision		( )
{
	collision::delete_object			( g_allocator, m_collision );
	m_collision							= 0;
}

void receiver_collision::delete_position	( sound_scene& scene )
{
	scene.delete_receiver_position		( m_position );
	m_position							= 0;
}

sound_scene::sound_scene	( IXAudio2SubmixVoice* submix_voice, u32 dbg_id ) :
	m_next						( 0 ),
	m_proxies_count				( 16 ),
	m_propagators_count			( 16 ),
	m_receivers_count			( 16 ),
	m_unmanaged_resources_ptr	( 0 ),
	m_spatial_tree				( 0 ),
	m_submix_voice				( submix_voice ),
	m_is_active					( false ),
	m_fade_state				( none ),
	m_fade_in_time				( 0 ),
	m_fade_out_time				( 0 ),
	m_volume					( 0.0f ),
	m_fade_vol_per_msec			( 0.0f ),
	m_is_paused					( false ),
	m_dbg_id					( dbg_id )
{
#ifndef MASTER_GOLD
	m_debug_snapshot					= 0;
	m_is_debug_stream_writing_enabled	= 0;
#endif // #ifndef MASTER_GOLD

	init_allocators						( );

	ASSERT								( !m_spatial_tree );
	m_spatial_tree						= &*collision::new_space_partitioning_tree( g_allocator, 0.0001f, 1024 );
	ASSERT								( m_spatial_tree );

	memory::zero						( &m_listener, sizeof( listener ) );
}

struct receiver_unconditional_erasing_predicate : private boost::noncopyable
{
	inline void operator()	( receiver_collision* receiver_container ) const
	{
		DELETE				( receiver_container );
	}	
};

sound_scene::~sound_scene	( )
{
	R_ASSERT						( !is_debug_stream_writing_enabled( ) );
	delete_space_partitioning_tree	( m_spatial_tree );

	if ( !m_receivers.empty( ) )
	{
		m_receivers.for_each	( receiver_unconditional_erasing_predicate() );
		m_receivers.clear		( );
	}

	R_ASSERT					( m_receivers.empty( ) );

	LOG_DEBUG					( "~sound_scene() id: %d", m_dbg_id );
}

void sound_scene::stop		( )
{
	LOG_DEBUG									( "sound_scene::stop" );
	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		stop_propagate_sound					( *proxy );
		proxy									= m_active_proxies.get_next_of_object( proxy );
	}
}

void sound_scene::tick		( sound_world& world, u32 time_delta )
{
	R_ASSERT					( m_is_active );
	if ( m_is_paused )
		return;

	process_fade				( world, time_delta );

	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		sound_instance_proxy_internal* next_proxy	= m_active_proxies.get_next_of_object( proxy );
		proxy->tick									( time_delta );
		proxy										= next_proxy;
	}

	update_receivers_position	( );
	notify_receivers			( );
	update_listener_properties	( );
	notify_listener				( world );
}

void sound_scene::update_listener_properties	( )
{
	m_listener.m_position		= m_list_position.get( );
	m_listener.m_orient_front	= m_list_orient_front.get( );
	m_listener.m_orient_top		= m_list_orient_top.get( );
}

sound_instance_proxy_ptr sound_scene::create_sound_instance_proxy	
										(
											sound_emitter_ptr emitter,
											sound_propagator_emitter const& propagator_emitter,
											world_user& user
										)
{
	LOG_DEBUG							( "sound_scene::create_sound_instance_proxy" );
	if ( m_proxies_allocator->total_size( ) == m_proxies_allocator->allocated_size( ) )
	{
		LOG_ERROR						( "can't allocate sound_instance_proxy, memory is full" );
		return sound_instance_proxy_ptr	( 0 );
	}

	sound_instance_proxy* new_proxy		= XRAY_NEW_IMPL( m_proxies_allocator.c_ptr(), sound_instance_proxy_internal )( *this, emitter, propagator_emitter, user );
	LOG_INFO							( "new sound_instance_proxy allocated, id %d", new_proxy->get_id( ) );
	return sound_instance_proxy_ptr		( new_proxy );
}

sound_instance_proxy_ptr sound_scene::create_sound_instance_proxy	
										(
											sound_emitter_ptr emitter,
											sound_propagator_emitter const& propagator_emitter,
											world_user& user,
											sound_cone_type cone_type
										)
{
	LOG_DEBUG							( "sound_scene::create_sound_instance_proxy" );
	if ( m_proxies_allocator->total_size( ) == m_proxies_allocator->allocated_size( ) )
	{
		LOG_ERROR						( "can't allocate sound_instance_proxy, memory is full" );
		return sound_instance_proxy_ptr	( 0 );
	}

	sound_instance_proxy* new_proxy		= XRAY_NEW_IMPL( m_proxies_allocator.c_ptr(), sound_instance_proxy_internal )( *this, emitter, propagator_emitter, user, cone_type );
	LOG_INFO							( "new sound_instance_proxy allocated, id %d", new_proxy->get_id( ) );
	return sound_instance_proxy_ptr		( new_proxy );
}

sound_instance_proxy_ptr sound_scene::create_sound_instance_proxy	
											(
												sound_emitter_ptr emitter,
												sound_propagator_emitter const& propagator_emitter,
												world_user& user,
												collision::geometry_instance& geometry,
												float radius
											)
{
	LOG_DEBUG							( "sound_scene::create_sound_instance_proxy" );
	if ( m_proxies_allocator->total_size( ) == m_proxies_allocator->allocated_size( ) )
	{
		LOG_ERROR						( "can't allocate sound_instance_proxy, memory is full" );
		return sound_instance_proxy_ptr	( 0 );
	}

	sound_instance_proxy* new_proxy		= XRAY_NEW_IMPL( m_proxies_allocator.c_ptr(), sound_instance_proxy_internal )( *this, emitter, propagator_emitter, user, geometry, radius );
	LOG_INFO							( "new sound_instance_proxy allocated, id %d", new_proxy->get_id( ) );
	return sound_instance_proxy_ptr		( new_proxy );
}

void sound_scene::free_sound_instance_proxy	( sound_instance_proxy* proxy )
{
	LOG_INFO							( "sound instance proxy with id %d is deallocated", proxy->get_id( ) );
	XRAY_DELETE_IMPL					( m_proxies_allocator.c_ptr(), proxy );
}

void sound_scene::emit_sound_propagators	(	sound_instance_proxy_internal& proxy,
												playback_mode mode,
												sound_producer const* const producer,
												sound_receiver const* const ignorable_receiver	)
{
	LOG_DEBUG							( "sound_scene::emit_sound_propagators" );
	create_sound_propagator_params params
		( 
			mode,
			proxy,
			producer,
			ignorable_receiver
		);

	typedef sound_instance_proxy_order_with_data< create_sound_propagator_params > order_type;

	order_type::functor_type functor	= boost::bind 
		( 
			&sound_scene::emit_sound_propagators_impl,
			this,
			_1
		);

	order_type* order	= 
		XRAY_NEW_IMPL	( proxy.get_world_user( ).get_allocator( ), order_type )
		( proxy.get_world_user( ), proxy, functor, params );

//	LOG_INFO							( "create_sound_propagator ORDER: 0x%8x with id %d", propagator_emitter, instance_id );
	proxy.get_world_user( ).add_order	( order );
}

void sound_scene::emit_sound_propagators	(	sound_instance_proxy_internal& proxy,
												source_params const& params,
												playback_mode mode, 
												sound_producer const* const producer,
												sound_receiver const* const ignorable_receiver
											)
{
	LOG_DEBUG							( "sound_scene::emit_sound_propagators" );
	create_sound_propagator_params prop_params
		( 
			params,
			mode,
			proxy,
			producer,
			ignorable_receiver
		);

	typedef sound_instance_proxy_order_with_data< create_sound_propagator_params > order_type;

	order_type::functor_type functor	= boost::bind 
		( 
			&sound_scene::emit_sound_propagators_impl,
			this,
			_1
		);

	order_type* order	= 
		XRAY_NEW_IMPL	( proxy.get_world_user( ).get_allocator( ), order_type )
		( proxy.get_world_user( ), proxy, functor, prop_params );

//	LOG_INFO							( "create_sound_propagator ORDER: 0x%8x with id %d", propagator_emitter, instance_id );
	proxy.get_world_user( ).add_order	( order );
}

void sound_scene::emit_sound_propagators_impl
									(
										create_sound_propagator_params const& params
									)
{	
	LOG_DEBUG							( "sound_scene::emit_sound_propagators_impl" );
	sound_instance_proxy_internal& proxy		= params.m_proxy;
	sound_propagator_emitter const& emitter		= proxy.get_sound_propagator_emitter( );

	u32 old_props_count							= proxy.get_propagators( ).size( );


	emitter.emit_sound_propagators
	(
		proxy,
		params.m_mode,
		0,
		0,
		params.m_producer,
		params.m_ignorable_receiver
	);

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: determine which propagator should execute callback ( for compositie sound )") )
	
	sound_propagator* prop					= proxy.get_propagators( ).front( );

	for ( u32 i = 0; i < old_props_count; ++i )
	{
		prop					=  proxy.get_propagators( ).get_next_of_object( prop );
	}

	sound_propagator* max_duration_prop		= prop;

	while ( prop )
	{
		u32 cur_offset	= prop->get_sound_length_original( ) + prop->get_before_palying_offset( );
		u32 max_offset	= max_duration_prop->get_sound_length_original( ) + max_duration_prop->get_before_palying_offset( );
		if ( cur_offset > max_offset )
			max_duration_prop		= prop;
		prop			= proxy.get_propagators( ).get_next_of_object( prop );
	}

	max_duration_prop->set_as_callback_executer( true );
//	proxy.get_propagators( ).front( )->set_as_callback_executer( true );
	LOG_INFO									( "sound propagators with id %d created", params.m_proxy.get_id() );

	if ( old_props_count == 0 )
	{
		R_ASSERT									( !m_active_proxies.contains_object( &params.m_proxy ));
		m_active_proxies.push_back					( &proxy );
	}
}

sound_propagator* sound_scene::create_sound_propagator
									(	
										sound_propagator_emitter const& owner,
										sound_instance_proxy_internal& proxy,
										playback_mode mode,
										u32 playing_offset,
										u32 before_playing_offset,
										u32 after_playing_offset,
										sound_producer const* const producer,
										sound_receiver const* const ignorable_receiver
									) 
{
	LOG_DEBUG							( "sound_scene::create_sound_propagator" );
	if ( m_propagators_allocator->total_size() == m_propagators_allocator->allocated_size() )
	{
		LOG_ERROR( "can't allocate sound_propagator, memory pool is empty :(" );
		return 0;
	}

	sound_propagator* new_propagator	= XRAY_NEW_IMPL
										( m_propagators_allocator.c_ptr(), sound_propagator )
										( 
											mode,
											playing_offset,
											before_playing_offset,
											after_playing_offset,
											proxy,
											owner,
											producer,
											ignorable_receiver
										);

	return new_propagator;
}

sound_propagator* sound_scene::create_sound_propagator_for_looped( sound_propagator& original )
{
	LOG_DEBUG							( "sound_scene::create_sound_propagator_for_looped" );
	if ( m_propagators_allocator->total_size() == m_propagators_allocator->allocated_size() )
	{
		LOG_ERROR( "can't allocate sound_propagator, memory pool is empty :(" );
		return 0;
	}

	sound_propagator* new_propagator	= XRAY_NEW_IMPL
										( m_propagators_allocator.c_ptr(), sound_propagator )
										( 
											original.get_playback_mode( ),
											0,
											original.get_before_palying_offset( ),
											original.get_after_palying_offset( ),
											original.get_proxy( ),
											original.get_propagator_emitter( ),
											original.get_producer( ),
											original.get_ignorable_receiver( )
										);

	return new_propagator;
}


void sound_scene::delete_sound_propagator	( sound_instance_proxy_internal& proxy, sound_propagator* propagator )
{
	LOG_DEBUG							( "sound_scene::delete_sound_propagator" );
	if ( propagator == 0 )
	{
		LOG_ERROR						( "can't delete sound_propagator, pointer == 0" );
		return;
	}

	R_ASSERT							( proxy.get_propagators().contains_object( propagator ));
	proxy.get_propagators().erase		( propagator );
	XRAY_DELETE_IMPL					( m_propagators_allocator.c_ptr(), propagator );

	R_ASSERT							( m_active_proxies.contains_object( &proxy ) );
	if ( proxy.get_propagators().empty() )
		m_active_proxies.erase			( &proxy );
}

void sound_scene::init_allocators	( )
{
	LOG_DEBUG						( "sound_scene::init_allocators" );
	u32	proxies_size				= m_proxies_count * sizeof( sound_instance_proxy_internal );
	u32	propagators_size			= m_propagators_count * sizeof( sound_propagator );
	u32 receiver_positions_size		= m_receivers_count * sizeof ( atomic_half3 );
	u32 receiver_collisions_size	= m_receivers_count * sizeof ( receiver_collision );
	u32 allocation_size				= proxies_size + propagators_size + receiver_positions_size + receiver_collisions_size;


	resources::creation_request request	
	(
		"unmanaged_sound_resources_allocation",
		allocation_size,
		resources::unmanaged_allocation_class
	);

	resources::query_create_resources_and_wait	
	(
		&request, 
		1,
		boost::bind( &sound_scene::on_unmanaged_resources_allocated, this, _1 ), 
		g_allocator
	);

	u32 proxies_offset				= 0;
	XRAY_CONSTRUCT_REFERENCE		( m_proxies_allocator, sound_proxies_allocator )
									( m_unmanaged_resources_ptr->buffer + proxies_offset, proxies_size );

	u32 propagators_offset			= proxies_offset + proxies_size;
	XRAY_CONSTRUCT_REFERENCE		( m_propagators_allocator, sound_propagators_allocator )
									( m_unmanaged_resources_ptr->buffer + propagators_offset, propagators_size );

	u32 receivers_positions_offset	= propagators_offset + propagators_size;
	XRAY_CONSTRUCT_REFERENCE		( m_receiver_positions_allocator, receiver_position_allocator )
									( m_unmanaged_resources_ptr->buffer + receivers_positions_offset, receiver_positions_size );

	u32 receivers_collisions_offset	= receivers_positions_offset + receiver_positions_size;
	XRAY_CONSTRUCT_REFERENCE		( m_receiver_collisions_allocator, receiver_collision_allocator )
									( m_unmanaged_resources_ptr->buffer + receivers_collisions_offset, receiver_collisions_size );
}

void sound_scene::on_unmanaged_resources_allocated	( resources::queries_result& queries )
{
	R_ASSERT					( queries[0].is_successful() );
	m_unmanaged_resources_ptr	= static_cast_resource_ptr<resources::unmanaged_allocation_resource_ptr>( queries[0].get_unmanaged_resource() );
}

void sound_scene::notify_receivers	( )
{
	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		proxy->notify_receivers					( *m_spatial_tree );
		proxy									= m_active_proxies.get_next_of_object( proxy );
	}
}

void sound_scene::notify_listener	( sound_world const& world )
{
	sound_instance_proxy_internal* proxy	= m_active_proxies.front( );
	while ( proxy )
	{
		sound_propagator* propagator		= proxy->get_propagators().front();
		while ( propagator )
		{
			if ( propagator->is_propagation_paused	( ) )
			{
				propagator			= proxy->get_propagators().get_next_of_object( propagator );
				continue;
			}

			float3 prop_pos;
			sound_type type			= proxy->get_sound_type( );
			switch ( type )
			{
			case cone: 
			case point:				prop_pos = proxy->get_position( ); break;
			case volumetric:		prop_pos = proxy->get_volumetric_position( m_listener.m_position ); break;
			default:				NODEFAULT( );
			}

			float real_dist_to_list	= math::length( prop_pos - m_listener.m_position );

			float inner_radius	= propagator->get_propagation_inner_radius_for_listener( );
			float outer_radius	= propagator->get_propagation_outer_radius_for_listener( );

			float unheard_dist	= propagator->get_unheard_distance( );

			if ( real_dist_to_list > unheard_dist )
			{
				if ( propagator->has_sound_voice() )
					propagator->detach_sound_voice		( );
				propagator			= proxy->get_propagators().get_next_of_object( propagator );
				continue;
			}

			float distance_per_tick	= proxy->get_world_user( ).get_sound_world( )->get_speed_of_sound() * precalculation_time_for_propagators_in_msec;
			if ( real_dist_to_list >= inner_radius && real_dist_to_list - distance_per_tick <= outer_radius )
			{
				//LOG_DEBUG							( "listener inside propagator area" );
				//propagator->get_sound_rms_value		( );
				//props_for_hdr_audio.push_back		( propagator );
				if ( !propagator->has_sound_voice() )
					propagator->attach_sound_voice	( precalculation_time_for_propagators_in_msec );
			}
			else if ( propagator->has_sound_voice() )
			{
				//LOG_DEBUG							( "listener outside propagator area" );
//				propagator->get_propagation_inner_radius_for_listener( );
				propagator->detach_sound_voice		( );
			}

			propagator			= proxy->get_propagators().get_next_of_object( propagator );
		}
		proxy				=  m_active_proxies.get_next_of_object( proxy );
	}

	sound_voice* voice		= m_active_voices.front( );
	while ( voice )
	{
		calculate_3d_sound			( *voice, world.get_panning_lut() );
		voice						= m_active_voices.get_next_of_object( voice );
	}
	calculate_hdr_audio				( );
}

static u32 cart_to_lut_position	(float re, float im)
{
    u32 pos = 0;
	float denom = math::abs(re) + math::abs(im);
    if(denom > 0.0f)
		pos = (u32)(panning_lut::quadrant_num*math::abs(im) / denom + 0.5);

    if(re < 0.0)
        pos = 2 * panning_lut::quadrant_num - pos;
    if(im < 0.0)
        pos = panning_lut::lut_num - pos;
    return pos%panning_lut::lut_num;
}

static float coeff_calc		( float g, float cw )
{
    float a		= 0.0f;

    /* Be careful with gains < 0.01, as that causes the coefficient
     * head towards 1, which will flatten the signal */
	g = math::max(	g, 0.01f );
    if ( g < 0.9999f ) /* 1-epsilon */
		a = (1 - g*cw - math::sqrt ( 2 * g * ( 1 - cw ) - g * g * ( 1 - cw * cw ))) / (1 - g);

    return a;
}

void sound_scene::calculate_3d_sound	( sound_voice& voice, panning_lut_ptr panning_lut )
{
	//const u32 MAX_SENDS		= 4;

	float dry_gain_hf		= 1.0f;

	//float wet_gain_hf[MAX_SENDS];
	//for( u32 i = 0; i < MAX_SENDS; ++i )
	//	wet_gain_hf[i]		= 1.0f;

	sound_instance_proxy_internal const& proxy	= voice.get_proxy( );
	sound_type type								= proxy.get_sound_type( );
	//get context properties
	//u32 num_sends				= 1;
	u32 frequency				= 44100;
	float head_dampen			= 0.25f;
	u32 num_channels			= 2;
	//get source properties
    float source_volume			= 1.0f;
    float min_volume			= 0.0f;
    float max_volume			= 1.0f;
	float min_dist				= 1.0f;
	float max_dist				= math::float_max;

	float inner_angle			= 360.0f;
	float outer_angle			= 360.0f;
	float outer_gain			= 0.0f;

	if ( type == cone )
	{
		switch( proxy.get_cone_type ( ) )
		{
		case human:
			{
				inner_angle			= 60.0f;
				outer_angle			= 90.0f;
				outer_gain			= 0.5f;
			} break;
		default:NODEFAULT			( );
		}
	}

	float outer_gain_hf			= 1.0f;
	float cone_volume			= 0.0f;
	float cone_hf				= 0.0f;
	bool dry_gain_hf_auto		= true;
	float air_absorption_factor	= 2.0f;

	float3 u, v, n;
	float4x4 matrix;

	n					= get_listenet_orient_front( );
	n.normalize			( );
	
	v					= get_listenet_orient_top( );
	v.normalize			( );

	u					= cross_product ( n, v );
	u.normalize			( );

	matrix.i[0] = u[0]; matrix.i[1] = v[0]; matrix.i[2] = -n[0]; matrix.i[3] = 0.0f;
	matrix.j[0] = u[1]; matrix.j[1] = v[1]; matrix.j[2] = -n[1]; matrix.j[3] = 0.0f;
	matrix.k[0] = u[2]; matrix.k[1] = v[2]; matrix.k[2] = -n[2]; matrix.k[3] = 0.0f;
	matrix.c[0] = 0.0f; matrix.c[1] = 0.0f; matrix.c[2] =  0.0f; matrix.c[3] = 1.0f;

	float3 position					= float3( 0.0f, 0.0f, 0.0f );
	float3 direction				= float3( 0.0f, 0.0f, 0.0f );

	switch( type )
	{
	case cone:			direction		= proxy.get_direction( );
	case point:			position		= proxy.get_position( );	break;
	case volumetric:	position		= proxy.get_volumetric_position( get_listenet_position( ) ); break;
	default:			NODEFAULT		( );
	}

	// Translate position
	position							-= get_listenet_position( );

	// Transform source position and direction into listener space
	float4 temp_position				( position, 1.0f );
	float4 temp_direction				( direction, 0.0f );

	float4x4 mtx						= math::transpose( matrix );

	position							= ( temp_position * matrix ).xyz( );
	direction							= ( temp_direction * matrix ).xyz( );

	float3 source_to_listener			= -position;
	source_to_listener.normalize_safe	( source_to_listener );
	direction.normalize_safe			( direction );

	//2. Calculate distance attenuation
	float distance						= math::sqrt( dot_product ( position, position ) );
    float orig_dist						= distance;

	distance							= math::max ( distance, min_dist );
	distance							= math::min ( distance, max_dist );

	float attenuation					= voice.get_sound_spl( )->get_loudness( distance );;
	
	//if( (min_dist + (distance - min_dist)) > 0.0f )
	//	attenuation = min_dist / (min_dist + (distance - min_dist));


	// Source Gain + Attenuation
    float dry_gain						= source_volume * attenuation;
	//float wet_gain		[MAX_SENDS];
	//for( u32 i = 0; i < num_sends; ++i )
	//{
	//	wet_gain[i]		= source_volume; //* RoomAttenuation[i];
	//	wet_gain_hf[i]	= 1.0f;
	//}

	float effective_dist = 0.0f;
    if( min_dist > 0.0f && attenuation < 1.0f )
        effective_dist = ( min_dist/attenuation - min_dist);//*MetersPerUnit;

   // Distance-based air absorption
    if( air_absorption_factor > 0.0f && effective_dist > 0.0f)
    {
        float absorb;

        // Absorption calculation is done in dB
		float const AIRABSORBGAINDBHF          = -0.05f;
        absorb = (air_absorption_factor*AIRABSORBGAINDBHF) * effective_dist;
        // Convert dB to linear gain before applying
		absorb = math::pow(10.0f, absorb/20.0f);

        dry_gain_hf *= absorb;
    }

	// apply directional soundcones
	float angle			= math::acos( direction.dot_product( source_to_listener )) * 180.0f / math::pi;
    if ( angle >= inner_angle && angle <= outer_angle )
    {
        float scale		= ( angle - inner_angle ) / ( outer_angle-inner_angle );
        cone_volume		= ( 1.0f + ( outer_gain - 1.0f ) * scale );
		cone_hf			= ( 1.0f + ( outer_gain_hf - 1.0f ) * scale );
    }
    else if(angle > outer_angle)
    {
        cone_volume		= ( 1.0f + ( outer_gain - 1.0f ) );
        cone_hf			= ( 1.0f + ( outer_gain_hf - 1.0f ) );
    }
    else
    {
        cone_volume		= 1.0f;
        cone_hf			= 1.0f;
    }

	// Apply some high-frequency attenuation for sources behind the listener
	// NOTE: This should be dot product({0,0,-1}, ListenerToSource), however
	// that is equivalent to dot product({0,0,1}, SourceToListener), which is
	// the same as SourceToListener[2]
	angle				= math::acos ( source_to_listener[2] ) * 180.0f / math::pi;

    // Sources within the minimum distance attenuate less
    if( orig_dist < min_dist)
        angle			*= orig_dist / min_dist;
    if( angle > 90.0f )
    {
        float scale		= ( angle - 90.0f) / ( 180.1f - 90.0f ); // .1 to account for fp errors
        cone_hf			*= 1.0f - ( head_dampen * scale );
    }

	dry_gain			*= cone_volume;
    if( dry_gain_hf_auto )
        dry_gain_hf		*= cone_hf;

    // Clamp to Min/Max Gain
	dry_gain			= math::min(dry_gain, max_volume );
    dry_gain			= math::max(dry_gain, min_volume );

    // Use energy-preserving panning algorithm for multi-speaker playback
	float length		= math::max( orig_dist, min_dist );
    if( length > 0.0f )
    {
		position.normalize_safe	( float3( 0.0f, 0.0f, 0.0f ));
    }

    u32 pos					= cart_to_lut_position( -position[2], position[0] );
	float* speaker_gain		= &((*panning_lut)[channels_count * pos]);
	float dir_gain			= math::sqrt( position[0]*position[0] + position[2]*position[2] );

	// elevation adjustment for directional gain. this sucks, but
	// has low complexity
	float ambient_gain		= math::sqrt( 1.0f / num_channels );

	float ch_l				= 0.0f;
	float ch_r				= 0.0f;

	for( u32 s = 0; s < num_channels; ++s )
    {
		u32 chan		= s; // Speaker2Chan
		float gain		= ambient_gain + ( speaker_gain[chan] - ambient_gain ) * dir_gain;
		
		switch ( s )
		{
		case 0: ch_l		= dry_gain * gain; break;
		case 1: ch_r		= dry_gain * gain; break;
		default: NODEFAULT( );
		}
    }

	//float sum				= ch_l + ch_r;
	
	//float spl_attenuation	= voice.get_sound_spl( )->get_loudness( distance );

	float channel_matrix[2];
	channel_matrix[0]				= ( ch_r/* / sum*/ );// * spl_attenuation;
	channel_matrix[1]				= ( ch_l/* / sum*/ );// * spl_attenuation;

//	LOG_DEBUG						( "left		: %f", channel_matrix[0] );
//	LOG_DEBUG						( "right	: %f", channel_matrix[1] );

	/* Update filter coefficients. */
	float const LOWPASSFREQCUTOFF   = 5000;
	float cw						= math::cos	( math::pi_x2 * LOWPASSFREQCUTOFF / frequency );

	float iir_filter_coeff			= coeff_calc	( math::sqrt( dry_gain_hf ), cw ) * 1;

	if ( type == volumetric )
	{
		float3 volumetric_listener_pos	= get_listenet_position( );
		float3 volumetric_source_pos	= proxy.get_volumetric_position( volumetric_listener_pos );
		float volumetric_distance		= ( volumetric_listener_pos - volumetric_source_pos ).length( );
		
		if ( volumetric_distance > proxy.get_volumetric_radius( ))
		{
			voice.set_output_matrix			( channel_matrix );
			voice.set_low_pass_filter_params( 1.0f - iir_filter_coeff );
			return;
		}

		float coeff					= volumetric_distance / proxy.get_volumetric_radius( );
		channel_matrix[0]			*= coeff;
		channel_matrix[1]			*= coeff;
		
		float stereo_coeff			= 1.0f - coeff; 
		channel_matrix[0]			+= stereo_coeff;
		channel_matrix[1]			+= stereo_coeff;

		if ( channel_matrix[0] > 1.0f )
			channel_matrix[0]			= 1.0f;

		if ( channel_matrix[1] > 1.0f )
			channel_matrix[1]			= 1.0f;

		if ( math::is_similar( volumetric_listener_pos, volumetric_source_pos ) )
			iir_filter_coeff			= 0.0f;
	}

	voice.set_output_matrix			( channel_matrix );
	voice.set_low_pass_filter_params( 1.0f - iir_filter_coeff );


	//LOG_DEBUG							( "OpenaAL calc -----------------------------------------" );
	//LOG_DEBUG							( "left channel		: %f", channel_matrix[0] );
	//LOG_DEBUG							( "right channel	: %f", channel_matrix[1] );
	//LOG_DEBUG							( "lpf direct coeff	: %f", iir_filter_coeff );
}

void sound_scene::calculate_hdr_audio	( )
{
	//float l_n_sum											= 0.0f;
	//// hdr audio calculation
	//{
	//	std::list<sound_propagator*>::const_iterator it_begin	= props_for_hdr_audio.begin	( );
	//	std::list<sound_propagator*>::const_iterator it_end		= props_for_hdr_audio.end	( );
	//	for ( ; it_begin != it_end; ++it_begin )
	//	{
	//		float dist			= get_distance_to_listener( (*it_begin)->get_position() );
	//		float al			= (*it_begin)->get_sound_spl( dist );
	//		float rms			= (*it_begin)->get_sound_rms_value( ) * 100.0f;
	//		float pl			= al + rms;

	//		//		if ( (*it_begin)->get_sound_rms_value( ) < math::epsilon_3 && (*it_begin)->has_sound_voice( ) )
	//		//			(*it_begin)->detach_sound_voice	( );

	//		//		LOG_DEBUG			( "al: %f", al );
	//		//		LOG_DEBUG			( "rms: %f", rms );
	//		//		LOG_DEBUG			( "pl: %f", pl );

	//		(*it_begin)->set_attenuated_loudness	( al );
	//		(*it_begin)->set_percived_loudness		( pl );
	//		////	(*it)->m_La		= 20* log10( 1.0f / ( (dist > 1.0f)? dist : 1.0f)) + 50.0f;
	//		////	(*it)->m_Lp		= (*it_begin)->m_La + (*it_begin)->get_rms( ) * 10;

	//		////	LOG_DEBUG		("dist - %f, La - %f, Lp - %f", dist, La, Lp );
	//		l_n_sum				+= pow(10.0f, pl / 10.0f);
	//	}
	//}


	//// calc L_current
	//if ( !math::is_zero ( l_n_sum ) )
	//{
	//	float L_current					= 10.0f * log10(l_n_sum);
	//	if(L_current > m_L_wintop)
	//		m_L_wintop = L_current;
	//	else 
	//		m_L_wintop					-= ( m_L_wintop - L_current ) * ( m_L_frame_fade_speed * time_delta );

	//	if( m_L_wintop < m_L_min )
	//		m_L_wintop = m_L_min;

	//	LOG_DEBUG		("m_L_wintop - %f, m_L_min - %f", m_L_wintop, m_L_min );
	//}

	//std::list<sound_propagator*>::const_iterator it_begin	= props_for_hdr_audio.begin	( );
	//std::list<sound_propagator*>::const_iterator it_end		= props_for_hdr_audio.end	( );
	//for ( ; it_begin != it_end; ++it_begin )
	//{
	//	if ( (*it_begin)->get_percived_loudness( ) >  m_L_min )
	//	{
	//		if ( !(*it_begin)->has_sound_voice() )
	//			(*it_begin)->attach_sound_voice	( time_delta );
	//	}
	//	else if ( (*it_begin)->has_sound_voice() )
	//		(*it_begin)->detach_sound_voice();
	//}
}

struct compare_receivers_predicate : private boost::noncopyable
{
	inline compare_receivers_predicate	( sound_receiver* const receiver )
		:	m_receiver					( receiver )
	{
	}

	inline bool	operator ( )			( receiver_collision* const container ) const
	{
		return container->get_sound_receiver( ) == m_receiver;
	}
	
	sound_receiver* const				m_receiver;
}; // struct receivers_erase_predicate


void sound_scene::register_receiver		( sound_receiver* receiver, atomic_half3* position )
{
	LOG_DEBUG							( "sound_scene::register_receiver" );
	compare_receivers_predicate pred	( receiver );
	receiver_collision* new_receiver	= m_receivers.find_if( pred );

	R_ASSERT							( new_receiver == 0 );

	if ( m_receiver_collisions_allocator->total_size( ) == m_receiver_collisions_allocator->allocated_size( ) )
	{
		LOG_ERROR					( "can't allocate receiver_collision, memory is full" );
		return;
	}

	new_receiver			= XRAY_NEW_IMPL( m_receiver_collisions_allocator.c_ptr(), receiver_collision )( receiver, position );

	m_receivers.push_back	( new_receiver );

	float4x4 scale			= math::create_scale( new_receiver->get_collision_object( )->get_aabb().extents( ));
	float4x4 translation	= math::create_translation( new_receiver->get_position( ) );
	float4x4 local_to_world	= scale * translation;
	m_spatial_tree->insert	
	(
		new_receiver->get_collision_object( ),
		local_to_world
	);

}

void sound_scene::unregister_receiver	( world_user& user, sound_receiver* receiver )
{
	LOG_DEBUG							( "unregister_receiver_impl" );
	compare_receivers_predicate pred	( receiver );
	receiver_collision* collision		= m_receivers.find_if( pred );

	if ( collision )
	{
		m_receivers.erase				( collision );
		m_spatial_tree->erase			( collision->get_collision_object() );
		collision->delete_position		( *this );
		XRAY_DELETE_IMPL				( m_receiver_collisions_allocator.c_ptr( ), collision );
	}
	else
		LOG_ERROR						( "attempt to delete not unregistered receiver" );

	user.on_receiver_deleted			( (u64)receiver );

	if ( !m_receivers.empty() )
		LOG_INFO	( "sound receiver unregistered, but list of registered receivers isn't empty" );
	else
		LOG_INFO	( "list of registered receivers is empty" );
}

void sound_scene::stop_produce_sound		( sound_instance_proxy_internal& proxy ) const
{
	sound_propagator* prop				= proxy.get_propagators( ).front( );
	while ( prop )
	{
		if ( prop->is_sound_producing( ) )
			prop->stop_produce_sound	( );
		prop							= proxy.get_propagators( ).get_next_of_object( prop );
	}
}

void sound_scene::stop_propagate_sound	( sound_instance_proxy_internal& proxy )
{
	LOG_DEBUG						( "sound_scene::stop_propagate_sound" );
	sound_propagator* prop			= proxy.get_propagators( ).front( );
	while ( prop )
	{
		prop->stop_propagate_sound	( );
		sound_propagator* next		= proxy.get_propagators( ).get_next_of_object( prop );
		R_ASSERT					( prop->can_be_deleted( ) );

		delete_sound_propagator		( proxy, prop );
	
		prop						= next;
	}
}

void sound_scene::pause_produce_sound	( sound_instance_proxy_internal& proxy ) const
{
	LOG_DEBUG						( "sound_scene::pause_produce_sound" );
	sound_propagator* prop			= proxy.get_propagators( ).front( );
	while ( prop )
	{
		if ( prop->is_sound_producing( ) )
			prop->pause_produce_and_preserve_state_for_resume	( );
		prop						= proxy.get_propagators( ).get_next_of_object( prop );
	}
}

void sound_scene::pause_propagate_sound	( sound_instance_proxy_internal& proxy ) const
{
	LOG_DEBUG						( "sound_scene::pause_propagate_sound" );
	sound_propagator* prop			= proxy.get_propagators( ).front( );
	while ( prop )
	{
		if ( !prop->is_propagation_paused( ) && !prop->is_propagation_finished( ) )
			prop->pause_propagation		( );
		prop						= proxy.get_propagators( ).get_next_of_object( prop );
	}
}

void sound_scene::resume_produce_sound	( sound_instance_proxy_internal& proxy )
{
	LOG_DEBUG						( "sound_scene::resume_produce_sound" );
	std::list<sound_propagator*>	temp_propagators;

	sound_propagator* prop			= proxy.get_propagators( ).front( );
	while ( prop )
	{
		if ( prop->is_state_preserved_for_resume( ) )
		{
			sound_propagator* new_propagator			= create_sound_propagator
			(
				prop->get_propagator_emitter( ),
				proxy,
				prop->get_playback_mode( ),
				prop->get_playing_offset( ),
				prop->get_before_palying_offset( ),
				prop->get_after_palying_offset( ),
				prop->get_producer( ),
				prop->get_ignorable_receiver( )
				);

			new_propagator->set_as_callback_executer	( prop->is_callback_executer( ) );
			prop->set_as_callback_executer				( false );
			temp_propagators.push_back					( new_propagator );
			prop->clear_preservation_state_for_resume	( );
		}
		prop						= proxy.get_propagators( ).get_next_of_object( prop );
	}

	std::list<sound_propagator*>::iterator it			= temp_propagators.begin( );
	std::list<sound_propagator*>::const_iterator it_end	= temp_propagators.end( );
	while ( it != it_end )
	{
		proxy.add_sound_propagator					( *(*it) );
		++it;
	}
}

void sound_scene::resume_propagate_sound	( sound_instance_proxy_internal& proxy ) const
{
	LOG_DEBUG						( "sound_scene::resume_propagate_sound" );
	sound_propagator* prop			= proxy.get_propagators( ).front( );
	while ( prop )
	{
		if ( prop->is_propagation_paused( ) )
			prop->resume_propagation	( );
		prop						= proxy.get_propagators( ).get_next_of_object( prop );
	}

}

void sound_scene::set_listener_properties( float4x4 const& inv_view_matrix )
{
	m_listener.m_orient_front		= inv_view_matrix.k.xyz();
	m_listener.m_orient_top			= inv_view_matrix.j.xyz();
	m_listener.m_position			= inv_view_matrix.c.xyz();
}

void sound_scene::set_listener_properties	(	
												float3 const& position,
												float3 const& orient_front,
												float3 const& orient_top
											)
{
	m_list_position.set		( position );
	m_list_orient_front.set	( orient_front );
	m_list_orient_top.set	( orient_top );
}

void sound_scene::fade_in	( sound_world& world, u32 time_in_msec )
{
	LOG_DEBUG						( "sound_scene::fade_in" );
	if ( !m_is_active )
	{
		world.add_scene_to_active	( *this );
		m_is_active					= true;
	}

	m_fade_state			= fade_in_state;
	m_fade_in_time			= time_in_msec;

	m_fade_vol_per_msec		= 1.0f;
	if ( time_in_msec != 0 )
		m_fade_vol_per_msec		= 1.0f / time_in_msec;

	resume_propagate_all_sounds			( );
}

void sound_scene::fade_out	( u32 time_in_msec )
{
	LOG_DEBUG				( "sound_scene::fade_out" );
	R_ASSERT				( m_is_active );
	m_fade_state			= fade_out_state;
	m_fade_out_time			= time_in_msec;

	m_fade_vol_per_msec		= 1.0f;
	if ( time_in_msec != 0 )
		m_fade_vol_per_msec		= 1.0f / time_in_msec;
}

void sound_scene::process_fade	( sound_world& world, u64 time_delta )
{
	R_ASSERT					( m_is_active );

	if ( m_fade_state == fade_in_state )
	{
		m_volume		+= m_fade_vol_per_msec * time_delta;
		if ( m_volume >= 1.0f )
		{
			m_volume		= 1.0f;
			m_fade_state	= none;
		}
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		if ( m_submix_voice )
			m_submix_voice->SetVolume( m_volume );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	}
	else if ( m_fade_state == fade_out_state )
	{
		m_volume		-= m_fade_vol_per_msec * time_delta;
		
		if ( m_volume < 0.0f || math::is_zero( m_volume ) )
		{
			m_volume								= 0.0f;
			m_fade_state							= none;
			pause_propagate_all_sounds				( );

			world.remove_scene_from_active			( *this );
			m_is_active								= false;
		}
		LOG_DEBUG								( "volume: %f", m_volume );
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
		if ( m_submix_voice )
			m_submix_voice->SetVolume( m_volume );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	}
}

atomic_half3* sound_scene::create_receiver_position	( )
{
	if ( m_receiver_positions_allocator->total_size( ) == m_receiver_positions_allocator->allocated_size( ) )
	{
		LOG_ERROR					( "can't allocate receiver_position, memory is full" );
		return						0;
	}

	atomic_half3* pos	= XRAY_NEW_IMPL( m_receiver_positions_allocator.c_ptr(), atomic_half3 )( );
	return pos;
}

void sound_scene::delete_receiver_position	( atomic_half3* pos )
{
	R_ASSERT			( pos );
	XRAY_DELETE_IMPL	( m_receiver_positions_allocator.c_ptr(), pos );
	pos					= 0;
}

void sound_scene::update_receivers_position	( )
{
	receiver_collision* rc	= m_receivers.front( );
	while ( rc )
	{
		m_spatial_tree->move			
		(
			rc->get_collision_object( ),
			math::create_scale( rc->get_collision_object( )->get_aabb().extents( ) ) *
			math::create_translation( rc->get_position( ) )
		);

		rc					= m_receivers.get_next_of_object( rc );
	}
}

void sound_scene::add_active_voice		( sound_voice& voice )
{
	R_ASSERT							( !m_active_voices.contains_object( &voice ) );
	m_active_voices.push_back			( &voice );
}

void sound_scene::remove_active_voice	( sound_voice& voice )
{
	R_ASSERT							( m_active_voices.contains_object( &voice ) );
	m_active_voices.erase				( &voice );
}

#ifndef MASTER_GOLD

void sound_scene::update_stats			( sound_debug_stats& stats ) const
{
	stats.update_statistic				( );
}


void sound_scene::enable_debug_stream_writing	( )
{
	R_ASSERT			( m_is_debug_stream_writing_enabled == false );
	R_ASSERT			( m_debug_snapshot == 0 );

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	XAPO_REGISTRATION_PROPERTIES props;
	props.MajorVersion					= XRAY_ENGINE_VERSION;
	props.MinorVersion					= XRAY_ENGINE_SUBVERSION;
	props.Flags							= XAPOBASE_DEFAULT_FLAG;
	props.MinInputBufferCount			= 1;
	props.MaxInputBufferCount			= 1;
	props.MinOutputBufferCount			= 1;
	props.MaxOutputBufferCount			= 1;

	m_debug_snapshot					= XRAY_NEW_IMPL( g_allocator, debug_snapshot )( props );
	m_debug_snapshot->AddRef			( );

	XAUDIO2_EFFECT_DESCRIPTOR descriptor;
	descriptor.InitialState				= true;
	descriptor.OutputChannels			= 2;
	descriptor.pEffect					= m_debug_snapshot;

	XAUDIO2_EFFECT_CHAIN chain;
	chain.EffectCount = 1;
	chain.pEffectDescriptors = &descriptor;

	R_ASSERT							( m_submix_voice );
	m_submix_voice->SetEffectChain		( &chain );
#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED						( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	threading::interlocked_exchange		( m_is_debug_stream_writing_enabled, 1 );
}

void sound_scene::disable_debug_stream_writing	( )
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	R_ASSERT							( m_submix_voice );
	m_submix_voice->SetEffectChain		( 0 );

	R_ASSERT							( m_is_debug_stream_writing_enabled );
	R_ASSERT							( m_debug_snapshot != 0 );
	XRAY_DELETE_IMPL					( g_allocator, m_debug_snapshot );

#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED						( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

	m_debug_snapshot					= 0;
	threading::interlocked_exchange		( m_is_debug_stream_writing_enabled, 0 );
}

#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

static void fill_path_name			( fs::path_string& dest, pcstr extention )
{
	SYSTEMTIME					date_time;
	GetLocalTime				( &date_time );
	dest.appendf				(	"%s/sound_dumps/scene_buffers_dump_%02d_%02d_%04d_%02d_%02d_%02d.%s",
									fs_new::portable_path_string::convert( core::user_data_directory() ).c_str(),
									date_time.wDay,
									date_time.wMonth,
									date_time.wYear,
									date_time.wHour, 
									date_time.wMinute,
									date_time.wSecond,
									extention
								 );
}
#endif // #ifdef XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360


void sound_scene::dump_debug_stream_writing	( ) const
{
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	R_ASSERT							( m_is_debug_stream_writing_enabled );
	R_ASSERT							( m_debug_snapshot );
	memory::writer writer				( &debug::g_mt_allocator );
	m_debug_snapshot->dump				( writer );
	fs::path_string path;
	fill_path_name						( path, "raw" );
	writer.save_to						( path.c_str( ) );
	writer.clear						( );

	path.clear							( );
	fill_path_name						( path, "txt" );
	
	configs::lua_config_ptr cfg			= configs::create_lua_config( path.c_str( ) );
	configs::lua_config_value val		= cfg->get_root( )["scene"];

	val["listener"]["position"]			= m_listener.m_position;
	val["listener"]["orient_front"]		= m_listener.m_orient_front;
	val["listener"]["orient_top"]		= m_listener.m_orient_top;

	val["active_proxies_count"]			= m_active_proxies.size( );

	sound_instance_proxy_internal* proxy= m_active_proxies.front( );
	u32 i								= 0;
	while( proxy )
	{
		configs::lua_config_value prx_val	= val["proxies"][i++];
		proxy->dump_debug_snapshot			( prx_val );
		proxy								= m_active_proxies.get_next_of_object( proxy );
	}
	cfg->save							( configs::target_sources );

#else // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	NOT_IMPLEMENTED						( );
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

}

debug_statistic* sound_scene::create_statistic	( ) const
{
	debug_statistic* statistic					= XRAY_NEW_IMPL( g_allocator, debug_statistic )( );
	statistic->m_listener_position				= m_listener.m_position;
	statistic->m_listener_orient_front			= m_listener.m_orient_front;
	statistic->m_listener_orient_top			= m_listener.m_orient_top;
	
	statistic->m_registered_receivers_count		= m_receivers.size		( );
	statistic->m_active_proxies_count			= m_active_proxies.size	( );
	statistic->m_active_voices_count			= m_active_voices.size	( );

	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		proxy_statistic* prx_stats					= XRAY_NEW_IMPL( g_allocator, proxy_statistic )( );
		R_ASSERT									( prx_stats );
		proxy->fill_statistic						( *prx_stats );
		sound_propagator* prop						= proxy->get_propagators( ).front( );
		while ( prop )
		{
			propagator_statistic* prop_stats			= XRAY_NEW_IMPL( g_allocator, propagator_statistic )( );
			R_ASSERT									( prop_stats );
			prop->fill_statistic						( *prop_stats );
			prx_stats->m_propagator_statistics.push_back( prop_stats );
			prop										= proxy->get_propagators( ).get_next_of_object( prop );
		}
		statistic->m_proxies_statistic.push_back	( prx_stats );
		proxy = m_active_proxies.get_next_of_object	( proxy );
	}
	return statistic;
}

void sound_scene::delete_statistic			( debug_statistic* statistic ) const
{
	if ( statistic )
	{
		proxy_statistic* prx_stats			= statistic->m_proxies_statistic.front( );
		while( prx_stats )
		{
			proxy_statistic* prx_stats_next		= statistic->m_proxies_statistic.get_next_of_object( prx_stats );
			propagator_statistic* prop_stats	= prx_stats->m_propagator_statistics.front( );
			while ( prop_stats )
			{
				propagator_statistic* prop_stats_next	= prx_stats->m_propagator_statistics.get_next_of_object( prop_stats );
				prx_stats->m_propagator_statistics.erase( prop_stats );
				XRAY_DELETE_IMPL						( g_allocator, prop_stats );
				prop_stats								= prop_stats_next;
			}
			statistic->m_proxies_statistic.erase( prx_stats );
			XRAY_DELETE_IMPL					( g_allocator, prx_stats );
			prx_stats							= prx_stats_next;
		}
		XRAY_DELETE_IMPL					( g_allocator, statistic );
		statistic							= 0;
	}
}
#endif // #ifndef MASTER_GOLD

void sound_scene::pause							( )
{
	R_ASSERT								( !m_is_paused );
	pause_propagate_all_sounds				( );
	m_is_paused			= true;
}

void sound_scene::resume						( )
{
	R_ASSERT								( m_is_paused );
	resume_propagate_all_sounds				( );
	m_is_paused			= false;
}

bool sound_scene::is_paused						( ) const
{
	return m_is_paused;
}

void sound_scene::pause_propagate_all_sounds	( ) const
{
	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		pause_propagate_sound					( *proxy );
		proxy									= m_active_proxies.get_next_of_object( proxy );
	}
}

void sound_scene::resume_propagate_all_sounds	( ) const
{
	sound_instance_proxy_internal* proxy		= m_active_proxies.front( );
	while ( proxy )
	{
		resume_propagate_sound					( *proxy );
		proxy									= m_active_proxies.get_next_of_object( proxy );
	}
}


} // namespace sound
} // namespace xray

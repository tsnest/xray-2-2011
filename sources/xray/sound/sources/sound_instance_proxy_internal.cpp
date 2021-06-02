////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_instance_proxy_internal.h"
#include <xray/sound/sound_propagator_emitter.h>
#include <xray/sound/single_sound.h>
#include <xray/collision/space_partitioning_tree.h>
#include <xray/sound/sound_receiver.h>
#include <xray/sound/world_user.h>
#include "sound_world.h"
#include "sound_instance_proxy_order.h"
#include "functor_command.h"
#include <xray/collision/geometry_instance.h>
//#include <xray/render/facade/game_renderer.h>
//#include <xray/render/facade/debug_renderer.h>

namespace xray {
namespace sound {

sound_instance_proxy_internal::sound_instance_proxy_internal(	sound_scene& scene,
																sound_emitter_ptr sound_emitter,
																sound_propagator_emitter const& propagator_emitter,
																world_user& user ) :
	m_scene					( scene ),
	m_user					( user ),
	m_sound_emitter			( sound_emitter ),
	m_propagator_emitter	( propagator_emitter ),
	m_next_for_sound_world	( 0 ),
	m_destruction_pending	( false ),
	m_volumetric_radius		( 0.0f ),
	m_type					( point ),
	m_collision				( 0 )
{
}

sound_instance_proxy_internal::sound_instance_proxy_internal	( 
										sound_scene& scene,
										sound_emitter_ptr sound_emitter,
										sound_propagator_emitter const& propagator_emitter,
										world_user& user,
										sound_cone_type cone_type
									) :
	m_scene					( scene ),
	m_user					( user ),
	m_sound_emitter			( sound_emitter ),
	m_propagator_emitter	( propagator_emitter ),
	m_next_for_sound_world	( 0 ),
	m_destruction_pending	( false ),
	m_volumetric_radius		( 0.0f ),
	m_type					( cone ),
	m_cone_type				( cone_type ),
	m_collision				( 0 )
{}

sound_instance_proxy_internal::sound_instance_proxy_internal	( 
										sound_scene& scene,
										sound_emitter_ptr sound_emitter,
										sound_propagator_emitter const& propagator_emitter,
										world_user& user,
										collision::geometry_instance& geometry,
										float radius
									) :
	m_scene					( scene ),
	m_user					( user ),
	m_sound_emitter			( sound_emitter ),
	m_propagator_emitter	( propagator_emitter ),
	m_next_for_sound_world	( 0 ),
	m_destruction_pending	( false ),
	m_type					( volumetric ),
	m_collision				( &geometry ),
	m_volumetric_radius		( radius )
{}

sound_instance_proxy_internal::~sound_instance_proxy_internal	( )
{
	LOG_DEBUG		( "~sound_instance_proxy_internal" );
}

void sound_instance_proxy_internal::play	(	playback_mode mode,
												sound_producer const* const producer,
												sound_receiver const* const ignorable_receiver
											)
{
	if ( producer )
		producer->on_play	( m_user );

	m_scene.emit_sound_propagators	( *this, mode, producer, ignorable_receiver );

	m_is_playing			= true;
	m_is_propagating_paused	= false;
	m_is_producing_paused	= false;
 
}

void sound_instance_proxy_internal::play_once	(	float3 const& position,
													sound_producer const* const producer,
													sound_receiver const* const ignorable_receiver
												)
{
	m_callback						= boost::bind( &sound_instance_proxy_internal::on_finish_callback, this, sound_instance_proxy_ptr( this ) );
	threading::interlocked_exchange	( m_is_playing_once, true );

	m_position.set					( position );
	play							( once, producer, ignorable_receiver );
}

void sound_instance_proxy_internal::pause	( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::pause_produce_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );

	m_is_producing_paused	= true;

}

void sound_instance_proxy_internal::resume	( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::resume_produce_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
	m_is_producing_paused					= false;
}

void sound_instance_proxy_internal::pause_propagate_sound( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::pause_propagate_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
	m_is_propagating_paused					= true;
}

void sound_instance_proxy_internal::resume_propagate_sound( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::resume_propagate_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );

	m_is_propagating_paused					= false;
}

void sound_instance_proxy_internal::stop	( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::stop_produce_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );

	m_is_playing							= false;
	m_is_producing_paused					= false;
	m_is_propagating_paused					= false;
}

void sound_instance_proxy_internal::stop_propagate_sound	( )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor					= boost::bind	(	&sound_scene::stop_propagate_sound,
																&m_scene, boost::ref( *this ) );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );

	m_is_playing							= false;
	m_is_producing_paused					= false;
	m_is_propagating_paused					= false;
}

//void sound_instance_proxy_internal::set_position	( float3 const& position )
//{
//	set_quality_for_resource( position );
//
//	typedef sound_instance_proxy_order::functor_type functor_type;
//	functor_type functor		= boost::bind	(	&sound_instance_proxy_internal::set_sound_propagator_position,
//													this,
//													position );
//
//	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
//											( m_user, *this, functor );
//
//	m_user.add_order						( order );
//}

void sound_instance_proxy_internal::set_position				( float3 const& position )
{
	R_ASSERT					( m_type == point );
	set_quality_for_resource	( position );
	m_position.set				( position );
}

void sound_instance_proxy_internal::set_position_and_direction	( float3 const& position, float3 const& direction )
{
	R_ASSERT					( m_type == cone );
	set_quality_for_resource	( position );
	m_position.set				( position );
	m_direction.set				( direction );
}

void sound_instance_proxy_internal::set_matrix					( float4x4 const& matrix )
{
	R_ASSERT					( m_type == volumetric );
	set_quality_for_resource	( matrix.c.xyz() );
	m_collision->set_matrix		( matrix );
}

void sound_instance_proxy_internal::free_object	( )
{
	threading::interlocked_exchange		( m_destruction_pending, true );
	m_callback.clear					( );

	LOG_DEBUG							( "sound_instance_proxy_internal::free_object" );
	destroy_sound_instance_proxy_order* order = 
		XRAY_NEW_IMPL ( m_user.get_allocator( ), destroy_sound_instance_proxy_order )
		( m_user, *this, &m_scene );

	m_user.add_order ( order );

}

void sound_instance_proxy_internal::tick	( u32 delta_time )
{
//	LOG_DEBUG					("sound_instance_proxy::tick");
	sound_propagator* propagator	= m_propagators.front( );
	while ( propagator )
	{
		sound_propagator* next	= m_propagators.get_next_of_object( propagator );

//		LOG_DEBUG				( "sound_instance_proxy_internal::tick sound_propagator address: 0x%8x", propagator );
		propagator->tick		( delta_time );

		if ( propagator->can_be_deleted( ) )
			m_scene.delete_sound_propagator( *this, propagator ); 
	
		propagator				= next;
	}
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

void sound_instance_proxy_internal::notify_receivers	( collision::space_partitioning_tree& spatial_tree )
{
	sound_propagator* prop			= m_propagators.front( );
	while ( prop )
	{
		sound_producer const* const producer			= prop->get_producer( );

		if ( prop->is_propagation_finished( ) || prop->is_propagation_paused( ) || producer == 0 )
		{
			prop			= m_propagators.get_next_of_object( prop );
			continue;
		}

		float outer_radius			= prop->get_propagation_outer_radius( );

		collision::objects_type query_result	( g_allocator );
		float3 position							= get_position( );
		get_receivers_in_radius					( spatial_tree, position, outer_radius, query_result );

		float inner_radius			( prop->get_propagation_inner_radius( ) );

		//LOG_DEBUG					( "inner_radius %f", inner_radius );
		if ( !math::is_zero( inner_radius ) )
		{

			collision::objects_type out_query_result	( g_allocator );
			get_receivers_in_radius	( spatial_tree, position, inner_radius, out_query_result );

			query_result.erase
			(
				std::remove_if
				(
					query_result.begin( ),
					query_result.end( ),
					erase_unaudible_predicate( &out_query_result )
				),
				query_result.end( )
			);
		}

		collision::objects_type::iterator query_it			= query_result.begin( );
		collision::objects_type::const_iterator query_end	= query_result.end( );

		for( ; query_it != query_end; ++query_it )
		{
			sound::receiver_collision* receiver			= static_cast_checked<sound::receiver_collision*>( (*query_it)->user_data( ) );
			sound_receiver const* ignorable_receiver	= prop->get_ignorable_receiver( );

			if ( ignorable_receiver != receiver->get_sound_receiver( ) ) 
			{
				notify_receiver_command* cmd	= XRAY_NEW_IMPL( g_allocator, notify_receiver_command )
				(
					boost::bind ( &sound_receiver::on_sound_event, receiver->get_sound_receiver( ), boost::ref( *producer )),
					m_user,
					(u64)producer,
					(u64)receiver
				);

				m_user.add_response( cmd );
			}
		}

		prop			= m_propagators.get_next_of_object( prop );
	}
}

void sound_instance_proxy_internal::get_receivers_in_radius			
									( 
										collision::space_partitioning_tree& spatial_tree,
										float3 const& position,
										float radius,
										collision::objects_type& query_result
									) const
{
	math::aabb aabb							( create_aabb_center_radius( position, float3 ( radius, radius, radius ) ) );
	spatial_tree.aabb_query					( 1, aabb, query_result );
}

void sound_instance_proxy_internal::set_quality_for_resource( float3 const& position )
{
	current_matrix_for_write( ) = create_translation( position );
	m_propagator_emitter.get_quality_for_resource( ); 
}

void sound_instance_proxy_internal::execute_callback	( )
{
	set_callback_pending		( false );
	m_is_playing				= false;

	if ( m_callback )
		m_callback				( );
}

void sound_instance_proxy_internal::on_finish_callback	( sound_instance_proxy_ptr last_reference )
{
	LOG_DEBUG						( "on_finish_callback" );
	// we must be sure that all m_callback binded parameters are destroyed and last_reference
	// parameter is really the last
	m_callback.clear				( );
}

void sound_instance_proxy_internal::serialize	( boost::function < void ( memory::writer*, memory::writer* ) >& fn, memory::writer* writer )
{
	writer->write_u64			( (u64)(&m_propagator_emitter) );
	bool is_callback_empty		= m_callback.empty( );
	writer->write				( &is_callback_empty, sizeof( bool ) );
	u32 is_callback_pending		= m_callback_pending;
	writer->write_u32			( is_callback_pending );
	u32 is_playing_once			= m_is_playing_once;
	writer->write_u32			( is_playing_once );
	writer->write				( &m_is_playing, sizeof( bool ) );
	writer->write				( &m_is_producing_paused, sizeof( bool ) );
	writer->write				( &m_is_propagating_paused, sizeof( bool ) );

	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor		= boost::bind	(	&sound_world::serialize_proxy,
													m_user.get_sound_world( ),
													this, fn, writer );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
}

void sound_instance_proxy_internal::deserialize	( 
													boost::function < void ( void ) >& fn,
													memory::reader* r,
													callback_type const& callback
												)
{

	XRAY_UNREFERENCED_PARAMETER ( fn );
	bool is_callback_was_set	= false;
	r->r						( &is_callback_was_set, sizeof( bool ), sizeof ( bool ) );
	if ( is_callback_was_set )
		m_callback				= callback;

	u32 is_callback_pending		= r->r_u32( );
	u32 is_playing_once			= r->r_u32( );

	r->r						( &m_is_playing, sizeof( bool ), sizeof( bool ) );
	r->r						( &m_is_producing_paused, sizeof( bool ), sizeof( bool ) );
	r->r						( &m_is_propagating_paused, sizeof( bool ), sizeof( bool ) );
	
	threading::interlocked_exchange( m_callback_pending, is_callback_pending );
	threading::interlocked_exchange( m_is_playing_once, is_playing_once );

	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor		= boost::bind	(	&sound_world::deserialize_proxy,
													m_user.get_sound_world( ),
													this, r );
	
	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
}

//void sound_instance_proxy_internal::dbg_draw	( render::game::renderer& render, render::scene_ptr const& scene ) const
//{
//	sound_propagator* prop			= m_propagators.front( );
//	while ( prop )
//	{
//		math::float4x4 mtx			= math::create_translation( prop->get_position( ) );
//		float out_r					= prop->get_propagation_outer_radius( );
//		float in_r					= prop->get_propagation_inner_radius( );
//		render.debug().draw_circle( scene, mtx, float3( out_r, out_r, out_r ), math::color( 255, 0, 0 ) ); 
//		render.debug().draw_circle( scene, mtx, float3( in_r, in_r, in_r ), math::color( 0, 0, 255 ) ); 
//		prop						= m_propagators.get_next_of_object( prop );
//	}
//}

void sound_instance_proxy_internal::on_propagators_serialized(	boost::function < void ( memory::writer*, memory::writer* ) >& fn,
																				memory::writer* sound_thread_writer,
																				memory::writer* w )
{
	fn							( sound_thread_writer, w );
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor		= boost::bind	(	&sound_world::on_proxy_serialized,
													m_user.get_sound_world( ),
													sound_thread_writer );
	
	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
}

#ifndef MASTER_GOLD
void sound_instance_proxy_internal::dbg_set_quality	( u32 quality )
{
	typedef sound_instance_proxy_order::functor_type functor_type;
	functor_type functor		= boost::bind	(	&sound_instance_proxy_internal::dbg_set_quality_impl,
													this,
													quality );

	sound_instance_proxy_order* const order	= XRAY_NEW_IMPL	( m_user.get_allocator( ), sound_instance_proxy_order )
											( m_user, *this, functor );

	m_user.add_order						( order );
}

void sound_instance_proxy_internal::dbg_set_quality_impl	( u32 quality )
{
	sound_propagator* prop			= m_propagators.front( );
	while ( prop )
	{
		prop->set_quality	( quality );
		prop				= m_propagators.get_next_of_object( prop );
	}
}
#endif //#ifndef MASTER_GOLD

float3 sound_instance_proxy_internal::get_volumetric_position	( float3 const& listener_position ) const
{
	R_ASSERT									( m_type == volumetric );
	return m_collision->get_closest_point_to	( listener_position );
}

#ifndef MASTER_GOLD
void sound_instance_proxy_internal::dump_debug_snapshot	( configs::lua_config_value& val ) const
{
	val["proxy_id"]		= get_id( );
	switch ( get_sound_type( ) )
	{
	case point:
		{
			val["sound_type"]					= "point";
			val["position"]						= m_position.get( );
		} break;
	case cone:
		{
			val["sound_type"]					= "cone";
			val["position"]						= m_position.get( );
			val["direction"]					= m_direction.get( );
			val["cone_type"]					= m_cone_type;
		} break;
	case volumetric:
		{
			val["sound_type"]					= "volumetric";
			val["nearest_point_to_listener"]	= get_volumetric_position( m_scene.get_listenet_position() );
		} break;
	default: NODEFAULT();
	};

	switch ( get_sound_emitter()->get_type() )
	{
	case single:		val["emitter_type"] = "single";		break;
	case composite:		val["emitter_type"] = "composite";	break;
	case collection:	val["emitter_type"] = "collection";	break;
	default: NODEFAULT();
	};

	sound_propagator* prop			= m_propagators.front( );
	u32 i							= 0;
	while ( prop )
	{
		configs::lua_config_value prop_val	= val["propagators"][i++];
		prop->dump_debug_snapshot			( prop_val );
		prop								= m_propagators.get_next_of_object( prop );
	}
}

void sound_instance_proxy_internal::fill_statistic	( proxy_statistic& statistic ) const
{
	statistic.m_id						= get_id( );
	switch ( get_sound_type( ) )
	{
	case point:
		{
			statistic.m_sound_type				= point;
			statistic.m_position				= m_position.get( );
		} break;
	case cone:
		{
			statistic.m_sound_type				= cone;
			statistic.m_cone_type				= m_cone_type;
			statistic.m_position				= m_position.get( );
			statistic.m_direction				= m_direction.get( );
		} break;
	case volumetric:
		{
			statistic.m_sound_type				= volumetric;
			statistic.m_position				= get_volumetric_position( m_scene.get_listenet_position() );
		} break;
	default: NODEFAULT();
	};

	statistic.m_emitter_type					= get_sound_emitter()->get_type();
}


#endif // #ifndef MASTER_GOLD

} // namespace sound
} // namespace xray
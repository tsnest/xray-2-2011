////////////////////////////////////////////////////////////////////////////
//	Created		: 05.09.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/sound/world_user.h>
#include "sound_scene.h"
#include "sound_world.h"

namespace xray {
namespace sound {

world_user::world_user	( sound_world* owner, base_allocator_type* allocator ) : 
	m_owner_world		( owner ),
	m_allocator			( allocator ),
	m_channel			( *g_allocator, *allocator ),
	m_deleted_producers	( 0 ),
	m_deleted_receivers	( 0 ),
	m_is_paused			( false )
{

	m_channel.responses.owner_initialize( NEW( sound_response )(), NEW( sound_response )() );
	m_channel.orders.user_initialize	( );
}

world_user::~world_user	( )
{
	m_channel.responses.owner_finalize	( );
}

void world_user::initialize				( )
{
	if ( m_allocator )
	{
		m_deleted_producers			= XRAY_NEW_IMPL( m_allocator, vectora<u64> ) ( m_allocator );
		m_deleted_receivers			= XRAY_NEW_IMPL( m_allocator, vectora<u64> ) ( m_allocator );
	}

	m_channel.orders.owner_initialize	(
		XRAY_NEW_IMPL( m_allocator, sound_order ) ( ),
		XRAY_NEW_IMPL( m_allocator, sound_order ) ( )
	);
	m_channel.responses.user_initialize	( );
}

void world_user::finalize				( )
{
	if ( m_deleted_producers )
		XRAY_DELETE_IMPL				( m_allocator, m_deleted_producers );

	if ( m_deleted_receivers )
		XRAY_DELETE_IMPL				( m_allocator, m_deleted_receivers );

	m_channel.orders.owner_finalize		( );
}

void world_user::dispatch_callbacks		( )
{
	process_responses					( );
}

void world_user::add_order			( sound_order* order )
{
	m_channel.orders.owner_push_back( order );
}

void world_user::add_response		( sound_response* response )
{
	m_channel.responses.owner_push_back( response );
}

void world_user::mute_all_sounds	( bool mute )
{
	XRAY_UNREFERENCED_PARAMETER		( mute );
}

void world_user::register_receiver	( sound_scene_ptr& scene, sound_receiver& receiver )
{
	sound_scene* scn					= static_cast_checked<sound_scene*>	( scene.c_ptr() );
	atomic_half3* pos					= scn->create_receiver_position	( );
	receiver.on_register_receiver_order_created	( pos );

	functor_order* const order	= XRAY_NEW_IMPL( m_allocator, functor_order )
	(
		boost::bind
		( 
			&sound_scene::register_receiver,
			scn,
			&receiver,
			pos
		)
	);

	LOG_INFO						( "register_receiver: 0x%8x", order );
	add_order						( order );
}

void world_user::unregister_receiver( sound_scene_ptr& scene, sound_receiver& receiver )
{
	sound_scene& scn		= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	
	if ( !m_owner_world->is_destroying( ) )
		mark_receiver_as_deleted( (u64)&receiver );

	functor_order* const order =
		XRAY_NEW_IMPL( m_allocator, functor_order )	( 
		boost::bind( &sound_scene::unregister_receiver, &scn, boost::ref( *this ), &receiver )
	);

	LOG_INFO						( "unregister_receiver: 0x%8x", order );
	add_order						( order );
}


//void world_user::set_listener_properties( sound_scene_ptr& scene, float4x4 const& inv_view_matrix )
//{
//	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );
//
//	listener_properties_order* order	= XRAY_NEW_IMPL( m_allocator, listener_properties_order )
//										( *m_owner_world, scn, inv_view_matrix );
//
//	add_order							( order );
//}

void world_user::set_listener_properties_interlocked(	sound_scene_ptr& scene, 
														float3 const& position,
														float3 const& orient_front,
														float3 const& orient_top
													)
{
	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	scn.set_listener_properties			( position, orient_front, orient_top );
}

void world_user::set_active_sound_scene	(	sound_scene_ptr& scene,
											u32 fade_in_time,
											u32 fade_out_old_scene_time  )
{
	R_ASSERT					( scene );
	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );

	functor_order* order				= XRAY_NEW_IMPL( m_allocator, functor_order )
	(	
		boost::bind( &sound_world::set_active_sound_scene_impl, m_owner_world,  boost::ref( scn ), fade_in_time, fade_out_old_scene_time )
	);

	add_order							( order );
}

void world_user::remove_sound_scene	( sound_scene_ptr& scene )
{
	functor_order* order				= XRAY_NEW_IMPL( m_allocator, functor_order )
	(	
		boost::bind( &sound_world::remove_sound_scene_impl, m_owner_world,  scene )
	);

	add_order							( order );
}

void world_user::process_orders		( )
{
	m_channel.responses.owner_delete_processed_items	( );
	while ( sound_order* const order = m_channel.orders.user_pop_front( ) )
		order->execute( );
}

void world_user::process_responses	( )
{
	m_channel.orders.owner_delete_processed_items	( );
	while ( sound_response* const response = m_channel.responses.user_pop_front() )
		response->execute			( );
}

two_way_threads_channel& world_user::get_channel( )
{
	return m_channel;
}

base_allocator_type* world_user::get_allocator( ) const
{
	return m_allocator;
}

void world_user::mark_producer_as_deleted		( u64 producer_address )
{
	R_ASSERT	( !is_producer_deleted( producer_address ) );
	m_deleted_producers->push_back	( producer_address );
}

bool world_user::is_producer_deleted			( u64 address ) const
{
	return std::find( m_deleted_producers->begin( ), m_deleted_producers->end( ), address ) != m_deleted_producers->end( );
}

void	world_user::on_producer_deleted		( u64 address )
{
	LOG_DEBUG						( "world_user::on_sound_producer_deleted" );

	functor_response* resp			= XRAY_NEW_IMPL( g_allocator, functor_response )
	( 
		boost::bind( &world_user::unmark_producer_as_deleted, this, address )
	);

	add_response( resp );
}

void	world_user::unmark_producer_as_deleted	( u64 address )
{
	vectora<u64>::iterator elem	= std::find( m_deleted_producers->begin( ), m_deleted_producers->end( ), address );
	R_ASSERT	( elem != m_deleted_producers->end( ) );
	m_deleted_producers->erase	( elem );
}

void world_user::mark_receiver_as_deleted	( u64 receiver_address )
{
	R_ASSERT	( !is_receiver_deleted( receiver_address ) );
	m_deleted_receivers->push_back		( receiver_address );
}

void world_user::unmark_receiver_as_deleted	( u64 receiver_address )
{
	vectora<u64>::iterator elem	= std::find( m_deleted_receivers->begin( ), m_deleted_receivers->end( ), receiver_address );
	R_ASSERT	( elem != m_deleted_receivers->end( ) );
	m_deleted_receivers->erase	( elem );
}

bool world_user::is_receiver_deleted	( u64 receiver_address ) const
{
	return std::find( m_deleted_receivers->begin( ), m_deleted_receivers->end( ), receiver_address ) != m_deleted_receivers->end( );
}

void world_user::on_receiver_deleted	( u64 receiver_address )
{
	if ( m_owner_world->is_destroying( ) )
		return;

	functor_response* resp			= XRAY_NEW_IMPL( g_allocator, functor_response )
	( 
		boost::bind( &world_user::unmark_receiver_as_deleted, this, receiver_address )
	);

	add_response( resp );
}


#ifndef MASTER_GOLD

void world_user::enable_debug_stream_writing	( sound_scene_ptr& scene )
{
	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	functor_order* order				= XRAY_NEW_IMPL( m_allocator, functor_order )
	(	
		boost::bind( &sound_scene::enable_debug_stream_writing, &scn )
	);
	add_order							( order );
}

void world_user::disable_debug_stream_writing	( sound_scene_ptr& scene )
{
	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	functor_order* order				= XRAY_NEW_IMPL( m_allocator, functor_order )
	(	
		boost::bind( &sound_scene::disable_debug_stream_writing, &scn )
	);
	add_order							( order );
}

bool world_user::is_debug_stream_writing_enabled	( sound_scene_ptr& scene ) const
{
	sound_scene const& scn			= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	return scn.is_debug_stream_writing_enabled		( );
}

void world_user::dump_debug_stream_writing		( sound_scene_ptr& scene )
{
	sound_scene& scn					= static_cast_checked<sound_scene&>( *scene.c_ptr() );
	functor_order* order				= XRAY_NEW_IMPL( m_allocator, functor_order )
	(	
		boost::bind( &sound_scene::dump_debug_stream_writing, &scn )
	);
	add_order							( order );
}

#endif // #ifndef MASTER_GOLD

void world_user::set_time_scale_factor				( float factor )
{
	R_ASSERT										( m_owner_world );

	sound_order* const order				= XRAY_NEW_IMPL
		( m_allocator, functor_order )
		( boost::bind( &sound_world::set_time_scale_factor, m_owner_world, factor ) );

	add_order								( order );
}

float world_user::get_time_scale_factor				( ) const
{
	return m_owner_world->get_time_scale_factor		( );
}

#ifndef MASTER_GOLD
void world_user::enable_current_scene_stream_writing	( )
{
	R_ASSERT								( m_owner_world );

	sound_order* const order				= XRAY_NEW_IMPL
		( m_allocator, functor_order )
		( boost::bind( &sound_world::enable_current_scene_stream_writing, m_owner_world ) );

	add_order								( order );
}

void world_user::disable_current_scene_stream_writing	( )
{
	R_ASSERT								( m_owner_world );

	sound_order* const order				= XRAY_NEW_IMPL
		( m_allocator, functor_order )
		( boost::bind( &sound_world::disable_current_scene_stream_writing, m_owner_world ) );

	add_order								( order );
}

void world_user::dump_current_scene_stream_writing		( )
{
	R_ASSERT								( m_owner_world );

	sound_order* const order				= XRAY_NEW_IMPL
		( m_allocator, functor_order )
		( boost::bind( &sound_world::dump_current_scene_stream_writing, m_owner_world ) );

	add_order								( order );
}

bool world_user::is_current_scene_stream_writing_enabled	( ) const
{
	return m_owner_world->is_current_scene_stream_writing_enabled( );
}
#endif //#ifndef MASTER_GOLD

} // namespace sound
} // namespace xray
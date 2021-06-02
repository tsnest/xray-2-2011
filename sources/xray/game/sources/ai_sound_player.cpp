////////////////////////////////////////////////////////////////////////////
//	Created		: 21.02.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "ai_sound_player.h"
#include <xray/sound/sound_emitter.h>
#include <xray/sound/sound_instance_proxy.h>
#include <xray/sound/world_user.h>

#pragma message( XRAY_TODO("dimetcm 2 dimetcm: this includes only needed to test, remove this constructor after test") )
#include <xray/input/world.h>
#include <xray/input/keyboard.h>
#include "human_npc.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/game_renderer.h>


namespace stalker2 {

ai_sound_player::sounds_collection_type::sounds_collection_type	(	ai_sound_player* parent,
																	ai::sound_collection_types collection_type,
																	sound::sound_emitter_ptr emitter_ptr,
																	u32 collection_priority ) :
		type				( collection_type ),
		priority			( collection_priority )
{
	emitter.initialize_and_set_parent( parent, emitter_ptr.c_ptr() );
}

ai_sound_player::ai_sound_player	(	sound::sound_scene_ptr& scene,
										u32 sounds_count,
										sound::world_user& user,
										sound::sound_producer const* const producer,
										sound::sound_receiver const* const ignorable_receiver
									) :
	m_scene							( scene ),
	m_user							( user ),
	m_sounds_count					( sounds_count ),
	m_sound_producer				( producer ),
	m_ignorable_receiver			( ignorable_receiver ),
	m_dbg_input_world				( 0 )
{	
}

static u32 random_seed				= 1;

ai_sound_player::ai_sound_player	(	sound::sound_scene_ptr& scene,
										u32 sounds_count,
										sound::world_user& user,
										input::world* input_world,
										sound::sound_producer const* const producer,
										sound::sound_receiver const* const ignorable_receiver
									) :
	m_scene							( scene ),
	m_user							( user ),
	m_sounds_count					( sounds_count ),
	m_sound_producer				( producer ),
	m_ignorable_receiver			( ignorable_receiver ),
	m_dbg_input_world				( input_world ),
	m_dbg_last_current_time_in_ms	( 0 ),
	m_dbg_time_from_last_playing	( 0 ),
	m_dbg_random					( random_seed++ )
{
	m_dbg_timer.start				( );
	u32 val							= m_dbg_random.random( 20000 ) + 5000;
	m_dbg_time_to_play_new_sound	= val;

	xray::const_buffer temp_buffer				( "", 1 );

	query_create_resource	
	( 
		"dbg_sound_scene",
		temp_buffer, 
		resources::sound_scene_class,
		boost::bind( &ai_sound_player::on_dbg_sound_scene_created, this, _1 ),
		g_allocator 
	);
}

void ai_sound_player::on_dbg_sound_scene_created	( resources::queries_result& data )
{
	R_ASSERT			( data.is_successful( ) );
	m_dbg_scene			= static_cast_resource_ptr< xray::sound::sound_scene_ptr >( data[0].get_unmanaged_resource() );
}

ai_sound_player::~ai_sound_player	( )
{
	LOG_DEBUG						( "~ai_sound_player" );

	m_dbg_scene						= 0;
	m_active_sound					= 0;
	m_dbg_active_sound				= 0;
//	m_scene							= 0;


	sounds_collection_type const* const begin	= sounds( );
	sounds_collection_type const* const end		= begin + m_sounds_count;
	for ( sounds_collection_type const* i = begin; i != end; ++i )
	{
		i->~sounds_collection_type	( );
	}

}

ai_sound_player::sounds_collection_type const* ai_sound_player::find (
		ai::sound_collection_types const sound_type
	) const
{
	sounds_collection_type const* const begin	= sounds( );
	sounds_collection_type const* const end		= begin + m_sounds_count;
	for ( sounds_collection_type const* i = begin; i != end; ++i )
	{
		if ( i->type == sound_type )
			return i;
	}
	return 0;
}

void ai_sound_player::play			(
		ai::sound_collection_types sound_type,
		bool sound_is_positioned,
		float3 const& position
	)
{
	sounds_collection_type const* type	= find( sound_type );
	R_ASSERT						( type, "such a type is absent in sound collections" );

	m_active_sound					= type->emitter->emit( m_scene, m_user );
	m_active_sound->set_callback	( boost::bind( &ai_sound_player::on_finish_playing, this ) );

//	if ( sound_is_positioned )
//		m_active_sound->play		( position, sound::once, m_sound_producer, m_ignorable_receiver );
//	else
//		m_active_sound->play		( sound::once, m_sound_producer, m_ignorable_receiver );
}

void ai_sound_player::play			(
		resources::unmanaged_resource_ptr sound_to_be_played,
		ai::finish_playing_callback const& finish_callback,
		float3 const& position
	)
{
	sound::sound_emitter_ptr sound	= static_cast_resource_ptr< sound::sound_emitter_ptr >( sound_to_be_played );
	m_active_sound					= sound->emit( m_scene, m_user );
	m_active_sound->set_callback	( finish_callback );
	m_active_sound->set_position	( position );
	m_active_sound->play			( sound::once, m_sound_producer, m_ignorable_receiver );
}

void ai_sound_player::play_once		(
		ai::sound_collection_types sound_type,
		bool sound_is_positioned,
		float3 const& position
	)
{
	XRAY_UNREFERENCED_PARAMETER				( sound_is_positioned );
	sounds_collection_type const* type		= find( sound_type );
	R_ASSERT								( type, "such a type is absent in sound collections" );

	sound::sound_instance_proxy_ptr proxy	= type->emitter->emit( m_scene, m_user );
	type->emitter->emit_and_play_once( m_scene, m_user, position, m_sound_producer, m_ignorable_receiver );
}

void ai_sound_player::tick			( )
{
	u32 const current_time_in_ms	= m_dbg_timer.get_elapsed_msec( );
	R_ASSERT_CMP					( current_time_in_ms, >=, m_dbg_last_current_time_in_ms );
	//u32 const time_delta			= current_time_in_ms - m_dbg_last_current_time_in_ms;

	human_npc const* npc			= static_cast_checked<human_npc const*>( m_sound_producer );
	if ( !npc->get_sound_dbg_mode( ) )
		return;

	if ( m_active_sound )
	{
		//m_active_sound->dbg_draw			( npc->get_dbg_render( ), npc->get_dbg_scene( ) );
		//if ( m_active_sound->is_playing( ) )
		//{
		//	math::float4x4 mtx	= math::create_translation( npc->get_position( float3( .0f, .0f, .0f )));
		//	npc->get_dbg_render().debug().draw_line_ellipsoid( npc->get_dbg_scene( ), mtx,/* float3( 1.f, 1.f, 1.f ),*/ math::color( 0, 255, 0 ) );
		//}
	}
	//npc->get_dbg_render( ).debug().

	//m_dbg_time_from_last_playing	+= time_delta;
	//if ( m_dbg_time_from_last_playing > m_dbg_time_to_play_new_sound )
	//{
	//	sounds_collection_type const* type		= find( ai::sound_collection_type_npc_pain );
	//	R_ASSERT								( type, "such a type is absent in sound collections" );

	//	m_active_sound							= type->emitter->emit( m_user );
	//	m_active_sound->play					( npc->get_position( float3( 0.0f, 0.0f, 0.0f )), sound::once, m_sound_producer, m_ignorable_receiver );

	//	u32 val							= m_dbg_random.random( 20000 ) + 5000;
	//	m_dbg_time_to_play_new_sound	= val;
	//	LOG_DEBUG						( "m_dbg_time_to_play_new_sound: %d", m_dbg_time_to_play_new_sound );
	//	m_dbg_time_from_last_playing	= 0;
	//}

#ifndef MASTER_GOLD
	process_input						( );
#endif // #ifndef MASTER_GOLD

	m_dbg_last_current_time_in_ms		= current_time_in_ms;
}

#ifndef MASTER_GOLD
void ai_sound_player::process_input	( )
{
	if ( m_dbg_input_world == 0 )
		return;

	human_npc* npc						= (human_npc*)m_sound_producer;
	if ( !strings::equal( "aggressive_npc", npc->get_description() ) )
		return;

	non_null<input::keyboard const>::ptr const k	= m_dbg_input_world->get_keyboard();

	static bool key_i_pressed					= false;
	if ( k->is_key_down( input::key_i ) && !key_i_pressed )
	{
		sounds_collection_type const* type		= find( ai::sound_collection_type_npc_pain );
		R_ASSERT								( type, "such a type is absent in sound collections" );
		
		m_active_sound							= type->emitter->emit( m_scene, m_user );
		m_active_sound->set_callback			( boost::bind( &ai_sound_player::on_finish_playing, this ) );
		m_active_sound->set_position			( npc->get_position( float3( 0.0f, 0.0f, 0.0f )));
		m_active_sound->play					( sound::once, m_sound_producer, m_ignorable_receiver );

		key_i_pressed							= true;
	}
	else
		key_i_pressed							= false;

	static bool key_o_pressed					= false;
	if ( k->is_key_down( input::key_o ) && !key_o_pressed )
	{
		sounds_collection_type const* type		= find( ai::sound_collection_type_npc_pain );
		R_ASSERT								( type, "such a type is absent in sound collections" );

		m_active_sound							= type->emitter->emit( m_scene, m_user );
		m_active_sound->set_position			( npc->get_position( float3( 0.0f, 0.0f, 0.0f )));
		m_active_sound->play					( sound::looped, m_sound_producer, m_ignorable_receiver );

		key_o_pressed							= true;
	}
	else
		key_o_pressed							= false;

	static bool key_p_pressed					= false;
	if ( k->is_key_down( input::key_p ) && !key_p_pressed )
	{
		sounds_collection_type const* type		= find( ai::sound_collection_type_npc_pain );
		R_ASSERT								( type, "such a type is absent in sound collections" );

		type->emitter->emit_and_play_once		
		(	
			m_scene,
			m_user,
			npc->get_position( float3( 0.0f, 0.0f, 0.0f )),
			m_sound_producer,
			m_ignorable_receiver
		);

		key_p_pressed							= true;
	}
	else
		key_p_pressed							= false;

	static bool key_j_pressed					= false;
	if ( k->is_key_down( input::key_j ) && !key_j_pressed )
	{
		if ( !m_active_sound->is_producing_paused( ) &&
			 !m_active_sound->is_propagating_paused( ))
			m_active_sound->pause				( );

		key_j_pressed							= true;
	}
	else
		key_j_pressed							= false;

	static bool key_k_pressed					= false;
	if ( k->is_key_down( input::key_k ) && !key_k_pressed )
	{
		if ( m_active_sound->is_producing_paused( ) &&
			 !m_active_sound->is_propagating_paused( ))
		m_active_sound->resume			( );

		key_k_pressed							= true;
	}
	else
		key_k_pressed							= false;

	static bool key_y_pressed					= false;
	if ( k->is_key_down( input::key_y ) && !key_y_pressed )
	{
		m_active_sound->stop		( );

		key_y_pressed							= true;
	}
	else
		key_y_pressed							= false;

	//static bool key_s_pressed					= false;
	//if ( k->is_key_down( input::key_s ) && !key_s_pressed )
	//{
	//	serialize								( );
	//	key_s_pressed							= true;
	//}
	//else
	//	key_s_pressed							= false;

	//static bool key_d_pressed					= false;
	//if ( k->is_key_down( input::key_d ) && !key_d_pressed )
	//{
	//	deserialize								( );
	//	key_d_pressed							= true;
	//}
	//else
	//	key_d_pressed							= false;

	static bool key_1_pressed					= false;
	if ( k->is_key_down( input::key_1 ) && !key_1_pressed )
	{
		m_active_sound->dbg_set_quality			( 0 );
		key_1_pressed							= true;
	}
	else
		key_1_pressed							= false;

	static bool key_2_pressed					= false;
	if ( k->is_key_down( input::key_2 ) && !key_2_pressed )
	{
		m_active_sound->dbg_set_quality			( 1 );
		key_2_pressed							= true;
	}
	else
		key_2_pressed							= false;

	static bool key_3_pressed					= false;
	if ( k->is_key_down( input::key_3 ) && !key_3_pressed )
	{
		m_active_sound->dbg_set_quality			( 2 );
		key_3_pressed							= true;
	}
	else
		key_3_pressed							= false;

	static bool key_v_pressed					= false;
	if ( k->is_key_down( input::key_v ) && !key_v_pressed )
	{
		float4x4 mtx;
		mtx.identity( );


		m_user.set_listener_properties_interlocked( 
			m_dbg_scene,
			float3( 0.0f, 0.0f, 0.0f ),
			float3( 1.0f, 0.0f, 0.0f ),
			float3( 0.0f, 1.0f, 0.0f ));
		LOG_DEBUG								( "m_user.set_listener_properties" );

		m_user.set_active_sound_scene			( m_dbg_scene, 1000, 1000 );

		sounds_collection_type const* type		= find( ai::sound_collection_type_world_ambient );
		R_ASSERT								( type, "such a type is absent in sound collections" );
		
		m_dbg_active_sound						= type->emitter->emit( m_dbg_scene, m_user );
		m_dbg_active_sound->set_position		( mtx.c.xyz() );
		m_dbg_active_sound->play				( sound::once );

		key_v_pressed							= true;
	}
	else
		key_v_pressed							= false;

	static bool key_b_pressed					= false;
	if ( k->is_key_down( input::key_b ) && !key_b_pressed )
	{
		m_user.set_active_sound_scene			( m_scene, 1000, 1000 );
		key_b_pressed							= true;
	}
	else
		key_b_pressed							= false;
}
#endif // #ifndef MASTER_GOLD

sound::command_result_enum ai_sound_player::on_finish_playing( )
{
//	m_active_sound				= 0;
	LOG_DEBUG					( "ai_sound_player::on_finish_playing( )" );
	return sound::command_result_executed;
}

void ai_sound_player::clear_resources	( )
{
	m_user.remove_sound_scene	( m_dbg_scene );
//	m_dbg_scene				= 0;
	m_active_sound			= 0;
	m_dbg_active_sound		= 0;
//	m_scene					= 0;
}

void ai_sound_player::serialize			( )
{
	memory::writer* w			= XRAY_NEW_IMPL( g_allocator, memory::writer ) ( g_allocator );			

	sounds_collection_type const* const begin	= sounds( );
	sounds_collection_type const* const end		= begin + m_sounds_count;
	for ( sounds_collection_type const* i = begin; i != end; ++i )
	{
		i->emitter->serialize	( *w );
	}

	boost::function< void ( memory::writer*, memory::writer* ) > fn
					= boost::bind( &ai_sound_player::on_active_sound_serialized, this, _1, _2 );	
	m_active_sound->serialize	( fn, w );
}

void ai_sound_player::on_active_sound_serialized	( memory::writer* sound_thread_writer, memory::writer* current_thread_writer )
{
	current_thread_writer->write		( sound_thread_writer->pointer(), sound_thread_writer->size() );
	if ( !current_thread_writer->save_to( "Z:/test.sound_player" ) )
	{
		LOG_ERROR( "unable to write file [Z:/test.sound_player]" );
	}

	XRAY_DELETE_IMPL					( g_allocator, current_thread_writer );
}


void ai_sound_player::deserialize		( )
{
	using namespace fs_new;
	synchronous_device_interface const & device	=	resources::get_synchronous_device();

	file_type*		f;
	if ( !device->open( &f, "Z:/test.sound_player", file_mode::open_existing, file_access::read) )
		LOG_ERROR( "unable to open file [Z:/test.sound_player]" );
	else
	{
		device->seek			(f, 0, seek_file_end);
		u32 read_data_size		= (u32)device->tell(f);
		device->seek			(f, 0, seek_file_begin);

		pvoid buff				= MALLOC( read_data_size, "deserialize ai_sound_player" );
		device->read			( f, buff, read_data_size );
		device->close			( f );

		memory::reader* r		= XRAY_NEW_IMPL( g_allocator, memory::reader ) ( (u8 const*)buff, read_data_size );

		sounds_collection_type const* const begin	= sounds( );
		sounds_collection_type const* const end		= begin + m_sounds_count;
		for ( sounds_collection_type const* i		= begin; i != end; ++i )
		{
			i->emitter->deserialize	( *r );
		}

		sounds_collection_type const* type		= find( ai::sound_collection_type_npc_pain );
		R_ASSERT								( type, "such a type is absent in sound collections" );

		boost::function< void ( void ) > func	= boost::bind( &ai_sound_player::on_active_sound_deserialized, this, r, buff );
		m_active_sound	= sound::sound_instance_proxy::deserialize(
			func,
			r,
			m_scene,
			m_user,
			*type->emitter.c_ptr( ),
			boost::bind( &ai_sound_player::on_finish_playing, this ) );
	}
}

void ai_sound_player::on_active_sound_deserialized( memory::reader* reader, pvoid buf )
{
	XRAY_UNREFERENCED_PARAMETERS		( reader, buf );
//	return sound::command_result_executed;
}

} // namespace stalker2

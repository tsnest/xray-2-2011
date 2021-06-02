////////////////////////////////////////////////////////////////////////////
//	Created		: 29.08.2011
//	Author		: Dmitry Kulikov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "sound_system_usage_example_application.h"
#include "sound_system_usage_example_memory.h"
#include <xray/core/core.h>
#include <xray/core/simple_engine.h>
#include <xray/os_include.h>		// for GetCommandLine
//
#include <xray/sound/api.h>
#include <xray/sound/engine.h>
#include <xray/sound/world.h>
#include <xray/sound/world_user.h>
#include <xray/sound/sound_instance_proxy.h>
#include <conio.h>
#include <xray/fs/simple_synchronous_interface.h>

using namespace xray;
using sound_system_usage_example::application;

memory::doug_lea_allocator_type	sound_system_usage_example::g_allocator;


class test_engine :	public core::simple_engine,
						public sound::engine
{
	virtual	pcstr	get_mounts_path		( ) const	{ return "../../mounts"; }
};

static xray::uninitialized_reference< test_engine >	s_core_engine;

xray::uninitialized_reference< xray::fs_new::simple_synchronous_interface >	s_fs_devices;
static xray::resources::mount_ptr	s_resources_mount;

static void on_resources_mounted		( xray::resources::mount_ptr result )
{
	CHECK_OR_EXIT						( result, "Cannot mount resources" );
	s_resources_mount					= result;
}

void application::initialize( )
{
	m_exit_code				= 0;

	XRAY_CONSTRUCT_REFERENCE( s_core_engine, test_engine );

	core::preinitialize		(
		&*s_core_engine,
		logging::create_log,
		GetCommandLine(), 
		command_line::contains_application_true,
		"sound_system_usage_example",
		__DATE__ 
	);

	g_allocator.do_register	( 4*1024*1024, "sound_system_usage_example" );

	memory::allocate_region	( );
	core::initialize		( "../../resources/sources/", "sound_system_usage_example", core::perform_debug_initialization );

	XRAY_CONSTRUCT_REFERENCE	( s_fs_devices, xray::fs_new::simple_synchronous_interface ) 
									( xray::fs_new::watcher_enabled_true );

	xray::core::initialize_resources	( s_fs_devices->hdd_async, s_fs_devices->dvd_async, resources::enable_fs_watcher_false );
	xray::resources::start_resources_threads	( );

	resources::query_mount	( "resources", & on_resources_mounted, &g_allocator );
}

void application::finalize	( )
{
	s_resources_mount		= NULL;

	resources::finish_resources_threads	( );
	core::finalize_resources( );
	core::finalize			( );

	XRAY_DESTROY_REFERENCE	( s_core_engine );
}

void application::on_sound_scene_created	( resources::queries_result& data )
{
	R_ASSERT			( data.is_successful( ) );
	m_sound_scene		= static_cast_resource_ptr< xray::sound::sound_scene_ptr >( data[0].get_unmanaged_resource() );
}

void application::on_sound_collection_created	( resources::queries_result& data )
{
	R_ASSERT						( data.is_successful( ) );

	sound::sound_emitter_ptr e_m	= static_cast_resource_ptr< sound::sound_emitter_ptr >
									( data[0].get_unmanaged_resource() );

	m_npc_marsh						= XRAY_NEW_IMPL( g_allocator, npc )
									(	float3( 10.0f, 0.0f, 0.0f ),
										float3( -1.0f, 0.0f, 0.0f ),
										e_m,
										"Marsh"
									);


	sound::sound_emitter_ptr e_b	= static_cast_resource_ptr< sound::sound_emitter_ptr >
									( data[1].get_unmanaged_resource() );
	
	m_npc_broflovski				= XRAY_NEW_IMPL( g_allocator, npc )
									(	float3( -5.0f, 0.0f, 0.0f ),
										float3( 1.0f, 0.0f, 0.0f ),
										e_b,
										"Broflovski"
									);
}

void application::on_sound_emitter_created	( resources::queries_result& data )
{
	m_sound_emitter		= static_cast_resource_ptr< sound::sound_emitter_ptr >
						( data[0].get_unmanaged_resource() );
}

void application::execute	( )
{

	initialize_sound_world					( );

	// create sound scene
	const_buffer temp_buffer				( "", 1 );
	resources::creation_request scene_request
	(
		"sound_scene",
		temp_buffer,
		resources::sound_scene_class
	);

	resources::query_create_resources_and_wait
	(
		&scene_request,
		1,
		boost::bind( &application::on_sound_scene_created, this, _1 ),
		&g_allocator
	);

	sound::world_user& user				= m_sound_world->get_logic_world_user( );
	user.set_active_sound_scene			( m_sound_scene, 0, 0 );
	user.set_listener_properties
	(	
		m_sound_scene,
		math::create_translation( float3( 0.0f, 0.0f, 0.0f ) ) 
	);

	resources::query_resource_and_wait
	(
		"jokes",
		resources::sound_collection_class,
		boost::bind( &application::on_sound_emitter_created, this, _1 ),
		&g_allocator
	);

	resources::request requests[2];
	requests[0].id		= resources::sound_collection_class;
	requests[0].path	= "jokes";
	requests[1].id		= resources::sound_collection_class;
	requests[1].path	= "jokes_reaction";

	resources::query_resources_and_wait
	(
		requests,
		array_size( requests ),
		boost::bind( &application::on_sound_collection_created, this, _1 ),
		&g_allocator
	);



	m_npc_marsh->register_as_sound_receiver		( m_sound_scene, user );
	m_npc_broflovski->register_as_sound_receiver( m_sound_scene, user );


	user.dispatch_callbacks			( );
	m_sound_world->tick				( );


	for ( ; ; )
	{
		if ( _kbhit( ) )
		{
			char ch		= (char)getchar();

			if		( ch == 'q' )
				break;
			else if ( ch == 'p' )
			{
				m_sound_emitter->emit_and_play_once	
					(
					m_sound_scene,
					user,
					float3( 5.0f, .0f, .0f )
					);
			}
			else if ( ch == 'm' )
			{
				LOG_DEBUG				( "m_npc_marsh->say" );
				m_npc_marsh->emit		( m_sound_scene, user );
				m_npc_marsh->say		( );
			}
			else if ( ch == 'b' )
			{
				LOG_DEBUG				( "m_npc_broflovski->say" );
				m_npc_broflovski->emit	( m_sound_scene, user );
				m_npc_broflovski->say	( );
			}
		}
	}

	m_npc_marsh->unregister_as_sound_receiver		( m_sound_scene, user );
	m_npc_broflovski->unregister_as_sound_receiver	( m_sound_scene, user );

	XRAY_DELETE_IMPL		( g_allocator, m_npc_marsh );
	XRAY_DELETE_IMPL		( g_allocator, m_npc_broflovski );

	m_sound_emitter					= 0;

	user.remove_sound_scene			( m_sound_scene );
	m_sound_scene					= 0;

	m_sound_world->clear_resources	( );

	user.dispatch_callbacks			( );
	m_sound_world->tick				( );

	user.finalize					( );

	sound::destroy_world			( m_sound_world );

	m_exit_code				= s_core_engine->get_exit_code();
}

void application::initialize_sound_world		( )
{
	sound::set_memory_allocator			( g_allocator );

	m_sound_world						= sound::create_world	
										( *s_core_engine.c_ptr(), g_allocator, 0  );

	m_sound_world->get_logic_world_user( ).initialize		( );
}

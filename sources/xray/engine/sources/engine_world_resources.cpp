////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "engine_world.h"
#include <xray/core/core.h>

namespace xray {
namespace engine {

static command_line::key					s_no_fs_watch("no_fs_watch", "", "file system", "disables file system changes watching");

void engine_world::resources_thread	( apc::threads_enum const apc_thread_id )
{
	apc::assign_thread_id				( apc_thread_id, threading::current_thread_id( ) );
	apc::process						( apc_thread_id );

	for (;;) 
	{
		if ( m_resources_destruction_started )
			break;

		resources::resources_thread_yield	( 300 );
		resources::resources_thread_tick	( );
	}
	
	apc::process						( apc_thread_id );
}

void engine_world::cooker_thread ( apc::threads_enum const apc_thread_id )
{
	apc::assign_thread_id				( apc_thread_id, threading::current_thread_id( ) );
	apc::process						( apc_thread_id );

	for (;;) 
	{
		if ( m_resources_cooker_destruction_started )
			break;

		resources::cooker_thread_yield	( 300 );
		resources::cooker_thread_tick	( );
	}
	
	apc::process						( apc_thread_id );
}

#ifndef MASTER_GOLD
void engine_world::watcher_thread ( apc::threads_enum const apc_thread_id )
{
	apc::assign_thread_id				( apc_thread_id, threading::current_thread_id( ) );
	apc::process						( apc_thread_id );

	for (;;) 
	{
		if ( m_resources_cooker_destruction_started )
			break;

		resources::watcher_thread_tick	( );
	}
	
	apc::process						( apc_thread_id );
}
#endif // #ifndef MASTER_GOLD

void   engine_world::initialize_resources ()
{
	core::initialize_resources			( * m_hdd_async_interface.c_ptr( ), 
										  * m_dvd_async_interface.c_ptr( ),
										  s_no_fs_watch ? 
											resources::enable_fs_watcher_false : resources::enable_fs_watcher_true );

	apc::assign_thread_id				( apc::res_man, u32(-1) );
	apc::assign_thread_id				( apc::res_cook, u32(-1) );
	apc::assign_thread_id				( apc::fs_watcher, u32(-1) );

#ifndef MASTER_GOLD
	if ( !s_no_fs_watch )
	{
		threading::spawn					(
			boost::bind( &engine_world::watcher_thread, this, apc::fs_watcher ), 
			"vfs watcher", 
			"vfs-watch", 
			0,
			1 % threading::core_count(),
			threading::tasks_aware
		);
		apc::run_remote_only				( apc::fs_watcher, boost::bind(&apc::empty::function), apc::break_process_loop, apc::wait_for_completion );
	}
#endif //#ifndef MASTER_GOLD

	threading::spawn					(
		boost::bind( &engine_world::resources_thread, this, apc::res_man ), 
		"resources manager", 
		"res_man", 
		0,
		2 % threading::core_count(),
		threading::tasks_aware
	);

	apc::run							( apc::res_man,	& resources::on_resources_thread_started, apc::break_process_loop, apc::wait_for_completion);

	threading::spawn					(
		boost::bind( &engine_world::cooker_thread, this, apc::res_cook ), 
		"resources cooker", 
		"res_cook", 
		0,
		2 % threading::core_count(),
		threading::tasks_aware
	);

	apc::run							( apc::res_cook,	boost::bind(&apc::empty::function), apc::break_process_loop, apc::wait_for_completion);

	R_ASSERT_BOX						(
		resources::get_physical_path_info( fs_new::native_path_string::convert(get_resources_path()) ).does_exist_and_is_folder( ),
		"Resources are not found on path: '%s'\nPlease check working directory", 
		get_resources_path( )
	);
}

void   engine_world::finalize_resources ()
{
	m_resources_mount					= NULL;
	m_user_data_mount					= NULL;
	m_test_resources_mount				= NULL;

	resources::finalize_thread_usage		( true );

	threading::interlocked_exchange			( m_resources_destruction_started, true );
	apc::run								( apc::res_man,	& resources::on_resources_thread_ending, apc::break_process_loop, apc::wait_for_completion);

	threading::interlocked_exchange			( m_resources_cooker_destruction_started, true );
	apc::run_remote_only					( apc::res_cook, boost::bind(&resources::finalize_thread_usage, false), apc::break_process_loop, apc::wait_for_completion );

	if ( !s_no_fs_watch )
		apc::run_remote_only				( apc::fs_watcher, boost::bind(&apc::empty::function), apc::break_process_loop, apc::wait_for_completion );

	core::finalize_resources				( );
}

} // namespace engine
} // namespace xray
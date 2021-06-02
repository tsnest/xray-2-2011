////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

void	log_resources						(memory_type *);

uninitialized_reference<game_resources_manager>		g_game_resources_manager;

void   game_resources_manager::schedule_release_all_resources ()
{
	m_data.flags.set						(game_resources_manager_data::flag_schedule_release_all_resources);
	g_resources_manager->wakeup_resources_thread	();
}

bool   game_resources_manager::is_scheduled_release_all_resources () const 
{ 
	return									m_data.flags.has(game_resources_manager_data::flag_schedule_release_all_resources); 
}

u64   game_resources_manager::current_increase_quality_tick () const 
{ 
	return									m_data.current_increase_quality_tick; 
}

void   initialize_game_resources_manager ()
{
	XRAY_CONSTRUCT_REFERENCE				(g_game_resources_manager, game_resources_manager);

	set_query_finished_callback				(boost::bind(& game_resources_manager::query_finished_callback, 
														   g_game_resources_manager.c_ptr(), _1));

	set_out_of_memory_callback				(boost::bind(& game_resources_manager::out_of_memory_callback, 
														   g_game_resources_manager.c_ptr(), _1));

	set_resource_freed_callback				(boost::bind(& game_resources_manager::on_resource_freed_callback, 
														   g_game_resources_manager.c_ptr(), _1, _2, _3));

	
	vfs::virtual_file_system * const file_system	=	g_resources_manager->get_vfs();

	file_system->on_node_hides			=	boost::bind(& game_resources_manager::on_node_hides, 
														  g_game_resources_manager.c_ptr(), _1);	
	file_system->on_node_unmount		=	boost::bind(& game_resources_manager::on_node_unmount, 
														  g_game_resources_manager.c_ptr(), _1);
	file_system->on_unmount_started		=	boost::bind(& game_resources_manager::on_unmount_started_callback, 
														   g_game_resources_manager.c_ptr());
}

void   finilize_game_resources_manager ()
{
	set_query_finished_callback				(NULL);
	set_out_of_memory_callback				(NULL);
	set_resource_freed_callback				(NULL);

	XRAY_DESTROY_REFERENCE					(g_game_resources_manager);
}

void   schedule_release_all_resources ()
{
	g_game_resources_manager->schedule_release_all_resources ();
}

bool   is_release_all_resources_scheduled	()
{
	return									g_game_resources_manager->is_scheduled_release_all_resources();
}

u64   current_increase_quality_tick ()
{
	return	g_game_resources_manager->current_increase_quality_tick();
}

void   tick_game_resources_manager ()
{
	if ( g_game_resources_manager.initialized() )
		g_game_resources_manager->tick		();
}

void   add_autoselect_quality_query (query_result * query)
{
	g_game_resources_manager->add_autoselect_quality_query (query);
}

void   remove_autoselect_quality_query (query_result * query)
{
	g_game_resources_manager->remove_autoselect_quality_query (query);
}

void   schedule_release_resource_from_grm (resource_base * resource)
{
	g_game_resources_manager->push_resource_to_release	(resource);
}

} // namespace resources
} // namespace xray

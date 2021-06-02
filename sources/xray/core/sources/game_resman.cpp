////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman.h"
#include "game_resman_quality_increase.h"
#include "game_resman_release.h"
#include "game_resman_test_resource.h"
#include "game_resman_test_utils.h"
#include "resources_macros.h"
#include "resources_manager.h"
#include "resources_vfs_associations.h"

namespace xray {
namespace resources {

void   game_resources_manager::query_finished_callback (resource_base * resource)
{
	if ( threading::current_thread_id() == g_resources_manager->resources_thread_id() )
	{
		dispatch_capture						(resource);
	}
	else
	{
		LOGI_TRACE								("grm", "pushed to capture %s", resources::log_string(resource).c_str());
		m_resources_to_capture.push_back		(resource);
		g_resources_manager->wakeup_resources_thread	();
	}
}

void   game_resources_manager::add_new_resource_to_increase_quality_tree (resource_base * resource)
{
	resource->set_target_quality			(resource->current_quality_level(), resource->current_satisfaction());
	quality_increase_functionality	quality_increase	(m_data);
	quality_increase.insert_to_increase_quality_tree	(resource);
}

void   game_resources_manager::capture_resource (resource_base * resource)
{
	if ( resource->cast_base_of_intrusive_base()->is_captured() )
		return;

	memory_usage_type const & memory_usage	=	resource->memory_usage();
	if ( memory_usage.type == & nocache_memory )
		return;

	resource->cast_base_of_intrusive_base()->on_capture_increment_reference_count	();

	LOGI_DEBUG								("grm", "captured %s", log_string(resource).c_str());
	
	memory_type * const info			=	(memory_type *)resource->memory_usage().type;
	if ( !info->in_list )
	{
		m_data.memory_types.push_back		(info);
		info->in_list					=	true;
	}

	info->resources.push_back				(resource);

	resource->update_satisfaction			(m_data.current_increase_quality_tick);
	if ( resource->is_resource_with_quality() )
		add_new_resource_to_increase_quality_tree	(resource);
}

void   game_resources_manager::on_node_unmount (vfs::vfs_iterator & it)
{
	resources::unmanaged_resource_ptr	unmanaged_resource	=	get_associated_unmanaged_resource_ptr(it);
	resources::managed_resource_ptr		managed_resource	=	get_associated_managed_resource_ptr(it);
	resources::resource_base *			base_resource	=	
		unmanaged_resource ? (resources::resource_base *)unmanaged_resource.c_ptr() : managed_resource.c_ptr();

	R_ASSERT								(!base_resource, "resource %s is still associated with fat-node %s! Call Lain",
											 base_resource->log_string().c_str(), it.get_virtual_path().c_str());
}

void   game_resources_manager::release_sub_fat (vfs_sub_fat_resource * resource)
{
	// dispatch captures so freeing see them!
	dispatch_capture						(); 

	R_ASSERT_CMP							(threading::current_thread_id(), ==, g_resources_manager->resources_thread_id());
	resources_to_free_collection			collection	(NULL, NULL, NULL);
	resource_freeing_functionality			resource_freeing(m_data, collection);
	resource_freeing.release_sub_fat		(resource);
}

void   game_resources_manager::on_unmount_started_callback ()
{
	// before unmount - dispatch queries, so unmount can signal us about resources we capture
	dispatch_capture						();
}

void   game_resources_manager::on_node_hides (vfs::vfs_iterator & it)
{
	resources::unmanaged_resource_ptr	unmanaged_resource	=	get_associated_unmanaged_resource_ptr(it);
	resources::managed_resource_ptr		managed_resource	=	get_associated_managed_resource_ptr(it);
	resources::resource_base * const	resource	=	managed_resource ? 
																(resources::resource_base *)managed_resource.c_ptr() 
																: unmanaged_resource.c_ptr();
	if ( !resource )
		return;

	if ( resource->is_pinned_by_grm() )
		release_resource					(resource);

	set_associated							(it, NULL);
	LOGI_INFO								("fs", "deassociated resource from node: '%s'", it.get_name());
}

void   game_resources_manager::log_resources (memory_type * info)
{
	fixed_string8192		log_string;
	for ( resource_base *	it_resource	=	info->resources.front();
							it_resource;
							it_resource	=	info->resources.get_next_of_object(it_resource) )
	{
		if ( unmanaged_resource_ptr unmanaged = it_resource->cast_unmanaged_resource() )
			if ( test_resource_ptr test_resource = static_cast_resource_ptr<test_resource_ptr>(unmanaged) )
				log_string.appendf			("%d ", test_resource->int_name());
	}

	LOGI_INFO								("grm", "resources in tree: %s", log_string.c_str());
}

void   game_resources_manager::tick ()
{
	dispatch_capture						();

	tick_memory_types						();

	if ( m_data.flags.has(game_resources_manager_data::flag_schedule_release_all_resources) )
	{
		dispatch_capture					(); // need it here
		releasing_functionality	releasing	(m_data);
		if ( releasing.release_all_resources() )
		{
			m_data.flags.unset				(game_resources_manager_data::flag_schedule_release_all_resources);
			g_resources_manager->wakeup_resources_thread	();
			// waking to continue release_all_resources
		}
	}

	dispatch_resources_to_release			();
	
	quality_increase_functionality			quality_increase(m_data);
	quality_increase.tick					();
}

void   game_resources_manager::add_autoselect_quality_query (query_result * query)
{
	ASSERT									(query->is_autoselect_quality_query());

	query->update_quality_levels_count		();
	query->update_satisfaction				(m_data.current_increase_quality_tick);
	add_new_resource_to_increase_quality_tree	(query);
}

void   game_resources_manager::remove_autoselect_quality_query (query_result * query)
{
	ASSERT									(query->is_autoselect_quality_query());
	ASSERT									(query->has_flags(resource_flag_in_grm_increase_quality_tree));

	quality_increase_functionality	quality_increase	(m_data);
	quality_increase.erase_from_increase_quality_tree	(query);
}

void   game_resources_manager::dispatch_capture (resource_base * resource)
{
	capture_resource						(resource);
	managed_resource_ptr	managed_resource	=	resource->cast_managed_resource();
	unmanaged_resource_ptr	unmanaged_resource	=	resource->cast_unmanaged_resource();
	resource->cast_base_of_intrusive_base()->unpin_reference_count_for_query_finished_callback();
	// if it_resource is 0 ref count, destructor of ptr will call destroy object
}

void   game_resources_manager::dispatch_capture ()
{
	if ( m_resources_to_capture.empty() )
		return;

	resource_base * it_resource				=	m_resources_to_capture.pop_all_and_clear();
	while ( it_resource )
	{
		resource_base * const next			=	m_resources_to_capture.get_next_of_object(it_resource);
		LOGI_TRACE								("grm", "dispatched query finished callback: %s", resources::log_string(it_resource).c_str());
		dispatch_capture						(it_resource);
		it_resource							=	next;
	}
}

void   game_resources_manager::push_resource_to_release (resource_base * resource)
{
	if ( resource->has_flags(resource_flag_in_grm_release) )
		return;

	resource->set_flags							(resource_flag_in_grm_release);
	m_resources_to_release.push_back			(resource);
	g_resources_manager->wakeup_resources_thread	();
}

void   game_resources_manager::dispatch_resources_to_release ()
{
	resource_base * it_resource			=	m_resources_to_release.pop_all_and_clear();
	while ( it_resource )
	{
		resource_base * const next		=	m_resources_to_release.get_next_of_object(it_resource);

		it_resource->unset_flags			(resource_flag_in_grm_release);
		if ( !it_resource->cast_base_of_intrusive_base()->is_captured() )
		{
			it_resource					=	next;
			continue;
		}

		LOG_INFO							("releasing resource from game resources manager: %s", resources::log_string(it_resource).c_str());
		release_resource					(it_resource);
		it_resource						=	next;
	}
}

void   game_resources_manager::release_resource	(resource_base * resource)
{
	releasing_functionality	releasing		(m_data);
	releasing.release_resource				(resource);
}

game_resources_manager::~game_resources_manager	()
{
	releasing_functionality					releasing	(m_data);
	bool const all_released				=	releasing.release_all_resources();
	R_ASSERT								(all_released);

	vfs::virtual_file_system * const file_system	=	g_resources_manager->get_vfs();
	file_system->on_node_unmount		=	NULL;
	file_system->on_unmount_started		=	NULL;
	file_system->on_node_hides			=	NULL;
}

} // namespace resources
} // namespace xray

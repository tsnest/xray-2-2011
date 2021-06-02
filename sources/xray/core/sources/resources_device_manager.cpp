////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/managed_allocator.h>
#include <xray/resources_query_result.h>
#include <xray/os_include.h>
#include <xray/vfs/virtual_file_system.h>
#include <xray/fs/asynchronous_device_commands.h>
#include "resources_device_manager.h"
#include "resources_macros.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

namespace detail {

query_logging_string   make_query_logging_string (query_result * query)
{
	fs_new::native_path_string const file_path	=	query->absolute_physical_path(assert_on_fail_false);

	vfs::vfs_iterator it				=	query->get_fat_it();
	query_logging_string	full_path	=	it.get_virtual_path().c_str();
	
	if ( it.is_end() )
		full_path.appendf					(" request path: %s", query->get_requested_path());
	
	query_logging_string					out_result; 
	out_result.assignf						( "'%s' from '%s'", full_path.c_str(), file_path.c_str());
	return									out_result;
}

} // namespace detail

//----------------------------------------------------------
// class device_manager
//----------------------------------------------------------

device_manager::device_manager			(u32 const sector_size) 
										:	m_doing_async_write(false), 
											m_sector_size(sector_size),
											m_pre_allocated_size(0),
											m_min_wanted_pre_allocated_size(4*1024),
											m_sector_data_last_file_pos((file_size_type)-1)

{
	m_ready								=	false;
	m_file_handle						=	NULL;
	m_min_bytes_to_operate				=	0;
}

void   device_manager::push_query (query_result * query)
{
	LOGI_TRACE								("resources:device_manager", "%s query added %s", 
											query->is_load_type() ? "read" : "write", detail::make_query_logging_string (query).c_str());

	push_query_impl							(query);
}

void   device_manager::on_pre_allocated_size_change (int size_delta, query_result * const query)
{
	XRAY_UNREFERENCED_PARAMETER				(query);
	R_ASSERT								(query->is_load_type());

// 	LOGI_INFO								("resources:test", "pre_allocated_size = %d, increment = %d (resource: %s)", 
// 											m_pre_allocated_size, size_delta, detail::make_query_logging_string (query).c_str());
	R_ASSERT_CMP							(m_pre_allocated_size, >=, -size_delta);
	m_pre_allocated_size				+=	size_delta;	
}

bool   device_manager::pre_allocate (query_result* const query)
{
	if ( query->is_load_type() )
	{
		if ( !query->raw_unmanaged_buffer() && !query->raw_managed_resource() )
		{
			allocation_result_enum const allocation_result	=	query->allocate_raw_managed_resource_if_needed();

			if ( allocation_result == allocation_result_failed )
			{
				m_finished.push_back		(query);
				return						false;
			}
		}
	}

	if ( query->get_error_type() == query_result_for_user::error_type_out_of_memory )
		return								false;

	threading::mutex_raii					pre_allocated_mutex_raii(m_pre_allocated_mutex);

	fs_new::asynchronous_device_interface * const async_device	=	g_resources_manager->get_hdd();

	fs_new::query_custom_operation_args	args	(boost::bind( & device_manager::process_query, this, query, _1),
												 boost::bind( & device_manager::on_query_processed, this, query, _1),
												 & g_resources_manager->get_resources_wakeup_event());

	if ( query->is_load_type() )
	{
		u32 const raw_buffer_size		=	query->raw_buffer_size();
		on_pre_allocated_size_change		(raw_buffer_size, query);
	}

	async_device->query_custom_operation	(args, helper_allocator());

	return									true;
}

void   device_manager::fill_pre_allocated ()
{
	queries_list							queries;
	bool grabbed_something				=	false;
	do 
	{
		grab_sorted_queries					(queries);
		grabbed_something				=	!queries.empty();

		for ( query_result *	it_query	=	queries.front();
								it_query; )
		{
			query_result * const it_next	=	queries.get_next_of_object(it_query);
			pre_allocate					(it_query);
			it_query					=	it_next;
		}

		queries.pop_all_and_clear			();

	} while ( grabbed_something ) ;
}

vfs_sub_fat_resource *	get_node_sub_fat	(vfs::base_node<> * const node);

void   device_manager::on_query_processed	(query_result * query, bool result)
{
	R_ASSERT								(result);
	threading::mutex_raii	raii			(m_pre_allocated_mutex);
	if ( query->is_load_type() )
		on_pre_allocated_size_change		(-(int)query->raw_buffer_size(), query);

	if ( query->is_replication_type() )
	{
#pragma message(XRAY_TODO("implement replication here"))
		NOT_IMPLEMENTED					("implement replication!");
// 			vfs::vfs_iterator fat_it	=	finished->get_fat_it();
// 			fs::g_fat->commit_replication	(fat_it.get_fat_node());
// 			LOGI_INFO						("resources", "replicated %s ", detail::make_query_logging_string (finished).c_str());
	}
	else if ( query->is_save_type() )
	{
		vfs::virtual_file_system * const file_system	=	g_resources_manager->get_vfs();
		save_generated_data * const data	=	query->get_save_generated_data();

		fs_new::native_path_string const physical_path	=	data->get_physical_path();
		fs_new::virtual_path_string virtual_path		=	data->get_virtual_path();

		vfs::vfs_locked_iterator			iterator;
		query_hot_mount_and_wait			(* file_system, 
											 physical_path, 
											 & virtual_path, 
											 & iterator, 
											 helper_allocator(),
											 boost::bind(& resources_manager::dispatch_callbacks, g_resources_manager.c_ptr(), false));

		R_ASSERT							(virtual_path);
		LOGI_INFO							("resources", "written generated resource to file: '%s'", physical_path.c_str());
		query->late_set_fat_it				(iterator);
	}

	query->on_file_operation_end			();
}

void   device_manager::update ()
{
	if ( m_pre_allocated_size < (int)m_min_wanted_pre_allocated_size )
		fill_pre_allocated					();

	fs_new::asynchronous_device_interface * const hdd	=	g_resources_manager->get_hdd();
	fs_new::asynchronous_device_interface * const dvd	=	g_resources_manager->get_dvd();

	if ( threading::g_debug_single_thread )
	{
		if ( hdd )
			hdd->tick						(false);
		if ( dvd )
			dvd->tick						(false);
	}
}

} // namespace resources
} // namespace xray
////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include <xray/resources_fs.h>
#include <xray/fs/device_utils.h>
#include <xray/configs_binary_config.h>
#include "resources_fs_task_composite.h"
#include "resources_fs_task_mount.h"

namespace xray {
namespace resources {

void   add_fs_task						(fs_task * const new_task, fs_task_composite * const composite_task)
{
	if ( composite_task )
		composite_task->add_child			(new_task);
	else
		g_resources_manager->add_fs_task	(new_task);
}

fs_task_composite *   create_mount_composite_helper_task_and_lock (memory::base_allocator * const allocator)
{
	fs_task_composite *	const new_task	=	XRAY_NEW_IMPL(* allocator, fs_task_composite) (allocator);

	new_task->lock_children					();

	g_resources_manager->add_fs_task		(new_task);
	return									new_task;
}

class mount_by_config_helper
{
public:
	enum type_enum		{	type_mount, 
							type_unmount	};

	mount_by_config_helper				(type_enum const				type,
										 pcstr const					mount_id, 
										 query_mount_callback			callback,
										 memory::base_allocator * const allocator) 
	:	m_type							(type), 
		m_mount_id						(mount_id), 
		m_callback						(callback), 
		m_allocator						(allocator), 
		m_success						(true), 
		m_physical_mounts_left_count	(0) 
	{
		fs_new::virtual_path_string			path;
		path.assignf						("mounts/%s.mount", mount_id);

		request	requests[]				=	{ { path.c_str(), binary_config_class } };
		
		g_resources_manager->change_count_of_pending_helper_query_for_mount(+1);
		
		query_resource_params	params		(requests, NULL, 1, 
											 boost::bind(& mount_by_config_helper::callback, this, _1),
											 allocator, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, query_type_helper_for_mount);

		m_mount_composite_task					=	create_mount_composite_helper_task_and_lock(allocator);
		g_resources_manager->query_resources_impl	(params);
	}

	void	callback					(queries_result & config_result)
	{
		if ( !config_result.is_successful() )
		{
			query_db										();
			return;
		}
		else
		{
			configs::binary_config_ptr const config_ptr	=	static_cast_checked<configs::binary_config *>
															(config_result[0].get_unmanaged_resource().c_ptr());


			ASSERT( config_ptr );

			configs::binary_config_value const & config	=	config_ptr->get_root();
			query_by_config									(config);
		}

		m_mount_composite_task->unlock_children	();
		//	unlock a lock requested by create_mount_composite_helper_task_and_lock
		//	now composite task can start executing

		g_resources_manager->change_count_of_pending_helper_query_for_mount(-1);
	}

private:
	void	query_db						()
	{
		fs_new::native_path_string			path;
		path.assignf_with_conversion		("mounts/%s.db", m_mount_id.c_str());

		query_mount_archive					("", path, path, m_mount_id.c_str(), m_callback, m_allocator, m_mount_composite_task);
		delete_this							();
	}

	void	physical_mount_callback		(mount_ptr result)
	{
		m_success						&=	(result != NULL);

		if ( result )
		{
			if ( !m_result )
				m_result				=	result;
			else
				m_result->link_mount_ptr	(result);			
		}

		R_ASSERT							(m_physical_mounts_left_count);
		--m_physical_mounts_left_count;
		if ( !m_physical_mounts_left_count )
		{
			if ( m_callback )
				m_callback					(m_success ? m_result : NULL);
			delete_this						();
		}
	}

	void	query_by_config				(configs::binary_config_value const & config)
	{
		configs::binary_config_value const & mounts	=	config["mounts"];
		m_physical_mounts_left_count	=	mounts.size();
		m_success						=	true;
	
		int start_index					=	(m_type == type_mount) ? 0 : (int)mounts.size() - 1;
		int end_index					=	(m_type == type_mount) ? (int)mounts.size() : -1;
		int step						=	(m_type == type_mount) ? +1 : -1;

		for ( int index = start_index; index != end_index; index += step )
		{
			configs::binary_config_value const & value	=	mounts[index];

			R_ASSERT								(value.value_exists("physical_path"), "physical_path must be set in mount configs");
			fs_new::native_path_string	const physical_path	=	fs_new::native_path_string::convert((pcstr)value["physical_path"]);

			R_ASSERT								(value.value_exists("virtual_path"), "virtual_path must be set in mount configs");
			fs_new::virtual_path_string	const virtual_path	=	(pcstr)value["virtual_path"];

			if ( value.value_exists("create_physical_path_if_not_exists") && 
				 (u32)value["create_physical_path_if_not_exists"] == 1 )
			{
				fs_new::create_folder_r		(g_resources_manager->get_synchronous_device(), 
											 physical_path.c_str(), 
											 true);
			}

			pcstr const mount_descriptor	=	(pcstr)value["id"];

			bool const watcher_enabled	=	value.value_exists("watcher_enabled") ?
												(bool)value["watcher_enabled"] : true;
			bool const lazy_mount		=	value.value_exists("lazy_mount") ?
												(bool)value["lazy_mount"] : true;

			query_mount_physical			(virtual_path, 
											 physical_path,
											 mount_descriptor,
											 (fs_new::watcher_enabled_bool)watcher_enabled,
											 boost::bind(& mount_by_config_helper::physical_mount_callback, this, _1), 
											 m_allocator,
											 (recursive_bool)!lazy_mount,
											 m_mount_composite_task);
		}
	}

	void	delete_this					()
	{
		mount_by_config_helper * this_ptr	=	this;
		XRAY_DELETE_IMPL					(m_allocator, this_ptr);
	}

	query_mount_callback				m_callback;
	fs_new::virtual_path_string			m_mount_id;
	fs_task_composite *					m_mount_composite_task;
	u32									m_physical_mounts_left_count;
	type_enum							m_type;
	memory::base_allocator *			m_allocator;
	bool								m_success;
	mount_ptr							m_result;
};

void   query_mount 						(pcstr const					mount_id,
				   						 query_mount_callback const		callback,
										 memory::base_allocator * const	allocator)
{
	XRAY_NEW_IMPL							(allocator, mount_by_config_helper)
											(mount_by_config_helper::type_mount, mount_id, callback, allocator);
	// memory will be freed by the object itself
}

void   query_mount_physical 			(fs_new::virtual_path_string const &	virtual_path, 
										 fs_new::native_path_string const &		physical_path, 
										 pcstr									descriptor,
										 fs_new::watcher_enabled_bool const		watcher_enabled,
						 				 query_mount_callback const				callback, 
						 				 memory::base_allocator * const			allocator,
						 				 recursive_bool const					recursive,
						 				 fs_task_composite * const				composite_task)
{
	fs_task_mount * const new_task		=	XRAY_NEW_IMPL(* allocator, fs_task_mount)
												(virtual_path,
												 physical_path,
												 descriptor,
												 watcher_enabled,
												 callback,
												 recursive,
												 allocator);
	
	add_fs_task								(new_task, composite_task);
}

void   query_mount_archive 				(fs_new::virtual_path_string const &	virtual_path,
										 fs_new::native_path_string const & 	fat_physical_path,
										 fs_new::native_path_string const & 	archive_physical_path,
										 pcstr									descriptor,
										 query_mount_callback const &			callback,
										 memory::base_allocator * const			allocator,
										 fs_task_composite * const				composite_task)
{
	fs_task_mount * const new_task		=	XRAY_NEW_IMPL(* allocator, fs_task_mount)
												(virtual_path,
												 fat_physical_path,
												 archive_physical_path,
												 descriptor,
												 callback,
												 allocator);											 

	add_fs_task								(new_task, composite_task);
}

} // namespace resources
} // namespace xray



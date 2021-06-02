////////////////////////////////////////////////////////////////////////////
//	Created		: 25.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_fs_iterator.h>
#include "resources_manager.h"
#include <xray/fs_utils.h>

namespace xray {
namespace resources {

using namespace fs;

void   log_fs_iterator_impl (fs_iterator it, buffer_string & prefix)
{
	if ( it.is_end() )
		return;

	path_string							full_path;
	it.get_full_path					(full_path);
	pcstr const name				=	it.get_name();

	path_string							file_info;
	if ( !it.is_folder() )
		file_info.assignf				("size (%d)", it.get_file_size());
	if ( it.is_compressed() )
		file_info.appendf				(" compressed (%d)", it.get_compressed_file_size());

	LOG_INFO							("%s %s (%s) %s", 
										 prefix.c_str(), 
										 name, 
										 full_path.c_str(), 
										 file_info.c_str());

	char const prefix_inc[]			=	"  ";
	u32 const prefix_inc_len		=	array_size(prefix_inc) - 1;
	prefix							+=	prefix_inc;
	for ( fs_iterator			jt	=	it.children_begin();
								jt	!=	it.children_end();
								++jt )
	{
		log_fs_iterator_impl			(jt, prefix);
	}
	
	prefix.set_length					(prefix.length() - prefix_inc_len);
}

void   log_fs_iterator (fs_iterator it)
{
	fixed_string<1024>	prefix;
	log_fs_iterator_impl					(it, prefix);
}

void   query_fs_iterator (pcstr							path, 
						  query_fs_iterator_callback	callback, 
						  memory::base_allocator *		allocator,
						  is_recursive_bool				is_recursive,
						  query_result_for_cook *		parent,
						  bool							call_from_get_path_info)
{
	ASSERT									(callback);

	bool sync_way_worked				=	false;
	if ( g_resources_manager->try_lock_read_fat() )
	{
		// TRYING SYNC WAY

		sub_fat_pin_fs_iterator				iterator;
		bool const found_sync			=	g_fat->try_find_sync(& iterator,
																 path, 
																 is_recursive ? find_recursively : 0);

		if ( call_from_get_path_info && 
			 threading::current_thread_id() == resources::g_resources_manager->resources_thread_id() )
		{
			R_ASSERT						(found_sync);
		}

		if ( found_sync )
		{
			callback						(iterator);
			sync_way_worked				=	true;
		}

		g_resources_manager->unlock_read_fat();
	}

	if ( !sync_way_worked )
	{
		// ASYNC WAY
		threading::mutex & mount_transaction_mutex	
										=	g_resources_manager->get_mount_transaction_mutex();
		threading::mutex_raii raii			(mount_transaction_mutex);
		
		fs_task *	new_task			=	XRAY_NEW_IMPL(* allocator, fs_task)(fs_task::type_fs_iterator_task);
		new_task->fs_iterator_path		=	path;
		new_task->allocator				=	allocator;
		new_task->fs_iterator_callback	=	callback;
		new_task->fs_iterator_recursive	=	(is_recursive == is_recursive_true);
		new_task->parent				=	parent;
		new_task->call_from_get_path_info	=	call_from_get_path_info;

		g_resources_manager->add_fs_task	(new_task);
	}
}

struct query_fs_iterator_and_wait_callback_proxy_pred
{
	query_fs_iterator_and_wait_callback_proxy_pred(query_fs_iterator_callback const callback) : callback_(callback), receieved_callback_(false) {}
	void callback (fs_iterator result)
	{
		callback_				(result);
		receieved_callback_	=	true;
	}

	bool received_callback		() const { return receieved_callback_; }

private:
	bool						receieved_callback_;
	query_fs_iterator_callback	callback_;
};

void   query_fs_iterator_and_wait	(pcstr							path, 	
									 query_fs_iterator_callback		callback, 
							 		 memory::base_allocator*		allocator,
									 is_recursive_bool				is_recursive,
									 query_result_for_cook *		parent,
									 bool							call_from_get_path_info)
{
	query_fs_iterator_and_wait_callback_proxy_pred		callback_proxy	(callback);
	query_fs_iterator				(path, boost::bind(& query_fs_iterator_and_wait_callback_proxy_pred::callback, & callback_proxy, _1), allocator, is_recursive, parent, call_from_get_path_info);

	while ( !callback_proxy.received_callback() )
	{
		if ( threading::g_debug_single_thread )
			resources::tick			();
		else if ( threading::current_thread_id() == g_resources_manager->resources_thread_id()	)
			g_resources_manager->resources_tick_impl	();

		dispatch_callbacks			();
	}
}

} // namespace resources
} // namespace xray 
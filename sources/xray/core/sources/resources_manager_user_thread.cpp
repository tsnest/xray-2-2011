////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "resources_device_manager.h"
#include "resources_hdd_manager.h"
#include "resources_macros.h"
#include <xray/managed_allocator.h>
#include "resources_thread_local_data.h"
#include <xray/resources_queries_result.h>
#include <xray/os_include.h>	// for MemoryBarrier

namespace xray {
namespace resources {

void   resources_manager::start_query_transaction ()
{
	thread_local_data *	const local_data	=	get_thread_local_data(threading::current_thread_id(), true);
	ASSERT										(!local_data->in_transaction);

	local_data->in_transaction				=	true;
}

void   resources_manager::end_query_might_destroy_this_transaction ()
{
	thread_local_data *	const local_data	=	get_thread_local_data(threading::current_thread_id(), false);
	ASSERT										(local_data && local_data->in_transaction);
	local_data->in_transaction				=	false;

	query_result * to_init					=	local_data->to_init_by_transaction.pop_all_and_clear();
	while ( to_init )
	{
		query_result * const next			=	local_data->to_init_by_transaction.get_next_of_object(to_init);
		push_new_query							(to_init);

		to_init								=	next;
	}

	wakeup_resources_thread						();
}

bool   is_fs_iterator_class				(class_id_enum const class_id)
{
	return									class_id == fs_iterator_class ||
											class_id == fs_iterator_recursive_class;
}

bool   cook_must_be_registered			(class_id_enum const class_id)
{
	return									class_id != raw_data_class && 
											class_id != raw_data_class_no_reuse && 
											!is_fs_iterator_class(class_id);											
}

queries_result *   resources_manager::create_queries_result (query_resource_params const & params)
{
	R_ASSERT								(params.requests || params.requests_create || !params.requests_count);
	u32* strings_lengths_with_zero		=	(u32 *)ALLOCA(sizeof(u32) * params.requests_count);
	u32	 additional_strings_length		=	0;
	u32  user_data_variant_count		=	0;
	u32	 transforms_count				=	0;
	for ( u32 i=0; i<params.requests_count; ++i )
	{
		if ( params.user_data && params.user_data[i] )
			++user_data_variant_count;

		if ( params.transforms && params.transforms[i] )
			++transforms_count;

		bool const is_query_create		=	params.requests_create && (params.requests_create[i].get_id() != unknown_data_class);

		R_ASSERT							(params.requests_create || params.requests);
		if ( !is_query_create )
			R_ASSERT						(params.requests[i].path, "empty path in request?");

		class_id_enum const request_class_id	=	is_query_create ? 
														params.requests_create[i].get_id() : params.requests[i].id;

		if ( cook_must_be_registered(request_class_id) && !cook_base::find_cook(request_class_id) )
		{
			R_ASSERT						(g_resources_manager->cooks_are_registering(),
											 "Omg! Cook is not registered for resource_id: %d", request_class_id);
		}

		pcstr request_path				=	is_query_create ? 
												params.requests_create[i].get_name() : params.requests[i].path;
		if ( !request_path )
			request_path				=	"";

		bool allocate_max_string_length	=	false;

		if ( g_resources_manager->cooks_are_registering() )
			allocate_max_string_length	=	true;
		else if ( !is_query_create && params.requests[i].id != raw_data_class && params.requests[i].id != raw_data_class_no_reuse )
			allocate_max_string_length	=	cook_base::does_create_resource_if_no_file(params.requests[i].id);

		if ( allocate_max_string_length )
			strings_lengths_with_zero[i] =	math::max((u32)fs_new::max_path_length, strings::length(request_path) + 1);
		else
			strings_lengths_with_zero[i] =	strings::length(request_path) + 1;

		if ( strings_lengths_with_zero[i] > array_size(((query_result *)NULL)->m_request_path_default_storage) )
			additional_strings_length	+=	strings_lengths_with_zero[i];
	}

	u32 const queries_size				=	sizeof(queries_result) + (sizeof(query_result) * params.requests_count);
	u32 const user_data_size			=	(sizeof(user_data_variant) * user_data_variant_count);
	u32 const transforms_size			=	sizeof(math::float4x4) * transforms_count;
	u32 const full_allocation_size		=	queries_size + user_data_size + transforms_size + additional_strings_length;
	
	queries_result * const queries		=	(queries_result *)
											XRAY_ALLOC_IMPL(params.allocator, 
															char, 
															full_allocation_size);

	bool const parent_is_helper_for_mount	=	params.parent && params.parent->is_helper_query_for_mount();
	bool const self_is_helper_for_mount		=	params.query_type == query_type_helper_for_mount;
	query_type_enum const query_type	=	(parent_is_helper_for_mount || self_is_helper_for_mount) ?
											query_type_helper_for_mount : query_type_normal;

	assert_on_fail_bool assert_on_fail	=	params.assert_on_fail;
	if ( params.parent && !params.parent->assert_on_fail() )
		assert_on_fail					=	assert_on_fail_false;

	u32 const user_thread_id			=	threading::current_thread_id();
	new ( queries )							queries_result((u32)params.requests_count, 
															params.callback, 
															params.allocator, 
															user_thread_id,
															params.parent,
															params.target_satisfactions,
															params.disable_cache,
															params.quality_indexes,
															query_type,
															params.autoselect_quality,
															assert_on_fail);
	if ( params.out_queries_id )
		* params.out_queries_id			=	queries->uid();

	queries->increment_reference_count		();

	user_data_variant *	cur_user_data_pos	=	pointer_cast<user_data_variant*>( pointer_cast<pbyte>( queries ) + queries_size);
	math::float4x4 * cur_transform_pos	=	pointer_cast< math::float4x4 *>( pointer_cast<pbyte>( queries ) + queries_size + user_data_size);
	pstr	cur_string_pos				=	(pstr)queries + queries_size + user_data_size + transforms_size;
	u32     space_left					=	additional_strings_length;
	for ( u32 i=0; i<params.requests_count; ++i )
	{
		query_result & query			=	queries->at(i);
		if ( params.user_data && params.user_data[i] )
		{
			query.m_user_data			=	cur_user_data_pos;
			new (query.m_user_data)			user_data_variant;
			* query.m_user_data			=	* params.user_data[i];
			++cur_user_data_pos;
		}

		if ( params.transforms && params.transforms[i] )
		{
			query.m_transform			=	cur_transform_pos;
			new (query.m_transform)			math::float4x4;
			* query.m_transform			=	* params.transforms[i];
			++cur_transform_pos;
		}

		bool const is_query_create		=	params.requests_create && (params.requests_create[i].get_id() != unknown_data_class);
		query.m_class_id				=	is_query_create ? 
												params.requests_create[i].get_id() : params.requests[i].id;

		pcstr request_path				=	is_query_create ? params.requests_create[i].get_name() : params.requests[i].path;
		if ( !request_path )
			request_path				=	"";

		bool const request_path_fits_default_storage =	strings::length(request_path) < array_size(query.m_request_path_default_storage);

		query.m_request_path			=	request_path_fits_default_storage ? query.m_request_path_default_storage : cur_string_pos;
		query.m_request_path_max_size	=	request_path_fits_default_storage ? array_size(query.m_request_path_default_storage) : strings_lengths_with_zero[i];

		if ( is_query_create && params.requests_create[i].get_data().size() )
			query.m_creation_data_from_user	=	params.requests_create[i].get_data();

		memory::copy						(query.m_request_path, 
											 request_path_fits_default_storage ? query.m_request_path_max_size : space_left, 
											 request_path, 
											 strings_lengths_with_zero[i]);

		if ( !request_path_fits_default_storage )
		{
			space_left					-=	strings_lengths_with_zero[i];
			cur_string_pos				+=	strings_lengths_with_zero[i];
		}
	}

	return									queries;
}

struct sorting_predicate
{	
	bool operator () (query_result * r1, query_result * r2) const
	{
		if ( r1->quality_index() >= r2->quality_index() )
			return							true;
		return								false;
	}
};

long   resources_manager::query_resources_impl   (query_resource_params const & params)
{
	thread_local_data *	const local_data	=	get_thread_local_data(threading::current_thread_id(), false);
	if ( local_data && local_data->in_translate_query_counter && !local_data->disable_translate_query_counter_check )
		R_ASSERT							(params.parent, "you must pass parent when you query_resource from cook");

	queries_result * queries			=	create_queries_result(params);

	R_ASSERT								(params.requests_count, "you should query at least 1 resource");

	XRAY_MEMORY_BARRIER_FULL				();
	queries->translate_request_paths		();

	for ( u32 i=0; i<params.requests_count; ++i )
	{
		query_result & query			=	queries->at(i);
		if ( query.is_autoselect_quality_query() )
			continue;

		//--------------------------------------------
		// look for resource in name registry cache
		//--------------------------------------------

		bool const only_try_to_get		=	!query.creation_data_from_user() || query.is_translate_query();

		query_result::consider_with_name_registry_result_enum const registry_result	=	
			query.consider_with_name_registry((query_result::only_try_to_get_associated_resource_bool)only_try_to_get);

		if ( registry_result == query_result::consider_with_name_registry_result_got_associated_resource )
		{
			query.end_query_might_destroy_this	();
			continue;
		}
		else if ( registry_result == query_result::consider_with_name_registry_result_added_as_referer )
			continue;

		if ( query.is_translate_query() )
			query.translate_query_if_needed	();
	}

	bool const can_try_sync				=	!g_resources_manager->mount_operations_are_pending();

	if ( can_try_sync )
	for ( u32 i=0; i<params.requests_count; ++i )
	{
		query_result & query		=	queries->at(i);

		if ( query.is_autoselect_quality_query() )
			continue;

		if ( query.is_fs_iterator_query() )
			continue;

		if ( query.is_translate_query() )
			continue;

		if ( query.has_flag(query_result::flag_is_referer) )
			continue;

		if ( query.has_flag(query_result::flag_finished) )
			continue;

		if ( query.creation_data_from_user() )
			continue;

		if ( query.process_request_path (true) )
		{
			if ( !query.check_fat_for_resource_reusage () )
			{
				query.allocate_raw_unmanaged_resource_if_needed();
				query.try_synchronous_cook_from_inline_data();
			}
		}
	}
	
	bool const in_transaction				=	local_data && local_data->in_transaction && 
												(params.query_type == query_type_normal);
	
	typedef	buffer_vector<query_result *>	query_array_type;
	query_array_type						queries_array	(ALLOCA(params.requests_count * sizeof(query_result *)),
															 params.requests_count);

	// ASYNC WAY
	bool pushed_something				=	false;
	bool need_sorting					=	false;
	bool has_fs_iterator_queries		=	false;
	
	for ( u32 i=0; i<params.requests_count; ++i )
	{
		query_result & query			=	queries->at(i);

		if ( query.has_flag(query_result::flag_finished) )
			continue;
		if ( query.has_flag(query_result::flag_translated_query) )
			continue;					// did translate query synchronously
		if ( query.has_flag(query_result::flag_is_referer) )
			continue;					// query became a referer, skip
		if ( query.has_flag(query_result::flag_was_referer) )
			continue;					// will be processed automatically
		if ( query.is_fs_iterator_query() )
		{
			has_fs_iterator_queries		=	true;
			continue;
		}
		
		if ( query.quality_index() != 0 )
			need_sorting				=	true;
		queries_array.push_back				(& query);
	}

	if ( need_sorting )
		std::sort							(queries_array.begin(), queries_array.end(), sorting_predicate());

	for ( query_array_type::iterator	it		=	queries_array.begin(),
										it_end	=	queries_array.end();
										it		!=	it_end;
										++it )
	{
		query_result * query			=	* it;
		if ( in_transaction )
			local_data->to_init_by_transaction.push_back	(query);
		else
		{
			push_new_query					(query);
			pushed_something			=	true;
		}
	}

	if ( pushed_something && !in_transaction )
		wakeup_resources_thread				();

	if ( queries->size() == 0 )
		queries->on_query_end				(true);

#pragma message(XRAY_TODO("Lain 2 Lain: revisit to check transactions are working in presence of fs iterator requests"))
	if ( !pushed_something && !in_transaction && has_fs_iterator_queries )
	{
		queries->query_fs_iterators			();
	}

	if ( queries->decrement_reference_count() == 0 )
	{
		// SYNC WAY
		if ( !queries->is_finished() )
		{
			R_ASSERT						(!queries->size());
			queries->set_result				(true);
		}

		R_ASSERT							(queries->is_finished());
		queries->end_and_delete_self		(false);
	}

	return									queries->uid();
}

class query_resources_helper
{
public:
	query_resources_helper (pcstr					const dir_path, 
							pcstr					const mask,
							class_id_enum			const class_id,
							query_resource_params const & params) 
							: m_dir_path		(dir_path), 
							  m_mask			(mask), 
							  m_class_id		(class_id),
							  m_params			(params)
	{
		query_vfs_iterator	(dir_path, 
							 boost::bind(& query_resources_helper::on_fs_iterator_ready, this, _1),
						  	 params.allocator,
							 (params.flags & query_flag_recursive) ? recursive_true : recursive_false);
	}

private:
	void   add_resources_from_folder (vfs::vfs_iterator								iterator, 
									  buffer_vector<fs_new::virtual_path_string> &	request_pathes,
									  buffer_vector<request> &						requests)
	{
		for ( vfs::vfs_iterator	child_it	=	iterator.children_begin();
								child_it	!=	iterator.children_end();
							  ++child_it )
		{
			if ( !child_it.is_folder() )
			{
				pcstr const name			=	child_it.get_name();
				if ( strings::compare_with_wildcards(m_mask, name) )
				{
					fs_new::virtual_path_string const cur_path	=	child_it.get_virtual_path();
					request_pathes.push_back	(cur_path);
					request	request			=	{ request_pathes.back().c_str(), m_class_id };
					requests.push_back			(request);
				}
			}

			if ( !!(m_params.flags & query_flag_recursive) )
				add_resources_from_folder		(child_it, request_pathes, requests);
		}
	}

	void   on_fs_iterator_ready (vfs::vfs_locked_iterator const & iterator )
	{
		if ( iterator.is_end() )
		{
			u32 const user_thread_id	=	threading::current_thread_id();
			queries_result*	queries		=	(queries_result*)
											XRAY_ALLOC_IMPL(*m_params.allocator, queries_result, 1);

			new (queries) queries_result	(0, 
											 m_params.callback, 
											 m_params.allocator, 
											 user_thread_id, 
											 m_params.parent, 
											 m_params.target_satisfactions,
											 m_params.disable_cache,
											 m_params.quality_indexes,
											 m_params.query_type,
											 m_params.autoselect_quality,
											 m_params.assert_on_fail);

			queries->set_result				(false);
			m_params.callback				(*queries);

			queries->~queries_result		();
			XRAY_FREE_IMPL					(m_params.allocator, queries);
			return;
		}

		u32 const num_children			=	!!(m_params.flags & query_flag_recursive) ? 
											iterator.get_nodes_count() : iterator.get_children_count();

		fs_new::virtual_path_string * requests_pathes_ptr	=	XRAY_ALLOC_IMPL(m_params.allocator, 
																				fs_new::virtual_path_string, 
																				num_children);

		buffer_vector<fs_new::virtual_path_string>	request_pathes(requests_pathes_ptr, num_children);
		buffer_vector<request>				requests(ALLOCA(sizeof(request)*num_children), num_children);

		add_resources_from_folder			(iterator, request_pathes, requests);

		request*	requests_ptr		=	requests.size() ? & requests.front() : NULL;
		query_resources						(requests_ptr, 
											 (u32)requests.size(), 
											 m_params.callback, 
											 m_params.allocator, 
											 NULL,
											 m_params.parent);

		XRAY_FREE_IMPL						(m_params.allocator, requests_pathes_ptr);

		this->~query_resources_helper		();
		query_resources_helper*	this_ptr	=	this;
		XRAY_FREE_IMPL						(m_params.allocator, this_ptr);
	}

private:
	query_resource_params					m_params;
	pcstr									m_mask;
	pcstr									m_dir_path;
	class_id_enum							m_class_id;
}; // query_resources_helper

void   resources_manager::query_resources_by_mask (query_resource_params const & params)
{
	pcstr const 	dir_end				=	strrchr(params.request_mask, fs_new::virtual_path_string::separator);
	fs_new::virtual_path_string				dir_path;
	if ( dir_end )
		dir_path.append						(params.request_mask, dir_end);

	pcstr const		mask				=	dir_end ? dir_end+1 : params.request_mask;
	u32 const		mask_len			=	strings::length(mask);

	query_resources_helper * const helper	=	XRAY_ALLOC_IMPL
												(params.allocator,
												 query_resources_helper, 
												 sizeof(query_resources_helper) + 
												 mask_len + 1 + dir_path.length() + 1);

	pstr			mask_in_helper		=	(pstr)helper + sizeof(query_resources_helper);
	pstr			dir_path_in_helper	=	mask_in_helper + mask_len + 1;

	strings::copy							(mask_in_helper, mask_len+1, mask);
	strings::copy							(dir_path_in_helper, dir_path.length()+1, dir_path.c_str());

	R_ASSERT								(params.requests);
	class_id_enum const class_id		=	params.requests->id;
	new (helper) query_resources_helper		(dir_path_in_helper, mask_in_helper, class_id, params);
}

void   resources_manager::dispatch_callbacks (bool const finalizing_thread)
{
	u32 const thread_id					=	threading::current_thread_id();

	dispatch_devices						();

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( m_watcher.initialized() )
		m_watcher->dispatch_watcher_callbacks	();
#endif

	delete_delayed_unmanaged_resources		();
	deallocate_delayed_unmanaged_resources	();

	thread_local_data* const thread_data =	get_thread_local_data(thread_id, true);
	if ( !thread_data )
		return;
	
	thread_data->dispatching_callbacks	=	true;

	translate_queries						(thread_data->to_translate_query, thread_data->to_translate_query.pop_all_and_clear());

	dispatch_fs_tasks_callbacks				(thread_data->ready_fs_tasks.pop_all_and_clear(),
											 finalizing_thread);

	m_allocate_functionality.tick			(finalizing_thread);

	create_resources						(thread_data->to_create_resource, thread_data->to_create_resource.pop_all_and_clear(), 
											 finalizing_thread);

	dispatch_query_callbacks				(thread_data->finished_queries.pop_all_and_clear(), 
											 finalizing_thread);	

	dispatch_tasks_finished_callback		(thread_data->queries_with_tasks_finished.pop_all_and_clear(),
											 finalizing_thread);

	thread_data->dispatching_callbacks	=	false;
}

void   resources_manager::dispatch_query_callbacks (queries_result * const	ready_query_list, 
												    bool const				finalizing_thread)
{
	queries_result * it_ready_query		=	ready_query_list;

	while ( it_ready_query )
	{
		queries_result * const next_ready_query	=	it_ready_query->m_next_ready;

			it_ready_query->end_and_delete_self		(finalizing_thread);

		it_ready_query					=	next_ready_query;
	}
}

void   resources_manager::dispatch_fs_tasks_callbacks (fs_task *	ready_fs_tasks, 
													   bool const	finalizing_thread)
{
	if ( !ready_fs_tasks )
		return;

	//g_resources_manager->lock_mount_and_fs_iterator_dispatch	();
	fs_task * it_task					=	ready_fs_tasks;
	while ( it_task )
	{
		fs_task * const next_fs_task			=	it_task->m_next;
		
		if ( !finalizing_thread )
			it_task->call_user_callback ();

		memory::base_allocator * const allocator	=	it_task->allocator();
		XRAY_DELETE_IMPL					(allocator, it_task);
		it_task							=	next_fs_task;
	}
}

void   resources_manager::on_added_queries (u32 const num_queries)
{
	for ( u32 i=0; i<num_queries; ++i )
	{
		threading::interlocked_increment	(m_pending_queries_count);
		threading::interlocked_increment	(m_uncooked_queries_count);
	}
}

void   resources_manager::on_dispatched_queries (u32 const num_queries)
{
	for ( u32 i=0; i<num_queries; ++i )
	{
		threading::interlocked_decrement	(m_pending_queries_count);
		// not decrementing here m_uncooked_queries_count
		// they have been decremented earlier
	}
}

void   resources_manager::start_cooks_registration () 
{ 
	threading::interlocked_increment		(m_num_cook_registrators);
}

void   resources_manager::finish_cooks_registration () 
{ 
	if ( threading::interlocked_decrement(m_num_cook_registrators) == 0 )
		wakeup_resources_thread				();
}

void   resources_manager::mount_mounts_path (pcstr const mounts_path)
{
	if ( !mounts_path )
	{
		threading::interlocked_exchange		(m_do_mount_mounts_path, false);
		return;
	}

	m_mounts_path.assign_with_conversion	(mounts_path);
	threading::interlocked_exchange			(m_do_mount_mounts_path, true);
	wakeup_resources_thread					();

	if ( threading::g_debug_single_thread )
		resources::tick						();
}

void   resources_manager::push_to_translate_query (query_result * query)
{
	class_id_enum const class_id		=	query->get_class_id();
	translate_query_cook * const cook	=	cook_base::find_translate_query_cook(class_id);
	R_ASSERT_U								(cook);
	u32 const translate_thread_id		=	query->translate_thread_id();
	if ( translate_thread_id == threading::current_thread_id() )
	{
		translate_query						(query);
		return;
	}
	
	thread_local_data * local_data		=	get_thread_local_data(translate_thread_id, true);
	local_data->to_translate_query.push_back	(query);
	wakeup_thread_by_id_if_needed			(translate_thread_id);
}

template <class queries_list>
void   resources_manager::translate_queries (queries_list const &, query_result * const to_translate_query)
{
	query_result * it_query				=	to_translate_query;
	while ( it_query )
	{
		query_result * next_query		=	queries_list::get_next_of_object(it_query);
		translate_query						(it_query);
		it_query						=	next_query;
	}
}

void   resources_manager::translate_query (query_result * const query)
{
	bool const translated_query			=	query->translate_query_if_needed();
	XRAY_UNREFERENCED_PARAMETER				(translated_query);
	R_ASSERT								(translated_query);
}

void   resources_manager::dispatch_tasks_finished_callback (query_result * query, bool finalizing_thread)
{
	XRAY_UNREFERENCED_PARAMETER				(finalizing_thread);
	query_result * it_query				=	query;
	while ( it_query )
	{
		query_result * const next		=	thread_local_data::queries_with_tasks_finished_list::get_next_of_object(it_query);

		it_query->call_tasks_finished_callback	();

		it_query						=	next;
	}
}

void   resources_manager::dispatch_devices	()
{
	m_hdd.dispatch_callbacks				();
	m_dvd.dispatch_callbacks				();
}

} // namespace resources
} // namespace xray


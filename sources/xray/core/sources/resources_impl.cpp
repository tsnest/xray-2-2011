////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "resources_allocators.h"

namespace xray {
namespace resources {

static out_of_memory_callback	s_out_of_memory_callback;
static resource_freed_callback	s_resource_freed_callback;

query_resource_params::query_resource_params	(request const 				requests[], 
												 creation_request const 	requests_create[], 
												 u32						count, 
												 query_callback const &		callback, 
												 memory::base_allocator *	allocator,
												 float *					target_satisfactions,
												 math::float4x4 const *		transforms[],
												 user_data_variant const *	user_data[],
												 query_result_for_cook *	parent,
												 bool const *				disable_cache,
												 u32 const *				quality_indexes,
												 pcstr						request_mask,
												 query_flag_enum			flags,
												 query_type_enum			query_type,
												 u32 *						out_queries_id,
												 autoselect_quality_bool *	autoselect_quality,
												 assert_on_fail_bool		assert_on_fail) 
:	requests(requests), requests_create(requests_create), requests_count(count),
	callback(callback), allocator(allocator), target_satisfactions(target_satisfactions), transforms(transforms), 
	user_data(user_data), parent(parent), disable_cache(disable_cache), quality_indexes(quality_indexes), 
	request_mask(request_mask), flags(flags), query_type(query_type), out_queries_id(out_queries_id), autoselect_quality(autoselect_quality),
	assert_on_fail(assert_on_fail)
{
}

creation_request::creation_request	( pcstr name, const_buffer data, class_id_enum id ) :
	m_name							( name ),
	m_data							( data ),
	m_id							( id )
{
	R_ASSERT						( m_data, "empty data in creation_request" );
}

creation_request::creation_request	( pcstr name, u32 buffer_size, class_id_enum id ) :
	m_name							( name ),
	m_data							( pbyte( 0 ), buffer_size ),
	m_id							( id )
{
}

void   initialize (fs_new::asynchronous_device_interface & hdd, 
				   fs_new::asynchronous_device_interface & dvd,
				   enable_fs_watcher_bool				   enable_fs_watcher)
{
	XRAY_CONSTRUCT_REFERENCE				(g_resources_manager, resources_manager) (hdd, dvd, enable_fs_watcher);

	threading::yield						(10);
// 	fs::set_allocator_thread_id				(threading::current_thread_id());
// 	memory::g_resources_helper_allocator.user_current_thread_id();
// 	g_resources_manager->get_managed_resource_allocator()->test_defragment();
}

void   mount_mounts_path (pcstr const mounts_path)
{
	g_resources_manager->mount_mounts_path	(mounts_path);
}

bool   is_initialized ()
{
	return									g_resources_manager.initialized();
}

void   finalize ()
{
	XRAY_DESTROY_REFERENCE					(g_resources_manager);
}

void   finalize_thread_usage (bool const calling_from_main_thread)
{
	g_resources_manager->finalize_thread_usage	(calling_from_main_thread);
}

void   wait_and_dispatch_callbacks	(bool const calling_from_main_thread)
{
	g_resources_manager->wait_and_dispatch_callbacks(calling_from_main_thread, false);
}

void   dispatch_callbacks ()
{
	if ( !g_resources_manager.initialized() )
		return;

	if ( threading::g_debug_single_thread )
		resources::tick						( );

	g_resources_manager->dispatch_callbacks	( false );
}

void   start_query_transaction ()
{
	g_resources_manager->start_query_transaction	();
}

void   end_query_might_destroy_this_transaction ()
{
	g_resources_manager->end_query_might_destroy_this_transaction	();
}

long   query_resources (query_resource_params const & params)
{
	return						g_resources_manager->query_resources_impl (params);
}

long   query_resources_autoselect_quality (request const 				requests[], 
										   u32							count, 
										   query_callback const &		callback, 
										   memory::base_allocator *		allocator,
										   math::float4x4 const *		transform[],
										   user_data_variant const *	user_data[],
										   query_result_for_cook *		parent)
{
	autoselect_quality_bool *	autoselect_quality =	(autoselect_quality_bool *)ALLOCA(sizeof(autoselect_quality_bool) * count);
	for ( u32 i=0; i<count; ++i )
		autoselect_quality[i]	=	autoselect_quality_true;

 	query_resource_params	params(requests, NULL, count, callback, allocator, NULL, transform, user_data, parent, NULL, NULL, 0, 0, query_type_normal, 0, autoselect_quality);
 	return						g_resources_manager->query_resources_impl (params);
}

long   query_resource (pcstr const						request_path, 
					   class_id_enum const				class_id, 
					   query_callback const &			callback, 
					   memory::base_allocator * const	allocator, 
					   user_data_variant const *		user_data,
					   query_result_for_cook * const	parent,
					   assert_on_fail_bool				assert_on_fail)
{
	request		request		=	{ request_path, class_id };
	user_data_variant const * user_data_array[]	=	{ user_data };
	return query_resources		(&request, 1, callback, allocator, user_data_array, parent, assert_on_fail);
}

long   query_resources (request const *					requests, 
					    u32 const						request_count, 
					    query_callback const &			callback, 
					    memory::base_allocator * const	allocator,
						user_data_variant const *		user_data[],
						query_result_for_cook * const	parent,
						assert_on_fail_bool				assert_on_fail)
{
	query_resource_params	params(requests, NULL, request_count, callback, allocator, NULL, NULL, user_data, parent);
	params.assert_on_fail	=	assert_on_fail;
	return g_resources_manager->query_resources_impl (params);
}

struct query_resources_and_wait_callback_proxy_pred
{
	query_resources_and_wait_callback_proxy_pred(query_callback const callback) : callback_(callback), receieved_callback_(false) {}
	void callback (queries_result & result)
	{
		callback_				(result);
		receieved_callback_	=	true;
	}

	bool received_callback		() const { return receieved_callback_; }

private:
	bool						receieved_callback_;
	query_callback				callback_;
};

void   query_resources_and_wait	(query_resource_params const & in_params)
{
	query_resources_and_wait_callback_proxy_pred callback_proxy	(in_params.callback);
	query_resource_params params	=	in_params;
	params.callback				=	boost::bind(& query_resources_and_wait_callback_proxy_pred::callback, & callback_proxy, _1);
	query_resources					(params);

	while ( !callback_proxy.received_callback() )
	{
		if ( threading::g_debug_single_thread )
			resources::tick			();

		dispatch_callbacks			();
	}
}

void   query_resources_and_wait	(request const *				requests, 
					    		 u32 const						request_count, 
					    		 query_callback const &			callback, 
					    		 memory::base_allocator * const	allocator,
								 user_data_variant const *		user_data[],
								 query_result_for_cook * const	parent,
								 assert_on_fail_bool			assert_on_fail)
{
	query_resource_params	params	(requests, NULL, request_count, callback, allocator, NULL, NULL, user_data, parent);
	params.assert_on_fail	=	assert_on_fail;
	query_resources_and_wait		(params);
}

void   query_resource_and_wait (pcstr const						request_path, 
					  			class_id_enum const				class_id, 
					  			query_callback const&			callback, 
					  			memory::base_allocator* const	allocator, 
								user_data_variant const *		user_data,
					  			query_result_for_cook* const	parent,
								assert_on_fail_bool				assert_on_fail)
{
	request		request		=	{ request_path, class_id };
	user_data_variant const * user_data_array[]	=	{ user_data };
	query_resources_and_wait	(&request, 1, callback, allocator, user_data_array, parent, assert_on_fail);
}

long   query_create_resource  (pcstr							request_name,
							   const_buffer 					src_data, 
							   class_id_enum const				class_id, 
							   query_callback const &			callback, 
							   memory::base_allocator * const	allocator, 
							   user_data_variant const *		user_data,
							   query_result_for_cook * const	parent,
							   assert_on_fail_bool const		assert_on_fail)
{
	creation_request	request( request_name, src_data, class_id );
	user_data_variant const * user_data_array[]	=	{ user_data };
	return query_create_resources		(& request, 1, callback, allocator, user_data_array, parent, assert_on_fail);
}

long   query_create_resources  (creation_request const *		requests, 
								u32 const						request_count, 
								query_callback const &			callback, 
								memory::base_allocator * const	allocator,
								user_data_variant const *		user_data[],
								query_result_for_cook * const	parent,
								assert_on_fail_bool const		assert_on_fail)
{
	query_resource_params	params(NULL, requests, request_count, callback, allocator, NULL, NULL, user_data, parent);
	params.assert_on_fail				=	assert_on_fail;
	return g_resources_manager->query_resources_impl (params);
}

struct query_create_resources_and_wait_callback_proxy_pred
{
	query_create_resources_and_wait_callback_proxy_pred(query_callback const callback) : callback_(callback), receieved_callback_(false) {}
	void callback (queries_result & result)
	{
		callback_				(result);
		receieved_callback_	=	true;
	}

	bool received_callback		() const { return receieved_callback_; }

private:
	bool						receieved_callback_;
	query_callback				callback_;
};

void   query_create_resources_and_wait  (creation_request const *		requests, 
										 u32 const						request_count, 
										 query_callback const &			callback, 
										 memory::base_allocator * const	allocator,
										 user_data_variant const *		user_data[],
										 query_result_for_cook * const	parent,
										 assert_on_fail_bool const		assert_on_fail)
{
	query_resource_params	params	(NULL, requests, request_count, callback, allocator, NULL, NULL, user_data, parent);
	params.assert_on_fail				=	assert_on_fail;
	query_resources_and_wait				(params);
}

void   query_resources_by_mask (pcstr const						request_mask, 
								class_id_enum const				class_id, 
								query_callback const &			callback, 
								memory::base_allocator * const	allocator,
					    		query_flag_enum const			flags,
								query_result_for_cook * const	parent)
{
	request	request	= { "", class_id };
	query_resource_params	params(& request, NULL, 0, callback, allocator, NULL, NULL, NULL, parent, NULL, NULL, request_mask, flags);
	g_resources_manager->query_resources_by_mask (params);
}

void   tick ()
{
	if ( threading::g_debug_single_thread )
	{
		g_resources_manager->resources_thread_tick	();
		g_resources_manager->cooker_thread_tick		();
	}
}

void   on_resources_thread_started		()
{
	g_resources_manager->on_resources_thread_started	();
}

void   resources_thread_tick					()
{
	g_resources_manager->resources_thread_tick	();
}

void   resources_thread_yield			(u32 thread_sleep_period)
{
	g_resources_manager->resources_thread_yield	(thread_sleep_period);
}

void   on_resources_thread_ending		()
{
	g_resources_manager->on_resources_thread_ending	();
}

void   cooker_thread_tick				()
{
	g_resources_manager->cooker_thread_tick	();
}

void   cooker_thread_yield				(u32 thread_sleep_period)
{
	g_resources_manager->cooker_thread_yield	(thread_sleep_period);
}

void   register_cook (cook_base * const cook)
{
	g_resources_manager->register_cook(cook);
}

cook_base *   unregister_cook (class_id_enum const resource_class)
{
	return	g_resources_manager->unregister_cook(resource_class);
}

void   start_cooks_registration ()
{
	g_resources_manager->start_cooks_registration	();
}

void   finish_cooks_registration ()
{
	g_resources_manager->finish_cooks_registration	();
}

memory::base_allocator *   unmanaged_allocator ()
{
	return						& memory::g_resources_unmanaged_allocator;
}

memory::base_allocator *   helper_allocator ()
{
	return						& memory::g_resources_helper_allocator;
}

pvoid   allocate_unmanaged_memory (u32 size, pcstr type_name)
{
	return						g_resources_manager->allocate_unmanaged_memory (size, type_name);
}

managed_resource *   allocate_managed_resource (u32 size, class_id_enum class_id)
{
	return						g_resources_manager->allocate_managed_resource (size, class_id);
}

void   free_managed_resource (managed_resource * resource)
{
	g_resources_manager->free_managed_resource (resource);
}

void   set_query_finished_callback (query_finished_callback callback)
{
	g_resources_manager->set_query_finished_callback	(callback);
}

void   set_out_of_memory_callback (out_of_memory_callback callback)
{
	s_out_of_memory_callback			=	callback;
}

void   set_resource_freed_callback (resource_freed_callback callback)
{
	s_resource_freed_callback			=	callback;
}

out_of_memory_callback   get_out_of_memory_callback ()
{
	return									s_out_of_memory_callback;
}

resource_freed_callback   get_resource_freed_callback ()
{
	return									s_resource_freed_callback;
}

u32   pending_queries_count ()
{ 
	if ( !g_resources_manager.initialized() )
		return								0;
	return									g_resources_manager->pending_queries_count(); 
}

static threading::atomic32_type	s_resources_thread_started	=	false;
static threading::atomic32_type	s_resources_thread_exit		=	false;
static threading::atomic32_type	s_resources_thread_finished	=	false;

static threading::atomic32_type	s_cooker_thread_started		=	false;
static threading::atomic32_type	s_cooker_thread_exit		=	false;
static threading::atomic32_type	s_cooker_thread_finished	=	false;

static void resources_thread ()
{
	on_resources_thread_started				();
	threading::interlocked_exchange			(s_resources_thread_started, true);

	for (;;) 
	{
		if ( s_resources_thread_exit )
			break;

		resources::resources_thread_yield	( 300 );
		resources::resources_thread_tick	( );
	}
	
	on_resources_thread_ending				();
	threading::interlocked_exchange			(s_resources_thread_finished, true);
}

static void cooker_thread ()
{
	threading::interlocked_exchange			(s_cooker_thread_started, true);
	for (;;) 
	{
		if ( s_cooker_thread_exit )
			break;

		resources::cooker_thread_yield	( 300 );
		resources::cooker_thread_tick	( );
	}
	
	finalize_thread_usage					(false);
	threading::interlocked_exchange			(s_cooker_thread_finished, true);
}

void   start_resources_threads			()
{
	threading::spawn					(
		& resources_thread, 
		"resources manager", 
		"res_man", 
		0,
		2 % threading::core_count(),
		threading::tasks_aware
	);

	while ( !s_resources_thread_started )
		threading::yield					();

	threading::spawn					(
		& cooker_thread, 
		"resources cooker", 
		"res_cook", 
		0,
		2 % threading::core_count(),
		threading::tasks_aware
	);
	
	while ( !s_cooker_thread_started )
		threading::yield					();
}

void   finish_resources_threads			()
{
	resources::finalize_thread_usage		( true );

	threading::interlocked_exchange			( s_resources_thread_exit, true );
	while ( !s_resources_thread_finished )
	{
		resources::dispatch_callbacks		();
		threading::yield					();
	}

	threading::interlocked_exchange			( s_cooker_thread_exit, true );
	while ( !s_cooker_thread_finished )
	{
		resources::dispatch_callbacks		();
		threading::yield					();
	}
}

} // namespace resources
} // namespace xray



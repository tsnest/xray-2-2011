////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_QUERY_RESULT_H_INCLUDED
#define XRAY_RESOURCES_QUERY_RESULT_H_INCLUDED

#include <xray/resources.h>
#include <xray/resources_resource.h>
#include <xray/resources_classes.h>
#include <xray/resources_cook_base.h>
#include <xray/resources_memory_usage.h>
#include <xray/type_variant.h>
#include <xray/vfs_sub_fat_resource.h>
#include <xray/resources_save_generated_data.h>

namespace xray {
namespace resources {

typedef	variant<8*sizeof(pcvoid)>		user_data_variant;

class device_manager;
class queries_result;
class memory_type;

enum query_type_enum			{ query_type_normal, query_type_helper_for_mount, query_type_autoselect_quality };

enum allocation_result_enum		{	allocation_result_failed, 
									allocation_result_success, 
									allocation_result_reallocate_scheduled,
									allocation_result_unset, 					};

enum out_of_memory_type_enum	{	out_of_memory_type_unset, 
									out_of_memory_on_translate_query, 
									out_of_memory_on_create_resource,
									out_of_memory_on_allocate_raw_resource,
									out_of_memory_on_allocate_final_resource	};

class XRAY_CORE_API query_result_for_user : public resource_base
{
public:
	enum error_type_enum		{ 	error_type_unset,
								  	error_type_file_not_found,
								  	error_type_cannot_open_file,
								  	error_type_cannot_read_file,
								  	error_type_cannot_write_file,
								  	error_type_invalid_file_content,
								  	error_type_canceled_by_finalization,
								  	error_type_out_of_memory, 
								  	error_type_cannot_decompress_file,
								  	error_type_hash_not_equal_to_db_hash,
									error_type_cook_not_registered,
									error_type_cook_failed,
									error_type_name_registry_error,
									error_loaded_but_inconsistent_quality	};
public:
									query_result_for_user	();
	virtual						   ~query_result_for_user	();

	bool							is_successful			() const { return (m_error_type == error_type_unset) && (m_create_resource_result != cook_base::result_error); }
	error_type_enum					get_error_type			() const { return m_error_type; }
	
	managed_resource_ptr			get_managed_resource	() const { return m_managed_resource; }
	unmanaged_resource_ptr			get_unmanaged_resource	() const { return m_unmanaged_resource; }
	vfs::vfs_iterator const &		get_result_iterator		() const { return m_result_iterator; }

	const_buffer					creation_data_from_user	() const { return m_creation_data_from_user; }

	save_generated_data *			get_save_generated_data		() const { return m_save_generated_data; }

	pcstr							get_requested_path			() const;
	pcstr							get_primary_requested_path	() const { return m_request_path; }
	bool							is_reused					() const;

	static signalling_bool			convert_logical_to_disk_path (buffer_string * out_disk_path, pcstr logical_path);

protected:
	save_generated_data *			grab_save_generated_data	() { save_generated_data * out = m_save_generated_data; m_save_generated_data = NULL; return out; }

protected:
	const_buffer					m_creation_data_from_user;
	
	managed_resource_ptr			m_managed_resource;
	unmanaged_resource_ptr			m_unmanaged_resource;
	save_generated_data *			m_save_generated_data;
	vfs::vfs_locked_iterator		m_result_iterator; // for iterator resource type

	pstr							m_request_path;
	pstr							m_requery_path;
	error_type_enum					m_error_type;
	cook_base::result_enum			m_create_resource_result;

}; // query_result_for_user

typedef	boost::function< void () >		tasks_finished_callback;
template class XRAY_CORE_API boost::function< void ( ) >;

class XRAY_CORE_API query_result_for_cook : public query_result_for_user
{
public:
									query_result_for_cook	(queries_result * parent);
	virtual						   ~query_result_for_cook	();

	void							set_managed_resource	(managed_resource * ptr) { R_ASSERT(!m_managed_resource); m_managed_resource = ptr; }
	void							set_zero_unmanaged_resource	();
	memory::base_allocator *		get_user_allocator		() const { return m_user_allocator; }
	signalling_bool					convert_logical_to_disk_path (buffer_string * out_disk_path, pcstr logical_path) const;
	
	void							save_generated_resource	(save_generated_data * data);

	void							set_requery_path		(pcstr path_to_requery);
	void							finish_query			(cook_base::result_enum result, assert_on_fail_bool assert_on_cook_failure = assert_on_fail_true);
	void							finish_query			(error_type_enum error_code, assert_on_fail_bool assert_on_cook_failure = assert_on_fail_true);

	u32								get_raw_file_size		() const;
	query_result_for_cook *			get_parent_query		() const;
	void							set_error_type			(error_type_enum error_type, bool check_was_unset = true);
	void							set_unmanaged_resource	(unmanaged_resource_ptr ptr, memory_type const & memory_type, u32 resource_size);
	void							set_unmanaged_resource	(unmanaged_resource_ptr ptr, memory_usage_type const & memory_usage);
	void							set_managed_resource	(managed_resource_ptr ptr) { R_ASSERT(!m_managed_resource); m_managed_resource = ptr; }
	user_data_variant *				user_data				() const { return m_user_data; }
	void							clear_user_data			();
	void							set_out_of_memory		(memory_type & type, u32 size) { m_out_of_memory = memory_usage_type(type, size); }
	void							set_zero_allocation		();

	bool							is_autoselect_quality_query		() const;
	math::float4x4 *				get_transform					() const { return m_transform; }
	void							update_quality_levels_count		();

	virtual float					satisfaction_with				(u32 quality_level) const;

	bool							is_helper_query_for_mount		() const;

	void							set_tasks_finished_callback		(tasks_finished_callback callback) { m_tasks_finished_callback = callback; }
	void							set_pending_tasks_count			(u32 count) { m_pending_tasks_count = count; }
	void							on_task_finished				();
	void							call_tasks_finished_callback	() { R_ASSERT(m_tasks_finished_callback); m_tasks_finished_callback(); }
	assert_on_fail_bool				assert_on_fail					() const;
	memory_usage_type				get_out_of_memory				() const { return m_out_of_memory; }
	out_of_memory_type_enum			get_out_of_memory_type			() const { return m_out_of_memory_type; }

protected:
	virtual void					increase_quality_to_target	(query_result_for_cook * parent_query);
	void							finish_query_impl			(cook_base::result_enum		result, 
																 assert_on_fail_bool const	assert_on_cook_failure,
																 error_type_enum			error_code);

	managed_resource_ptr			get_managed_resource		() const { return m_managed_resource; }
	u32								get_out_of_memory_sub_queries	() const { return m_out_of_memory_sub_queries; }
	void							on_sub_query_out_of_memory	() { threading::interlocked_increment(m_out_of_memory_sub_queries); }

protected:
	tasks_finished_callback			m_tasks_finished_callback;
	threading::atomic32_type		m_pending_tasks_count;

	math::float4x4 *				m_transform;
	out_of_memory_type_enum			m_out_of_memory_type;
	memory_usage_type				m_out_of_memory;
	threading::atomic32_type		m_out_of_memory_sub_queries;
	u8								m_out_of_memory_retries_count;
	u8								m_out_of_memory_reallocations_count;

	memory::base_allocator *		m_user_allocator;
	queries_result *				m_parent;
	user_data_variant *				m_user_data;
	string_path						m_request_path_default_storage;
	u32								m_request_path_max_size;

	friend class					device_manager;
	friend class					resources_manager;
	friend class					query_result;

}; // query_result_for_cook

class query_result;
class XRAY_CORE_API query_result_debug
{
public:
	query_result *	next_in_pending_query_list;
	query_result *	prev_in_pending_query_list;
	query_result *	prev_in_vfs_lock_query_list; 
	query_result *	next_in_vfs_lock_query_list; 

	query_result_debug	() : 
		next_in_pending_query_list(NULL),
		prev_in_pending_query_list(NULL),
		prev_in_vfs_lock_query_list(NULL),
		next_in_vfs_lock_query_list(NULL) {}

};

class XRAY_CORE_API query_result :	
									public	query_result_for_cook,
#if DEBUG_RESOURCES
									public  query_result_debug,
#endif
									public	core::noncopyable
{

public:
									query_result			(u16 flags							=	0,
															 queries_result * parent			=	NULL,
															 memory::base_allocator * allocator	=	NULL,
															 u32 user_thread_id					=	0,
															 float target_satisfaction			=	0,
															 bool disable_cache					=	0,
															 u32 quality_index					=	0,
															 query_type_enum queries_type		=	query_type_normal,
															 autoselect_quality_bool autoselect_quality = autoselect_quality_false);

									~query_result			();

	bool							is_load_type			() const { return has_flag(flag_load); }
	bool							is_save_type			() const { return has_flag(flag_save); }
	bool							is_replication_type		() const { return has_flag(flag_replication); }
	bool							is_compressed			() const;

	u32								quality_index			() const { return m_quality_index; }
	queries_result *				get_owner_queries		() const { return m_parent; }
	fs_new::native_path_string		absolute_physical_path	(assert_on_fail_bool = assert_on_fail_true);

private:
	enum						{	flag_unset									=	1 << 0,
									flag_load									=	1 << 1,
									flag_replication							=	1 << 2,
									flag_save									=	1 << 3,
									flag_locked_fat_iterator					=	1 << 4,
									flag_in_generating_because_no_file_queue	=	1 << 5,
									flag_is_referer								=	1 << 6,	
									flag_was_referer							=	1 << 7,	
									flag_processed_request_path					=	1 << 8,
									flag_finished								=	1 << 9,	
									flag_should_free_request_path				=	1 << 10,
									flag_uses_physical_path						=	1 << 11,
									flag_inplace_in_user_data_for_creation_or_inline_data	=	1 << 12,
									flag_refers_to_raw_file						=	1 << 13,
									flag_translated_query						=	1 << 14,
									flag_zero_unmanaged_resource_was_set		=	1 << 15,
									flag_zero_allocation_was_set				=	1 << 16,
									flag_in_pending_list						=	1 << 17,
									flag_reused_resource						=	1 << 18,
									flag_reused_raw_resource					=	1 << 19,
									flag_in_create_resource						=	1 << 20,
									flag_finished_create_resource				=	1 << 21,
									flag_in_grm_out_of_memory_queue				=	1 << 22,
									flag_name_registry_entry_in_queue			=	1 << 24,
									flag_selected_quality_level					=	1 << 25,
									flag_disable_cache							=	1 << 26,
									flag_is_helper_query_for_mount				=	1 << 27,
									flag_is_autoselect_query					=	1 << 28,
								};

	cook_base::result_enum			get_create_resource_result	() const { return m_create_resource_result; }

	bool							is_finished				() const { return has_flag(flag_finished); }
	bool							append_data_if_needed	(const_buffer data, file_size_type data_pos_in_file);

	allocation_result_enum			allocate_compressed_resource_if_needed		();
	allocation_result_enum			allocate_raw_unmanaged_resource_if_needed	();
	allocation_result_enum			allocate_raw_managed_resource_if_needed		();
	bool							need_create_resource_inplace_in_creation_or_inline_data				();
	void							bind_unmanaged_resource_buffer_to_creation_or_inline_data	();
	allocation_result_enum			allocate_final_unmanaged_resource_if_needed	();
	allocation_result_enum			allocate_final_managed_resource_if_needed	();
	void							free_unmanaged_buffer		 				();
	mutable_buffer					raw_unmanaged_buffer						() const { return m_raw_unmanaged_buffer; }
	managed_resource *				raw_managed_resource						() const { return m_raw_managed_resource.c_ptr(); }

	bool							try_synchronous_cook_from_inline_data	();

	void							clear_reference			();
	void							add_referrer			(query_result *	referrer, bool log_that_referer_query_added = true);
	void							set_flag				(u32 flag);
	bool							has_flag				(u32 flag) const { return (m_flags & flag) == flag; }
	void							unset_flag				(u32 flag);

	bool							check_fat_for_resource_reusage				();

	bool							is_flag					() const;

	void							replicate				();

	// called by device_manager, or when file was taken from fat_it cache
	void							on_file_operation_end	(query_result *	referred = NULL);
	void							on_save_operation_end	();
	void							on_load_operation_end	(query_result *	referred = NULL);
	bool							check_file_crc			();
	// called for referer queries by refered query when its ending
	void							on_refered_query_ended  (query_result * refered_query);

	void							prepare_final_resource	();
	void							on_allocated_final_resource		();
	// sends resource to cook later in right thread
	void							send_to_create_resource		();
	// is called from cooker right thread
	void							do_create_resource						(bool * out_finished_create = NULL);
	void							do_create_resource_impl					();
	void							do_managed_create_resource				(managed_cook * cook);
	void							do_inplace_managed_create_resource		(inplace_managed_cook * cook);
	void							do_unmanaged_create_resource			(unmanaged_cook * cook);
	void							do_inplace_unmanaged_create_resource	(inplace_unmanaged_cook * cook);

	void							do_create_resource_end_part		();
	// is called by resource_manager::dispatch_created_resources or directly if no cooking needed
	void							on_create_resource_end			();
	bool							try_push_created_resource_to_manager_might_destroy_this			();
	void							associate_created_resource_with_fat_or_name_registry	();
	
	// called when query_result is fully ready
	void							end_query_might_destroy_this_impl	();

	void							init_replication			(vfs::vfs_iterator		fat_it, 
																 managed_resource_ptr	resource);
	void							init_save					(save_generated_data *	save_data, 
																 query_result *			data_to_save_generator);

	device_manager *				find_capable_device_manager ();
	u32								get_user_thread_id			() const { return m_user_thread_id; }

	bool							process_request_path		(bool try_sync_way_only);
	void								on_request_iterator_ready	(vfs::vfs_iterator & it, bool try_sync_way_only);

	void							translate_request_path		();

	bool							translate_query_if_needed	();
	bool							is_translate_query			() const;
	bool							is_fs_iterator_query		() const;

	void							on_decompressing_end		();
	bool							file_loaded					() const;

	enum consider_with_name_registry_result_enum {	consider_with_name_registry_result_error,
													consider_with_name_registry_result_no_action,
													consider_with_name_registry_result_added_as_referer,
													consider_with_name_registry_result_added_self_as_host_for_referers,
													consider_with_name_registry_result_got_associated_resource,	};

	enum only_try_to_get_associated_resource_bool { only_try_to_get_associated_resource_false, only_try_to_get_associated_resource_true };
	consider_with_name_registry_result_enum   consider_with_name_registry		(only_try_to_get_associated_resource_bool  only_try_to_get_associated_resource);
	consider_with_name_registry_result_enum   consider_with_name_registry_impl	(pcstr name, only_try_to_get_associated_resource_bool only_try_to_get_associated_resource);

	void							late_set_fat_it						(vfs::vfs_iterator it);
	void							lock_fat_it							();
	void							unlock_fat_it						();

	const_buffer					pin_raw_buffer						();
	void							unpin_raw_buffer					(const_buffer const & pinned_raw_buffer);
	const_buffer					pin_raw_file						();
	void							unpin_raw_file						(const_buffer const & pinned_raw_file);
	const_buffer					pin_compressed_file					();
	void							unpin_compressed_file				(const_buffer const & pinned_compressed_file);
	const_buffer					pin_compressed_or_raw_file	();
	void							unpin_compressed_or_raw_file	(const_buffer const & pinned_file);
	u32								raw_buffer_size						();

	bool							has_uncompressed_inline_data				();
	bool							has_inline_data								();
	bool							need_create_resource_if_no_file						();

	bool							copy_inline_data_to_resource_if_needed		();
	bool							copy_creation_data_to_resource_if_needed	();
	void							copy_data_to_resource						(const_buffer data);
	u32								loaded_bytes								() const { return m_loaded_bytes; }
	void							set_loaded_bytes							(u32 byte_count);
	void							add_loaded_bytes							(u32 byte_count);
	u32								compressed_or_raw_file_size					() const;
	vfs::vfs_iterator 				get_fat_it_zero_if_physical_path_it			() const;
	void							set_creation_source_for_resource 			(unmanaged_resource_ptr resource);
	void							set_creation_source_for_resource 			(managed_resource_ptr resource);
	resource_base::creation_source_enum   creation_source_for_resource			();

	void							set_deleter_object							(unmanaged_resource * resource);
	threading::thread_id_type		allocate_thread_id							() const;
	threading::thread_id_type		translate_thread_id							() const;
	virtual void					recalculate_memory_usage_impl				() { ; } // not used
	void							set_create_resource_result					(cook_base::result_enum result,
																				 error_type_enum		error_type);
	void							finish_normal_query							(cook_base::result_enum create_resource_result);
	void							finish_translated_query						(cook_base::result_enum create_resource_result);

	void							observe_resource_destruction				(resource_base * resource);
	void							on_observed_resource_destroyed				(pcstr resource_request_path);
	bool							retry_action_that_caused_out_of_memory		();
	bool							ready_to_retry_action_that_caused_out_of_memory		();
	void							set_ready_to_retry_action_that_caused_out_of_memory	(bool ready) { m_ready_to_retry_action_that_caused_out_of_memory = ready; }

	virtual log_string_type			log_string									() const;
	resource_base *					resource_for_grm_cache						() const { return m_resource_for_grm_cache; }
	u32								decrease_query_end_guard					();
	void							increase_query_end_guard					();
	bool							end_query_might_destroy_this				();
	void							set_deleter_object_if_needed				(bool * out_is_new_resource = NULL);
	void							propogate_sub_fats_to_resources				();
	template <class Resource>
	void							propogate_sub_fats_to_resource				(Resource & resource);
	static void						set_is_unmovable_if_needed					(managed_resource * resource, bool is_unmovable);
	void							set_result_iterator							(vfs::vfs_locked_iterator const & it);

	void							requery_on_out_of_memory					();
	void							requery										();
	void							prepare_requery								();

public:
	query_result *					m_next_out_of_memory;

private:
	vfs_sub_fat_resource_ptr		m_sub_fat;
	
	union {
		query_result *				m_next_in_device_manager;
		query_result *				m_next_to_create_resource; // used in processed query-s lists 
		query_result *				m_next_generated_to_save;
		query_result *				m_next_decompress;
		query_result *				m_next_to_allocate_raw_resource;
		query_result *				m_next_to_allocate_resource;
		query_result *				m_next_to_translate_query;
		query_result *				m_next_to_init;
		query_result *				m_next_tasks_finished;
		query_result *				m_union_nexts;
	}; // union
	query_result *					m_next_in_generate_if_no_file_queue;
	query_result *					m_prev_in_generate_if_no_file_queue;
		
	query_result *					m_next_referer;
	query_result *					m_data_to_save_generator;

	managed_resource_ptr			m_compressed_resource;	// may be NULL
	managed_resource_ptr			m_raw_managed_resource; 
	mutable_buffer					m_raw_unmanaged_buffer; 
	mutable_buffer					m_unmanaged_buffer; 
	name_registry_entry				m_name_registry_entry;

	u32								m_offset_to_file;
	u32								m_loaded_bytes;
	u32								m_quality_index;

	threading::atomic32_type		m_query_end_guard;
	threading::atomic32_type		m_flags;
	threading::atomic32_type		m_observed_resource_destructions_left;
	u32								m_final_resource_size;
	u32								m_user_thread_id;
	resource_base *					m_resource_for_grm_cache;
	bool							m_ready_to_retry_action_that_caused_out_of_memory;

	threading::atomic32_type		m_on_created_resource_guard;


	friend class					device_manager;
	friend class					queries_result;
	friend class					resources_manager;
	friend class					thread_local_data;
	friend class					cook;
	friend class					unmanaged_resource;
	friend class					query_result_for_cook;
	friend class					query_result_for_user;
	friend class					game_resources_manager;
	friend class					resource_base;
	friend class					resource_freeing_functionality;
	friend class					hdd_manager;
	friend class					allocate_functionality;
};

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_QUERY_RESULT_H_INCLUDED

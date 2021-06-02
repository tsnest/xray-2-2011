////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/resources_query_result.h>
#include <xray/resources_queries_result.h>
#include <xray/resources_cook_classes.h>
#include <xray/fs/path_string_utils.h>
#include <xray/managed_allocator.h>
#include "resources_manager.h"
#include "resources_device_manager.h"
#include "game_resman_test_resource.h"
#include "game_resman_test_utils.h"

static xray::command_line::key s_assert_on_resource_failure( "assert_on_resource_failure", "assert_on_resource_failure", "resources", "asserts when resources loading or cooking failed", "" );

namespace xray {
namespace resources {

query_result_for_cook *   query_result_for_cook::get_parent_query () const 
{ 
	return									m_parent->get_parent_query(); 
}

void   query_result::add_referrer (query_result* const referer, bool log_that_referer_query_added)
{
	referer->free_unmanaged_buffer			();

	ASSERT									( !has_flag(flag_is_referer) );
	ASSERT									( !referer->has_flag(flag_is_referer) );
	if ( log_that_referer_query_added )
		LOGI_INFO							("resources:manager", "referer query: '%s' [quid_ref %d, quid_host %d]", 
											 m_request_path, referer->uid(), uid());

	referer->set_flag						(flag_is_referer | flag_was_referer);
	
	referer->m_next_referer				=	m_next_referer;
	m_next_referer						=	referer;
}

bool   query_result::append_data_if_needed (const_buffer data, file_size_type data_pos_in_file)
{
	R_ASSERT								(!m_fat_it.is_end());

	file_size_type const file_start		=	m_fat_it.get_file_offs();
	file_size_type const file_end		=	file_start + compressed_or_raw_file_size();
	file_size_type const data_start		=	data_pos_in_file;
	file_size_type const data_end		=	data_pos_in_file + data.size();

	if ( file_end <= data_start || data_end <= file_start )
		return false;

	file_size_type dest_start			=	file_start;
	file_size_type src_start			=	data_start;
	if ( file_start < data_start )
		dest_start						=	data_start;
	else
		src_start						=	file_start;		

	file_size_type dest_end				=	file_end;
	file_size_type src_end				=	data_end;
	if ( file_end < data_end )
		src_end							=	file_end;
	else
		dest_end						=	data_end;

	mutable_buffer file_data			=	cast_away_const(pin_compressed_or_raw_file());
	u32 const dest_offs					=	u32(dest_start - file_start);
	u32 const dest_size					=	u32(dest_end - dest_start);
	u32 const src_offs					=	u32(src_start - data_start);
	u32 const src_size					=	u32(src_end - src_start);

	R_ASSERT								(dest_offs == m_loaded_bytes);
	R_ASSERT								(dest_size == src_size);

	mutable_buffer const dest_data		=	file_data.slice(dest_offs, dest_size);
	const_buffer const src_data			=	data.slice(src_offs, src_size);

	memory::copy							(dest_data, src_data);
	m_loaded_bytes						+=	dest_size;

	unpin_compressed_or_raw_file			(file_data);
	return									true;
}

bool   query_result::is_compressed		() const
{
	ASSERT									(!m_fat_it.is_end());
	return									m_fat_it.is_compressed();
}

bool   query_result::check_file_crc		()
{
	if ( !m_fat_it.is_archive() )
		return								true;
	
	const_buffer const pinned_file		=	pin_compressed_or_raw_file();

	u32 const	raw_file_size			=	m_fat_it.get_raw_file_size();
	u32 const	raw_file_hash			=	fs_new::crc32((pcstr)pinned_file.c_ptr(), raw_file_size);

	u32	const	hash_in_fat				=	m_fat_it.get_file_hash();

	R_ASSERT								(raw_file_hash == hash_in_fat);

	unpin_compressed_or_raw_file			(pinned_file);

	return									raw_file_hash == hash_in_fat;
}

device_manager *   query_result::find_capable_device_manager ()
{
	fs_new::native_path_string				file_path;
	if ( !m_fat_it.is_end() )
		file_path						=	m_fat_it.get_physical_path();

	device_manager * const manager		=	g_resources_manager->find_capable_device_manager(file_path.c_str());
	R_ASSERT( manager, "No device manager can process path: '%s'", file_path.c_str() );
	return									manager;
}

bool   query_result::translate_query_if_needed ()
{
	R_ASSERT								(!has_flag(flag_translated_query));
	if ( !is_translate_query() )	
		return								false;

	translate_query_cook * const cook	=	cook_base::find_translate_query_cook(m_class_id);
	u32 const translate_query_thread	=	translate_thread_id();
	if ( translate_query_thread != threading::current_thread_id() )
		return								false;

	if ( cook->reuse_type() == cook_base::reuse_true && strings::length(m_request_path) > 0 )
	{
		consider_with_name_registry_result_enum const result	=	consider_with_name_registry(only_try_to_get_associated_resource_false);

		if ( result == consider_with_name_registry_result_got_associated_resource )
		{
			end_query_might_destroy_this	();
			return							true;
		}
		else if ( result == consider_with_name_registry_result_error )
		{
			end_query_might_destroy_this	();
			return							true;
		}
		else if ( result == consider_with_name_registry_result_added_as_referer )
			return							true;
	}

	threading::interlocked_increment		(m_on_created_resource_guard);
	
	thread_local_data * const tls		=	g_resources_manager->get_thread_local_data(threading::current_thread_id(), true);
	
	++tls->in_translate_query_counter;
	
		cook->translate_query					(* this);

	--tls->in_translate_query_counter;


		bool const is_out_of_memory			=	(get_error_type() == error_type_out_of_memory);
		set_flag								(flag_translated_query);
	
	try_push_created_resource_to_manager_might_destroy_this	();

	return										!is_out_of_memory;
}

void   query_result_for_cook::set_error_type (error_type_enum error_type, bool check_was_unset)
{ 
	if ( check_was_unset )
		R_ASSERT							(get_error_type() == error_type_unset);

	m_error_type						=	error_type; 
}

void   query_result_for_cook::set_unmanaged_resource (unmanaged_resource_ptr	ptr, 
													  memory_type const &		memory_type, 
													  u32						resource_size)
{
	m_unmanaged_resource				=	ptr;
	if ( ptr )
	{
		R_ASSERT							(resource_size, "you cannot pass 0 value as a resource size");
		ptr->set_memory_usage				(memory_usage_type(& memory_type, resource_size));
	}
}

void   query_result_for_cook::set_unmanaged_resource (unmanaged_resource_ptr ptr, memory_usage_type const & memory_usage)
{
	set_unmanaged_resource					(ptr, * memory_usage.type, memory_usage.size);
}

const_buffer   query_result::pin_compressed_file ()
{
	if ( m_compressed_resource )
		return								memory::buffer(m_compressed_resource->pin(), m_compressed_resource->get_size());

	R_ASSERT								(has_inline_data());
	R_ASSERT								(m_fat_it.is_compressed());
	const_buffer							inline_data;
	m_fat_it.get_inline_data				(& inline_data);
	return									inline_data;
}

void   query_result_for_cook::on_task_finished ()
{
	R_ASSERT								(m_tasks_finished_callback, "you forgot to call set_tasks_finished_callback!");
	R_ASSERT								(m_pending_tasks_count != 0, "you probably forgot to call set_pending_tasks_count");
	if ( threading::interlocked_decrement(m_pending_tasks_count) == 0 )
		g_resources_manager->push_to_call_tasks_finished_callback	(static_cast_checked<query_result *>(this));
}

assert_on_fail_bool   query_result_for_cook::assert_on_fail	() const 
{ 
	return									(assert_on_fail_bool)(!m_parent || m_parent->assert_on_fail()); 
}

void   query_result::unpin_compressed_file (const_buffer const & pinned_compressed_data)
{
	if ( m_compressed_resource )
		m_compressed_resource->unpin		((pcbyte)pinned_compressed_data.c_ptr());
}

const_buffer   query_result::pin_raw_buffer ()
{
	if ( m_raw_managed_resource )
		return								memory::buffer(m_raw_managed_resource->pin(), m_raw_managed_resource->get_size());
	else if ( m_creation_data_from_user )
		return								m_creation_data_from_user;
	else if ( m_raw_unmanaged_buffer )
		return								m_raw_unmanaged_buffer;

	R_ASSERT								(has_uncompressed_inline_data());
	const_buffer							inline_data;
	m_fat_it.get_inline_data				(& inline_data);
	return									inline_data;
}

void   query_result::unpin_raw_buffer (const_buffer const & pinned_raw_buffer)
{
	if ( m_raw_managed_resource )
		m_raw_managed_resource->unpin		((pcbyte)pinned_raw_buffer.c_ptr());
}

const_buffer   query_result::pin_raw_file ()
{
	const_buffer	out_buffer			=	pin_raw_buffer() + m_offset_to_file;
	u32 const raw_file_size				=	get_raw_file_size();
	ASSERT_CMP								(out_buffer.size(), >=, raw_file_size);
	out_buffer							=	const_buffer(out_buffer.c_ptr(), raw_file_size);
	return									out_buffer;
}

void   query_result::unpin_raw_file (const_buffer const & pinned_raw_file)
{
	unpin_raw_buffer						(memory::buffer((pcstr)pinned_raw_file.c_ptr() - m_offset_to_file, 
											 pinned_raw_file.size() + m_offset_to_file));
}

const_buffer   query_result::pin_compressed_or_raw_file ()
{
	return									(!m_fat_it.is_end() && m_fat_it.is_compressed()) ? 
											pin_compressed_file() : pin_raw_file();
}

void   query_result::unpin_compressed_or_raw_file (const_buffer const & pinned_file)
{
	if ( !m_fat_it.is_end() && m_fat_it.is_compressed() )
		unpin_compressed_file				(pinned_file);
	else
		unpin_raw_file						(pinned_file);
}

u32   query_result::raw_buffer_size ()
{
	const_buffer const raw_buffer		=	pin_raw_buffer();
	u32 const out_size					=	raw_buffer.size();
	unpin_raw_buffer						(raw_buffer);
	return									out_size;
}

bool   query_result::has_uncompressed_inline_data ()
{
	return									!m_fat_it.is_end() && 
											 m_fat_it.data_node()->is_inlined() && 
											!m_fat_it.is_compressed();
}

bool   query_result::has_inline_data ()
{
	return									!m_fat_it.is_end() && m_fat_it.data_node()->is_inlined();
}

bool   query_result::need_create_resource_if_no_file ()
{
	return									m_fat_it.is_end() && !creation_data_from_user() && !has_flag(flag_refers_to_raw_file);
}

u32   query_result::compressed_or_raw_file_size () const
{
	R_ASSERT								(!m_fat_it.is_end()); 
	return									m_fat_it.get_raw_file_size();
}

u32	  query_result_for_cook::get_raw_file_size () const
{
	R_ASSERT								(!m_fat_it.is_end() || creation_data_from_user());
	if ( !m_fat_it.is_end() )
		return								m_fat_it.get_file_size();

	return									creation_data_from_user().size();
}

vfs::vfs_iterator   query_result::get_fat_it_zero_if_physical_path_it () const
{
	if ( has_flag(flag_uses_physical_path) )
		return								vfs::vfs_iterator::end();

	return									m_fat_it;	
}

void   query_result::set_loaded_bytes (u32 byte_count)
{
	m_loaded_bytes						=	byte_count;
	R_ASSERT								(m_loaded_bytes <= compressed_or_raw_file_size());
}

void   query_result::add_loaded_bytes (u32 byte_count)
{
	m_loaded_bytes						+=	byte_count;
	R_ASSERT								(m_loaded_bytes <= compressed_or_raw_file_size());
}

bool   query_result::file_loaded () const
{
	return									m_loaded_bytes == compressed_or_raw_file_size();
}

void   query_result_for_cook::set_zero_unmanaged_resource ()
{
	query_result * const this_ptr		=	static_cast_checked<query_result *>(this);
	this_ptr->set_flag						(query_result::flag_zero_unmanaged_resource_was_set);
}

void   query_result::set_flag (u32 flag) 
{ 
	threading::interlocked_or				(m_flags, flag); 
}

void   query_result::unset_flag (u32 flag) 
{ 
	threading::interlocked_and				(m_flags, ~flag); 
}

void   query_result::observe_resource_destruction (resource_base * resource)
{
	threading::interlocked_increment		(m_observed_resource_destructions_left);

// 	LOGI_DEBUG								("grm", "+ %s observes %s (%d observed now)", 
// 											 resources::log_string(this).c_str(), 
// 											 resources::log_string(resource).c_str(), 
// 											 m_observed_resource_destructions_left);

	resource->set_destruction_observer		(this);
}

void   query_result::on_observed_resource_destroyed (pcstr resource_request_path)
{
	XRAY_UNREFERENCED_PARAMETER				(resource_request_path);
	ASSERT_CMP								(m_observed_resource_destructions_left, >, 0);
	threading::interlocked_decrement		(m_observed_resource_destructions_left);

	XRAY_UNREFERENCED_PARAMETER				(resource_request_path);

// 	LOGI_DEBUG								("grm", "- %s observes %s (%d observed now)", 
// 											 resources::log_string(this).c_str(), 
// 											 resource_request_path ? resource_request_path : "", 
// 											 m_observed_resource_destructions_left);
}

void   query_result_for_cook::set_zero_allocation ()
{
	query_result * const this_ptr		=	static_cast_checked<query_result *>(this);
	this_ptr->set_flag						(query_result::flag_zero_allocation_was_set);
}

bool   query_result_for_user::is_reused	() const
{
	query_result const * const this_ptr		=	static_cast_checked<query_result const *>(this);
	return									this_ptr->has_flag(query_result::flag_reused_resource);
}

pcstr   query_result_for_user::get_requested_path () const
{
	if ( m_requery_path )
		return								m_requery_path;
	return									m_request_path;
}

void   query_result_for_cook::update_quality_levels_count ()
{
	ASSERT_CMP								(m_quality_levels_count, ==, 1);
	cook_base * const cook				=	cook_base::find_cook(get_class_id());
	m_quality_levels_count				=	cook->calculate_quality_levels_count(this);
}

float   query_result_for_cook::satisfaction_with (u32 quality_level) const
{
	ASSERT_CMP								(m_quality_levels_count, !=, 1);
	ASSERT									(is_autoselect_quality_query());
	cook_base * const cook				=	cook_base::find_cook(get_class_id());
	math::float4x4 * transform			=	get_transform();
	R_ASSERT								(transform);
	return									cook->satisfaction_with(quality_level, * transform, 1);
}

bool   query_result_for_cook::is_helper_query_for_mount	() const
{
	query_result const * const this_ptr	=	static_cast_checked<query_result const *>(this);
	return									this_ptr->has_flag(query_result::flag_is_helper_query_for_mount);
}

bool   query_result_for_cook::is_autoselect_quality_query () const 
{ 
	query_result const * const this_ptr	=	static_cast_checked<query_result const *>(this);
	if ( this_ptr->has_flag(query_result::flag_is_autoselect_query) )
	{
		R_ASSERT							(m_transform != NULL);
		return								true;
	}

	return									false;
}

void   remove_autoselect_quality_query		(query_result * query);

void   query_result_for_cook::increase_quality_to_target (query_result_for_cook * parent_query) 
{
	R_ASSERT_U								(this == parent_query);
	query_result * const this_ptr		=	static_cast_checked<query_result*>(this);
	remove_autoselect_quality_query			(this_ptr);
	this_ptr->set_flag						(query_result::flag_selected_quality_level);
	g_resources_manager->push_new_query		(this_ptr);
}

void   query_result::set_result_iterator	(vfs::vfs_locked_iterator const & it) 
{ 
	m_result_iterator.grab					(it); 
	set_flag								(flag_finished);

	unset_flag								(flag_in_pending_list);

	g_resources_manager->debug()->unregister_pending_query	(this);
	
	if ( m_result_iterator )
		g_resources_manager->debug()->register_vfs_lock_query	(this);
}

log_string_type	  query_result::log_string	() const 
{ 
	log_string_type							out_string; 
	out_string.assignf						("'%s' [quid %d][class %d]", get_requested_path(), uid(), m_class_id);
	return									out_string;
}

} // namespace resources
} // namespace xray

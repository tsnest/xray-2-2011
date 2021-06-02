////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace fs_new {

bool   asynchronous_device_interface::query_open_file (native_path_string const &			physical_path,
													   open_file_params const &				open_file_params,
													   query_open_file_callback const &		callback,
													   memory::base_allocator * const		allocator, 
													   threading::event *					event_to_fire_after_execute)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		open_file_device_query	synchronous_query	(physical_path, open_file_params, callback, allocator, m_device, event_to_fire_after_execute);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return										true;
	}

	open_file_device_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, open_file_device_query)
														(physical_path, open_file_params, callback, allocator, m_device);
	if ( !new_query )
		return								false;

	push_query								(new_query);
	return									true;
}

bool   asynchronous_device_interface::query_close_file (file_type * file, memory::base_allocator * allocator, threading::event * event_to_fire_after_execute)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		close_file_device_query	synchronous_query	(file, allocator, m_device);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return								true;
	}

	close_file_device_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, close_file_device_query)(file, allocator, m_device, event_to_fire_after_execute);
	if ( !new_query )
		return								false;

	push_query								(new_query);
	return									true;
}

bool   asynchronous_device_interface::query_write_file (query_write_file_args const & args,
														memory::base_allocator * allocator)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		write_file_device_query	synchronous_query	(args, allocator, m_device);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return								true;
	}

	write_file_device_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, write_file_device_query)(args, allocator, m_device);
	if ( !new_query )
		return								false;
	push_query								(new_query);
	return									true;
}

bool   asynchronous_device_interface::query_read_file (query_read_file_args const & args,
													   memory::base_allocator * const allocator)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		read_file_device_query	synchronous_query	(args, allocator, m_device);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return								true;
	}

	read_file_device_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, read_file_device_query)(args, allocator, m_device);
	if ( !new_query )
		return								false;

	push_query								(new_query);
	return									true;
}

bool   asynchronous_device_interface::query_physical_path_info (native_path_string const & native_physical_path,
															    query_physical_path_info_callback const & callback,
																memory::base_allocator * allocator,
																threading::event *	event_to_fire_after_execute)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		query_physical_path_device_query	synchronous_query	(native_physical_path, callback, allocator, m_device, event_to_fire_after_execute);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return								true;
	}

	query_physical_path_device_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, query_physical_path_device_query)
													(native_physical_path, callback, allocator, m_device, event_to_fire_after_execute);
	if ( !new_query )
		return								false;

	push_query								(new_query);
	return									true;
}

bool   asynchronous_device_interface::query_custom_operation (query_custom_operation_args const & args,
 															  memory::base_allocator *	allocator)
{
	if ( m_device_mode == device_mode_synchronous )
	{
		custom_operation_query	synchronous_query	(args, allocator, m_device);
		synchronous_query.execute					();
		synchronous_query.callback_if_needed		();
		return								true;
	}

	custom_operation_query * new_query	=	XRAY_NEW_WITH_CHECK_IMPL(allocator, new_query, custom_operation_query)
													(args, allocator, m_device);
	if ( !new_query )
		return								false;

	push_query								(new_query);	
	return									true;
}

} // namespace fs_new
} // namespace xray

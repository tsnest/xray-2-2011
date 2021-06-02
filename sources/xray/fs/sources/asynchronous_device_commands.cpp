////////////////////////////////////////////////////////////////////////////
//	Created		: 07.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/asynchronous_device_commands.h>
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/asynchronous_device_interface.h>

namespace xray {
namespace fs_new {

void   synchronize_device_query::execute () 
{
	m_device->set_synchronous_thread_id		(m_synchronous_thread_id);
	m_synchronization_started_event.set		(true);
	m_synchronization_ended_event.wait		(threading::wait_time_infinite);
}

void   synchronize_device_query::wait_synchronization ()
{
	m_synchronization_started_event.wait	(threading::wait_time_infinite);
}

void   synchronize_device_query::on_synchronized_ended ()
{
	R_ASSERT_CMP							(m_device->get_synchronous_thread_id(), ==, m_synchronous_thread_id);
	m_device->set_synchronous_thread_id		(u32(-1));
	m_synchronization_ended_event.set		(true);
}

void   open_file_device_query::execute	()
{
	bool const open_file_result			=	m_device.open(& m_result_file, m_physical_path, m_open_file_params);
	XRAY_UNREFERENCED_PARAMETER				(open_file_result);
}

void   open_file_device_query::callback_if_needed ()
{
	R_ASSERT								(m_callback);
	m_callback								(m_result_file);
}

void   close_file_device_query::execute ()
{
	m_device.close						(m_file);
}

void   write_file_device_query::execute ()
{
	m_result							=	m_device.write(m_args.file, m_args.data, m_args.size) == m_args.size;
}

void   write_file_device_query::callback_if_needed ()
{
	if ( m_args.callback )
		m_args.callback						(m_result);
}

void   read_file_device_query::execute ()
{
	m_result							=	m_device.read(m_args.file, m_args.data, m_args.size) == m_args.size;
}

void   read_file_device_query::callback_if_needed ()
{
	if ( m_args.callback )
		m_args.callback						(m_result);
}

void   query_physical_path_device_query::execute ()
{
	m_result							=	m_device.get_physical_path_info(m_path);
}

void   query_physical_path_device_query::callback_if_needed ()
{
	if ( m_callback )
		m_callback							(m_result);
}

void   custom_operation_query::execute ()
{
	synchronous_device_interface	device	(m_device);
	m_result							=	m_args.custom_operation(device);
}

void   custom_operation_query::callback_if_needed ()
{
	if ( m_args.callback )
		m_args.callback						(m_result);
}

} // namespace fs_new
} // namespace xray
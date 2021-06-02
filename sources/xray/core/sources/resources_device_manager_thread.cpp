////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/device_utils.h>
#include "resources_device_manager.h"
#include "resources_allocators.h"
#include "resources_manager.h"

namespace xray {
namespace resources {

bool   device_manager::process_write_query (fs_new::file_type *								file,
											query_result *									query, 
											fs_new::synchronous_device_interface const &	device)
{
	const_buffer	src_data;
	file_size_type	file_pos			=	0;
	bool const	is_save_query			=	query->has_flag(query_result::flag_save);

	uninitialized_reference< pinned_ptr_const<u8> >	resource_to_save;

	if ( is_save_query )
	{
		save_generated_data * const data	=	query->get_save_generated_data();

		if ( data->get_data() )
			src_data					=	data->get_data();
		else
		{
			managed_resource_ptr resource	=	data->get_resource();
			R_ASSERT						(resource);
			XRAY_CONSTRUCT_REFERENCE		(resource_to_save, pinned_ptr_const<u8>)(resource);

			src_data					=	* resource_to_save;
		}
		R_ASSERT							(src_data);
	}
	else
	{
		vfs::vfs_iterator const fat_it	=	query->get_fat_it();
		file_pos						=	fat_it.get_file_offs();
		src_data						=	query->pin_compressed_or_raw_file();
	}

	bool const out_result				=	do_async_operation(file, query, device, cast_away_const(src_data), file_pos, false);

	if ( !is_save_query )
		query->unpin_compressed_or_raw_file	(src_data);

	if ( resource_to_save.initialized() )
		XRAY_DESTROY_REFERENCE				(resource_to_save);

	return									out_result;
}

bool   device_manager::process_read_query (fs_new::file_type *							file,
										   query_result *								query, 
										   fs_new::synchronous_device_interface const &	device)
{
	vfs::vfs_iterator fat_it			=	query->get_fat_it();
	file_size_type const file_pos		=	fat_it.get_file_offs();
	mutable_buffer const file_data		=	cast_away_const(query->pin_compressed_or_raw_file());
	bool const result					=	do_async_operation(file, query, device, file_data, file_pos, false);

	if ( result )
		query->add_loaded_bytes				(file_data.size());

	R_ASSERT								(query->file_loaded());
	return									result;
}

bool   device_manager::process_query	(query_result * query, fs_new::synchronous_device_interface const & device)
{
	fs_new::file_type * file			=	NULL;

	if ( !open_file(& file, query, device) )
		return								false;

	bool const operation_result			=	query->is_load_type() ? 
												process_read_query	(file, query, device) : 
												process_write_query	(file, query, device);

 	LOGI_DEBUG								("resources:device_manager", "async %s %s %s",
											 query->is_load_type() ? "read" : "write",
											 detail::make_query_logging_string (query).c_str(),
											 operation_result ? "successfull" : "failed");

	device->close							(file);
	return									true;
}

bool   device_manager::open_file		(fs_new::file_type * *							out_file, 
										 query_result *									query, 
										 fs_new::synchronous_device_interface const &	device)
{
	ASSERT									(!m_file_handle);

	if ( !query->is_save_type() )
	{
		vfs::vfs_iterator const it		=	query->get_fat_it();
		if ( it.is_end() || it.is_folder() )
		{
			query->set_error_type			(query_result::error_type_file_not_found);
			return							false;
		}
	}

	fs_new::native_path_string const &	native_path		=	query->absolute_physical_path();
	
	fs_new::file_mode::mode_enum			mode; 
	fs_new::file_access::access_enum		access;

	if ( query->is_load_type() )
	{
		mode							=	fs_new::file_mode::open_existing;
		access							=	fs_new::file_access::read;
	}
	else
	{
		fs_new::create_folder_r				(device, native_path, false);

		mode							=	fs_new::file_mode::create_always;
		access							=	fs_new::file_access::write;
	}

	bool const opened_file				=	device->open	(out_file, native_path, mode, access, 
											 				 assert_on_fail_false, fs_new::notify_watcher_true, 
											 				 fs_new::use_buffering_true);

	LOGI_INFO								("resources:device_manager", "%s opened %s for %s", 
											opened_file ? "successfull" : "failed",
											native_path.c_str(),
											query->is_load_type() ? "read" : "write");

	if ( m_last_file_name != native_path )
	{
		m_sector_data_last_file_pos		=	(file_size_type) -1;
		m_last_file_name				=	native_path;
	}
	
	if ( !opened_file )
	{
		query->set_error_type				(query_result::error_type_cannot_open_file);
		return								false;
	}

	return									true;
}
 
bool   device_manager::do_async_operation (fs_new::file_type *							file, 
										   query_result *								query,
										   fs_new::synchronous_device_interface const &	device,
										   mutable_buffer								out_data, 
										   file_size_type const							file_pos, 
										   bool											sector_aligned)
{
	XRAY_UNREFERENCED_PARAMETER				(sector_aligned);

	device->seek							(file, file_pos, fs_new::seek_file_begin);
	file_size_type const bytes_done		=	query->is_load_type() ? 
												device->read	(file, out_data.c_ptr(), out_data.size()) :
												device->write	(file, out_data.c_ptr(), out_data.size());

	bool const result					=	(bytes_done == out_data.size());

	if ( !result )
	{
		query->set_error_type				(query->is_load_type() ? 
												query_result::error_type_cannot_read_file :
												query_result::error_type_cannot_write_file);
		return								false;
	}

	return									true;
}

} // namespace resources
} // namespace xray

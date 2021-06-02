////////////////////////////////////////////////////////////////////////////
//	Created		: 20.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/unpack.h>
#include <xray/fs/device_utils.h>
#include <xray/memory_allocated_buffer.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/vfs/virtual_file_system.h>
#include "branch_locks.h"

namespace xray {
namespace vfs  {

using namespace fs_new;

bool   unpack_node						(base_node<> * node, virtual_file_system & file_system, unpack_arguments & args);

bool   unpack							(virtual_file_system & file_system, unpack_arguments & args)
{
	base_node<> * start_node			=	NULL;
	file_system.hashset.find_and_lock_branch	(start_node, args.source_path.c_str(), lock_type_read);
	if ( !start_node )
	{
		LOGI_ERROR							("unpack", args.log_format, args.log_flags, "nothing mounted on virtual path: '%s'", args.source_path.c_str());
		return								false;
	}

	native_path_string						absolute_path;
	if ( !convert_to_absolute_path(& absolute_path, args.target_path, assert_on_fail_true) )
	{
		LOGI_ERROR							("unpack", args.log_format, args.log_flags, "cannot convert to absolute path: '%s'", args.target_path.c_str());
		return								false;
	}

	args.target_path					=	absolute_path;

	if ( !create_folder_r(args.device, args.target_path, true) )
	{
		LOGI_ERROR							("unpack", args.log_format, args.log_flags, "cannot create folder: '%s'", args.target_path.c_str());
		return								false;
	}

	args.nodes_count					=	calculate_count_of_nodes(start_node);
	args.unpacked_nodes_count			=	0;

	bool const result					=	unpack_node(start_node, file_system, args);
	unlock_branch							(start_node, lock_type_read);
	return									result;
}

result_enum   decompress_node			(base_node<> *					node, 
										 allocated_buffer *				uncompressed_data, 
										 synchronous_device_interface &	device,
										 memory::base_allocator *		allocator,
										 compressor *					compressor)
{
	R_ASSERT								(node->is_compressed());

	u32 const compressed_file_size		=	get_compressed_file_size(node);

	allocated_buffer	compressed_data		(compressed_file_size, allocator, "compressed_file_data");
	if ( !compressed_data )
		return								result_out_of_memory;

	u32 const uncompressed_file_size	=	get_file_size(node);

	uncompressed_data->assign				(uncompressed_file_size, allocator, "uncompressed_file_data");
	if ( !* uncompressed_data )
		return								result_out_of_memory;
	
	native_path_string const source_file_path	=	get_node_physical_path(node);

	file_type_pointer	source_file			(source_file_path, device, file_mode::open_existing, file_access::read);
	if ( !source_file )
		return								result_fail;

	file_size_type const file_offset	=	get_file_offs(node);
	device->seek							(source_file, file_offset, seek_file_begin);
	file_size_type const bytes_read		=	device->read(source_file, compressed_data.c_ptr(), compressed_file_size);
	CURE_ASSERT								(bytes_read == compressed_file_size, return result_fail);

	u32 result_size						=	0;
	compressor->decompress					(compressed_data, * uncompressed_data, result_size);
	R_ASSERT								(result_size == uncompressed_file_size);

	return									result_success;
}

bool   unpack_node						(base_node<> * node, virtual_file_system & file_system, unpack_arguments & args)
{
	u32 const saved_path_length			=	args.target_path.length();
	
	if ( strings::length(node->get_name()) != 0 )
		args.target_path.appendf			("%c%s", native_path_string::separator, node->get_name());

	bool const need_unpack				=	args.filter_callback ? args.filter_callback(node) : true;

	bool out_result						=	true;
	if ( node->is_folder() )
	{
		if ( need_unpack )
		if ( !create_folder_r(args.device, args.target_path, true) )
		{
			LOGI_ERROR						("unpack", args.log_format, args.log_flags, "cannot create folder: '%s'", args.target_path.c_str());
			return							false;
		}

		base_folder_node<> * const folder	=	node_cast<base_folder_node>(node);
		for ( base_node<> * it_child	=	folder->get_first_child();
							it_child;
							it_child	=	it_child->get_next() )
		{
			out_result					&=	unpack_node	(it_child, file_system, args);
		}
	}
	else if ( need_unpack )
	{
		args.device->erase					(args.target_path);
		if ( !create_folder_r(args.device, args.target_path, false) )
		{
			LOGI_ERROR						("unpack", args.log_format, args.log_flags, "cannot create folder: '%s'", args.target_path.c_str());
			return							false;
		}

		file_type_pointer	target_file		(args.target_path, args.device, file_mode::create_always, file_access::write);
		if ( !target_file )
		{
			LOGI_ERROR						("unpack", args.log_format, args.log_flags, "cannot open target file: '%s'", args.target_path.c_str());
			return							false;
		}

		if ( node->is_archive() && node->is_compressed() )
		{
			allocated_buffer				uncompressed_data;
			result_enum const decompress_result	=	decompress_node	(node, & uncompressed_data, args.device, 
																	 args.allocator, & args.compressor);
			R_ASSERT_U						(decompress_result == result_success);

			args.device->write				(target_file, uncompressed_data.c_ptr(), uncompressed_data.size());
		}
		else
		{
			native_path_string const source_file_path	=	get_node_physical_path(node);
			file_type_pointer	source_file	(source_file_path, args.device, file_mode::open_existing, file_access::read);
			if ( !source_file )
			{
				LOGI_ERROR					("unpack", args.log_format, args.log_flags, "cannot open source file: '%s'", source_file_path.c_str());
				return						false;
			}

			file_size_type const file_offs	=	get_file_offs(node);
			u32 const raw_file_size		=	get_file_size(node);
			args.device->seek				(source_file, file_offs, seek_file_begin);
			copy_data						(args.device, target_file, source_file, raw_file_size, NULL, 0);
		}
	}

	args.target_path.set_length				(saved_path_length);

	++args.unpacked_nodes_count;
	if ( args.callback )
	{
		args.callback						(args.unpacked_nodes_count, args.nodes_count, 
											 node->get_name(), node->get_flags());
	}

	return									out_result;
}

} // namespace vfs 
} // namespace xray 

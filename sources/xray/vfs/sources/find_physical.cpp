////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"
#include <xray/fs/synchronous_device_interface.h>
#include <xray/fs/file_type_pointer.h>
#include "fat_header.h"

namespace xray {
namespace vfs {

using namespace fs_new;

bool   check_is_archive_file			(pcstr physical_path, synchronous_device_interface & device)
{
#ifndef MASTER_GOLD
	if ( strings::equal(fs_new::extension_from_path(physical_path), "orig") )
		return								false;
#endif // ifndef MASTER_GOLD

	file_type_pointer file				(physical_path, device, file_mode::open_existing, file_access::read, assert_on_fail_false);
	if ( !file )
		return								false;

	u32 const char_count				=	10;
	fixed_string<char_count + 1> big_endian_string_part		(big_endian_string, big_endian_string + char_count);
	fixed_string<char_count + 1> little_endian_string_part	(little_endian_string, little_endian_string + char_count);

	fixed_string<char_count + 1>			 part;
	part.set_length							(char_count);
	bool is_db_file						=	false;
	if ( device->read(file, part.get_buffer(), char_count) == char_count )
		if ( part == big_endian_string_part || part == little_endian_string_part )
			is_db_file					=	true;

	if ( is_db_file )
	{
		device->seek(file, 1024, seek_file_begin);
		char magic = 0;
		device->read(file, & magic, 1);
		if ( magic != 0x0D )
			return							false;
	}
	
	return									is_db_file;
}

bool   need_physical_folder_mount		(base_node<> * node, find_enum find_flags, traverse_enum traverse_type)
{
	bool const need_recursive			=	(traverse_type != traverse_branch) && (find_flags & find_recursively);

	physical_folder_node<> * physical_folder	=	node_cast<physical_folder_node>(node);
	if ( !physical_folder )
		return								false;
	
	if ( !physical_folder->is_scanned(need_recursive) )
		return								true;

	return									false;
}

bool   need_automatic_archive_mount		(bool *	is_out_of_memory, base_node<> * node, memory::base_allocator * allocator)
{
	* is_out_of_memory					=	false;

	if ( node->is_erased() )
		return								false;

	physical_file_node<> * file_node	=	node_cast<physical_file_node>(node);
	if ( file_node->checked_is_archive() )
		return								file_node->is_archive_file() && !file_node->is_mounted_archive();

	mount_root_node_base<> * mount_root	=	node->get_mount_root();

	native_path_string const file_path	=	get_node_physical_path(file_node);

	synchronous_device_interface	sync_device(mount_root->async_device, allocator,
												mount_root->device, mount_root->watcher_enabled);
	if ( sync_device.out_of_memory() )
	{
		* is_out_of_memory				=	true;
		return								false;
	}

	bool const is_archive_file			=	check_is_archive_file(file_path.c_str(), sync_device);

	file_node->set_checked_is_archive		(is_archive_file);
	return									file_node->is_archive_file();
}

bool   need_physical_mount				(bool *						is_out_of_memory,
										 base_node<> *				node, 
										 find_enum					find_flags, 
										 traverse_enum				traverse_type, 
										 memory::base_allocator *	allocator)
{
	* is_out_of_memory					=	false;
	if ( node->is_link() )
		return								false;
	if ( node_cast<physical_folder_node>(node) )
		return								need_physical_folder_mount		(node, find_flags, traverse_type);
	
	if ( node_cast<physical_file_node>(node) && traverse_type != traverse_child )
	{
		if ( (traverse_type == traverse_node) && (find_flags & find_file_only) )
			return							false;

		return								need_automatic_archive_mount	(is_out_of_memory, node, allocator);
	}

	return									false;
}

bool   need_physical_mount_or_async (base_node<> *				node, 
									 find_enum					find_flags, 
									 traverse_enum				traverse_type)
{
	if ( node->is_link() )
		return								false;
	if ( node_cast<physical_folder_node>(node) )
		return								need_physical_folder_mount		(node, find_flags, traverse_type);

	physical_file_node<> * file_node	=	node_cast<physical_file_node>(node);
	if ( file_node && traverse_type != traverse_child  )
	{
		if ( (traverse_type == traverse_node) && (find_flags & find_file_only) )
			return							false;

		if ( file_node->checked_is_archive() )
			return							file_node->is_archive_file() && !file_node->is_mounted_archive();

		return								true;
	}

	return									false;
}

void   find_async_expand_physical		(base_node<> * const			node, 
										 base_node<> * const			node_parent, 
										 async_callbacks_data * const	async_data, 
										 recursive_bool const			recursive,
										 u32 const						increment)
{
	if ( node->is_folder() )
		R_ASSERT							(node->is_folder() && node->is_physical());
	else
		R_ASSERT							(node->is_file() && node->is_physical());

	mount_root_node_base<> * mount_root	=	node->get_mount_root();
	virtual_path_string						virtual_path;
	node->get_full_path						(& virtual_path);
	native_path_string	physical_path	=	get_node_physical_path(node);

	device_file_system_proxy				device_proxy(mount_root->device, mount_root->watcher_enabled);
	synchronous_device_interface			sync_device(device_proxy);
	query_mount_arguments	args;
	args.allocator						=	mount_root->allocator; // async_data->env.allocator;
	args.virtual_path					=	virtual_path;
	args.asynchronous_device			=	mount_root->async_device;
	args.synchronous_device				=	mount_root->device ? & sync_device : NULL;

	if ( node->is_folder() )
	{
		args.type						=	mount_type_physical_path;
		args.physical_path				=	physical_path;
		args.callback					=	boost::bind( & async_callbacks_data::on_lazy_mounted, async_data, _1);
	}
	else
	{
		args.type						=	mount_type_archive;
		args.fat_physical_path			=	physical_path;
		args.callback					=	boost::bind( & async_callbacks_data::on_automatic_archive_or_subfat_mounted, async_data, 
														 _1, increment);	
	}

	args.recursive						=	recursive;
	args.submount_node					=	node;
	args.submount_type					=	node->is_folder() ? submount_type_lazy : submount_type_automatic_archive;
	args.parent_of_submount_node		=	node_parent;
	args.root_write_lock				=	async_data->env.node;
	args.unlock_after_mount				=	false;

	async_data->env.file_system->query_mount	(args);
}

} // namespace vfs
} // namespace xray

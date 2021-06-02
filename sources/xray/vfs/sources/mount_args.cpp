////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/mount_args.h>
#include <xray/fs/path_string_utils.h>
#include "nodes.h"

namespace xray {
namespace vfs {

using namespace fs_new;

query_mount_arguments::query_mount_arguments () 
	:	type								(mount_type_unknown), 
		allocator							(NULL),
		asynchronous_device					(NULL),
		synchronous_device					(NULL),
		submount_node						(NULL),
		parent_of_submount_node				(NULL),
		root_write_lock						(NULL),
		watcher_enabled						(watcher_enabled_true),
		mount_id							(0),
		mount_ptr							(NULL),
		submount_type						(submount_type_unset),
		unlock_after_mount					(true)
{
}

query_mount_arguments	query_mount_arguments::mount_physical_path 
												(memory::base_allocator *			allocator,
												 virtual_path_string const &		virtual_path,
												 native_path_string const &			physical_path,
												 pcstr								descriptor,
												 asynchronous_device_interface *	async_device,
												 synchronous_device_interface *		sync_device,
												 query_mount_callback				callback,
												 recursive_bool						recursive,
												 lock_operation_enum				lock_operation,
												 watcher_enabled_bool				watcher_enabled)
{
	query_mount_arguments					args;
	args.type							=	mount_type_physical_path; 
	args.allocator						=	allocator; 
	args.virtual_path					=	virtual_path;
	args.physical_path					=	physical_path; 
	args.descriptor						=	descriptor;
	args.asynchronous_device			=	async_device; 
	args.synchronous_device				=	sync_device; 
	args.callback						=	callback; 
	args.recursive						=	recursive;
	args.watcher_enabled				=	watcher_enabled;
	args.lock_operation					=	lock_operation;
	R_ASSERT								(physical_path.length());
	return									args;
}

query_mount_arguments	query_mount_arguments::mount_archive
											(memory::base_allocator *			allocator,
											 virtual_path_string const &		virtual_path,
											 native_path_string const &			archive_physical_path,
											 native_path_string const &			fat_physical_path,
											 pcstr								descriptor,
											 asynchronous_device_interface *	async_device,
											 synchronous_device_interface *		sync_device,
											 query_mount_callback				callback,
											 lock_operation_enum				lock_operation)
{
	query_mount_arguments					args;
	args.type							=	mount_type_archive; 
	args.allocator						=	allocator;
	args.virtual_path					=	virtual_path; 
	args.archive_physical_path			=	archive_physical_path; 
	args.fat_physical_path				=	fat_physical_path;
	args.descriptor						=	descriptor;
	args.asynchronous_device			=	async_device; 
	args.synchronous_device				=	sync_device; 
	args.callback						=	callback; 
	args.watcher_enabled				=	watcher_enabled_false;
	args.lock_operation					=	lock_operation;
	R_ASSERT								(archive_physical_path.length());
	return									args;
}

void   query_mount_arguments::convert_pathes_to_absolute ()
{
	virtual_path.make_lowercase				();
	if ( type == mount_type_physical_path )
	{
		physical_path.make_lowercase		();
		bool convertion_result			=	convert_to_absolute_path_inplace(& physical_path, assert_on_fail_true);
		XRAY_UNREFERENCED_PARAMETER			(convertion_result);

	}
	else if ( type == mount_type_archive )
	{
		archive_physical_path.make_lowercase	();
		fat_physical_path.make_lowercase	();
 		if ( fat_physical_path.length() == 0 )
 			fat_physical_path			=	archive_physical_path;
		else if ( archive_physical_path.length() == 0 )
			archive_physical_path		=	fat_physical_path;

 		bool convertion_result			=	convert_to_absolute_path_inplace(& archive_physical_path, assert_on_fail_true);
		convertion_result				=	convert_to_absolute_path_inplace(& fat_physical_path, assert_on_fail_true);
		XRAY_UNREFERENCED_PARAMETER			(convertion_result);
	}
	else
		NODEFAULT					();
}

native_path_string	 query_mount_arguments::get_physical_path ()
{
	if ( type == mount_type_physical_path )
		return							physical_path;

	if ( !submount_node )
		return							fat_physical_path;

	if ( !submount_node->is_external_sub_fat() )
		return							fat_physical_path;

	native_path_string					out_result;
	get_path_without_last_item			(& out_result, fat_physical_path.c_str());
	external_subfat_node<> * const external_subfat	=	node_cast<external_subfat_node>(submount_node);
	bool const append_result		=	append_relative_path(& out_result, 
															native_path_string(external_subfat->relative_path_to_external));
	R_ASSERT_U							(append_result);
	return								out_result;
}

} // namespace vfs
} // namespace xray

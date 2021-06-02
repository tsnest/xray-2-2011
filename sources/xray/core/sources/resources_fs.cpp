////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/resources_fs.h>
#include "resources_manager.h"

namespace xray {
namespace resources {

fs_new::physical_path_info   get_physical_path_info	(vfs::vfs_iterator const & it)
{
	if ( it.is_end() )
		return								fs_new::physical_path_info();

	fs_new::native_path_string const physical_path	=	it.get_physical_path();

	return									get_physical_path_info(physical_path, false);
}

fs_new::physical_path_info   get_physical_path_info	(fs_new::native_path_string const & path, bool check_archive_on_the_way)
{
	fs_new::synchronous_device_interface const &	device	=	g_resources_manager->get_synchronous_device();

	if ( !check_archive_on_the_way )
		return								device->get_physical_path_info(path);

	fs_new::native_path_string	current_path	=	path;
	while ( current_path )
	{
		fs_new::physical_path_info const & current_info	=	device->get_physical_path_info(current_path);

		if ( current_info.does_exist_and_is_file() )
			return							current_info;

		if ( current_info.does_exist_and_is_folder() )
		{
			if ( current_path.length() == path.length() )
				return						current_info;
			else
				return						fs_new::physical_path_info();
		}

		fs_new::get_path_without_last_item_inplace	(& current_path);
	}

	return						fs_new::physical_path_info();
}

fs_new::physical_path_info   get_physical_path_info	(fs_new::virtual_path_string const & path, 
													 pcstr mount_descriptor, 
													 bool check_archive_on_the_way)
{
	vfs::virtual_file_system * file_system	=	g_resources_manager->get_vfs();

	fs_new::native_path_string				physical_path;
	fs_new::physical_path_info				path_info;
	vfs::vfs_mount_ptr						mount_ptr;
 
	for (	u32 index = 0; 
			file_system->convert_virtual_to_physical_path	(& physical_path, path, index, & mount_ptr);
			++index	)
	{
		fs_new::physical_path_info const it_path_info	=	get_physical_path_info(physical_path, check_archive_on_the_way);
		if ( !it_path_info.exists() )
			continue;
		
		if ( mount_descriptor && *mount_descriptor )
		{
			if ( strings::equal(mount_ptr->get_descriptor(), mount_descriptor) )
				return						it_path_info;
		}

		R_ASSERT							(
			!path_info.exists(), 
			"more then one physical file on virtual path: '%s' ('%s' and '%s')",
			path.c_str(),
			path_info.get_full_path().c_str(),
			it_path_info.get_full_path().c_str()
			);

		path_info						=	it_path_info;
	}

	return									path_info;	
}

#if XRAY_FS_NEW_WATCHER_ENABLED

u32	  subscribe_watcher					(fs_new::virtual_path_string const &	virtual_path, 
										 vfs::vfs_notification_callback const &	callback)
{
	vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();
	return									vfs->subscribe(virtual_path, callback);
}

void   unsubscribe_watcher				(u32 subscriber_id)
{
	vfs::virtual_file_system * const vfs	=	g_resources_manager->get_vfs();
	vfs->unsubscribe						(subscriber_id);
}

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

fs_new::synchronous_device_interface &	get_synchronous_device	()
{
	return									g_resources_manager->get_synchronous_device();
}

} // namespace resources
} // namespace xray

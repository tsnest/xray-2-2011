////////////////////////////////////////////////////////////////////////////
//	Created		: 09.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "nodes.h"
#include <xray/vfs/mount_history.h>
#include <xray/vfs/virtual_file_system.h>
#include <xray/vfs/mount_args.h>
#include <xray/vfs/watcher.h>

namespace xray {
namespace vfs {

using namespace fs_new;

#if XRAY_FS_NEW_WATCHER_ENABLED

void   quit_watching_history			(vfs_watcher * watcher, mount_history_container & history);
void   watch_history					(vfs_watcher * watcher, mount_history_container & history);

#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

void   add_to_mount_history				(vfs_mount * new_item, virtual_file_system & file_system)
{
#if XRAY_FS_NEW_WATCHER_ENABLED
	vfs_watcher::mutex * const mutex	=	file_system.watcher ? & file_system.watcher->get_mutex() : NULL;
	bool const watcher_enabled			=	mutex && new_item->get_mount_root()->watcher_enabled == watcher_enabled_true;
	if ( watcher_enabled )
	{
		mutex->lock							();
		quit_watching_history				(file_system.watcher, file_system.mount_history);
	}
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

	file_system.mount_history.push_back		(new_item);

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( watcher_enabled )
	{
		watch_history						(file_system.watcher, file_system.mount_history);
		mutex->unlock						();
	}
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED
}

void   erase_from_mount_history			(vfs_mount * item, virtual_file_system & file_system)
{
#if XRAY_FS_NEW_WATCHER_ENABLED
	vfs_watcher::mutex * const mutex	=	file_system.watcher ? & file_system.watcher->get_mutex() : NULL;
	bool const watcher_enabled			=	mutex && item->get_mount_root()->watcher_enabled == watcher_enabled_true;
	if ( watcher_enabled )
	{
		mutex->lock							();
		quit_watching_history				(file_system.watcher, file_system.mount_history);
	}
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED

	file_system.mount_history.erase			(item);

#if XRAY_FS_NEW_WATCHER_ENABLED
	if ( watcher_enabled )
	{
		watch_history						(file_system.watcher, file_system.mount_history);
		mutex->unlock						();
	}
#endif // #if XRAY_FS_NEW_WATCHER_ENABLED
}

vfs_mount_ptr   find_in_mount_history	(convertion_filter const &	filter,
										 mount_history_container &	history)
{
	R_ASSERT								(filter);

	mount_history_container::mutex_raii	raii	(history.policy());

	vfs_mount *			result			=	NULL;
	for ( vfs_mount *			it		=	history.front();
								it		!=	NULL;
								it		=	history.get_next_of_object(it) )
	{
		if ( !filter(it->get_descriptor(), it->get_physical_path(), it->get_virtual_path()) )
			continue;

		R_ASSERT							(!result);
		result							=	it;
	}

	if ( !result )
		return								NULL;

	return									result->make_pointer_possibly_increase_destroy_count();
}

vfs_mount_ptr   find_in_mount_history	(virtual_path_string const &	virtual_path, 
										 native_path_string const &		fat_physical_path, 
										 mount_history_container &		history)
{
	ASSERT									(virtual_path.length() || fat_physical_path.length());

	mount_history_container::mutex_raii	raii	(history.policy());

	for ( vfs_mount *			it		=	history.front();
								it		!=	NULL;
								it		=	history.get_next_of_object(it) )
	{
		if ( virtual_path.length() && virtual_path != it->get_mount_root()->virtual_path )
			continue;

		if ( fat_physical_path.length() && fat_physical_path != it->get_mount_root()->physical_path )
			continue;

		return								it->make_pointer_possibly_increase_destroy_count();
	}

	return									NULL;
}

vfs_mount_ptr    find_sub_fat_parent	(virtual_path_string const &	virtual_path, 
										 native_path_string const &		fat_physical_path, 
										 mount_history_container &		history)
{
 	vfs_mount_ptr closest_parent		=	NULL;
 	u32 closest_level					=	0;
 
	u32 const sub_fat_level				=	virtual_path.count_of(virtual_path_string::separator);
 
 	for ( vfs_mount *			it		=	history.front();
								it		!=	NULL;
								it		=	history.get_next_of_object(it) )
	{
 		vfs_mount_ptr mount				=	& * it;
 		if ( fat_physical_path != mount->get_mount_root()->physical_path )
 			continue;
		
 		u32 const level					=	strings::count_of(mount->get_mount_root()->virtual_path, virtual_path_string::separator);
 		if ( level >= sub_fat_level )
 			continue;
 	
 		if ( !closest_parent || closest_level < level )
 		{
 			closest_parent				=	mount;
 			closest_level				=	level;
 		}	
 	}
 
 	return									closest_parent;
}

bool  convert_virtual_to_physical_path	(mount_history_container &		mount_history,
										 native_path_string *			out_path, 
										 virtual_path_string const &	path,
										 convertion_filter const &		filter,
										 vfs_mount_ptr *				out_mount_ptr)
{
	mount_history_container::mutex_raii	raii(mount_history.policy());
	
	vfs_mount * out_mount				=	NULL;

	native_path_string						physical_path;

	for ( vfs_mount *	it_history		=	mount_history.front();
						it_history;
						it_history		=	mount_history.get_next_of_object(it_history) )
	{
		mount_root_node_base<> * mount_root	=	it_history->get_mount_root();

		if ( mount_root->mount_type != mount_type_physical_path )
			continue;

		if ( !path_starts_with(path.c_str(), mount_root->virtual_path) )
			continue;

		if ( filter && !filter(mount_root->descriptor, mount_root->physical_path, mount_root->virtual_path) )
			continue;

		physical_path					=	mount_root->physical_path;
		if ( physical_path.length() )
			physical_path				+=	native_path_string::separator;

		physical_path.append_with_conversion	(path.c_str() + strings::length(mount_root->virtual_path) + 1,
												 path.c_str() + path.length());

		CURE_ASSERT							(!out_mount, break, 
											 "ambiguity for virtual path '%s'!\n"
											 "physical path could be: '%s'\nor '%s'",
											 path.c_str(),
											 out_path->c_str(), physical_path.c_str());

		out_mount						=	it_history;
		* out_path						=	physical_path;

		// not breaking to check only one virtual path corresponds to physical path
	}

	if ( out_mount_ptr )
		* out_mount_ptr					=	out_mount;

	return									out_mount != NULL;
}

struct filter_helper
{
	filter_helper						(u32 index) : index(index), current(0) {}
	bool	filter						(pcstr description, pcstr physical_path, pcstr virtual_path)
	{
		XRAY_UNREFERENCED_PARAMETERS		(description, physical_path, virtual_path);
		bool const result				=	(current == index);
		++current;
		return								result;
	}
	u32 current;
	u32	index;
};

bool  convert_virtual_to_physical_path	(mount_history_container &		mount_history,
										 native_path_string *			out_path, 
										 virtual_path_string const &	path,
										 u32 const						index,
										 vfs_mount_ptr *				out_mount_ptr)
{
	filter_helper	helper					(index);
	bool const result					=	convert_virtual_to_physical_path
												(mount_history, 
												 out_path, 
												 path, 
												 boost::bind(& filter_helper::filter, & helper, _1, _2, _3), 
												 out_mount_ptr);

	return									result;
}

bool  convert_physical_to_virtual_path	(mount_history_container &		mount_history,
										 virtual_path_string *			out_path, 
										 native_path_string const &		in_path,
										 convertion_filter const &		filter,
										 vfs_mount_ptr *				out_mount_ptr)
{
	R_ASSERT								(in_path.is_lowercase(), "you should pass only lowercased path to convert_physical_to_virtual_path. path was: '%s'", in_path.c_str());
	native_path_string						path;
	if ( !convert_to_absolute_path(& path, in_path, assert_on_fail_false) )
		return								false;

	mount_history_container::mutex_raii	raii(mount_history.policy());
	
	vfs_mount * out_mount				=	NULL;

	virtual_path_string						virtual_path;

	for ( vfs_mount *	it_history		=	mount_history.front();
						it_history;
						it_history		=	mount_history.get_next_of_object(it_history) )
	{
		mount_root_node_base<> * mount_root	=	it_history->get_mount_root();

		if ( mount_root->mount_type != mount_type_physical_path )
			continue;

		if ( !path_starts_with(path.c_str(), mount_root->physical_path) )
			continue;

		if ( filter && !filter(mount_root->descriptor, mount_root->physical_path, mount_root->virtual_path) )
			continue;

		virtual_path					=	mount_root->virtual_path;
		if ( virtual_path.length() )
			virtual_path				+=	virtual_path_string::separator;
		virtual_path.append_with_conversion	(path.c_str() + strings::length(mount_root->physical_path) + 1,
											path.c_str() + path.length());

		CURE_ASSERT							(!out_mount, break, "ambiguity for physical path '%s'!\n"
											 "physical path could be: '%s'\nor '%s'",
											 path.c_str(),
											 out_path->c_str(), virtual_path.c_str());

		out_mount						=	it_history;
		* out_path						=	virtual_path;
		// not breaking to check only one virtual path corresponds to physical path
	}

	if ( out_mount_ptr )
		* out_mount_ptr					=	out_mount;

	return									out_mount != NULL;
}

bool  convert_physical_to_virtual_path	(mount_history_container &		mount_history,
										 virtual_path_string *			out_path, 
										 native_path_string const &		path,
										 u32 const						index,
										 vfs_mount_ptr *				out_mount_ptr)
{
	filter_helper	helper					(index);
	bool const result					=	convert_physical_to_virtual_path
												(mount_history, 
												 out_path, 
												 path, 
												 boost::bind(& filter_helper::filter, & helper, _1, _2, _3), 
												 out_mount_ptr);

	return									result;
}

} // namespace vfs
} // namespace xray
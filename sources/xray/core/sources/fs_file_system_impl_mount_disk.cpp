////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// mounting disk
//-----------------------------------------------------------------------------------

void   file_system_impl::mount_disk (pcstr const logical_path, pcstr const physical_path, file_system::watch_directory_bool const watch_directory)
{
	threading::mutex_raii		raii		(m_mount_mutex);

	verify_path_is_portable					(logical_path);
	verify_path_is_portable					(physical_path);

	u32					hash;
	fat_folder_node<> *	folder			=	find_or_create_folder(logical_path, & hash);

	if ( get_path_info(NULL, physical_path) == path_info::type_file )
	{
		mount_disk_file						(folder, physical_path, hash);
		LOGI_INFO							("file_system", "mounted file '%s' on '%s'", physical_path, logical_path);
	}
	else
	{
		mount_disk_folder					(folder, physical_path, hash);
		on_mount							(mount_history::mount_type_disk, logical_path, physical_path, watch_directory);
		LOGI_INFO							("file_system", "mounted dir '%s' on '%s'", physical_path, logical_path);
	}	
}

bool   file_system_impl::get_disk_path_to_store_file (pcstr				logical_path, 
													   buffer_string *		out_physical_path, 
													   mount_history * *	out_opt_mount_history)
{
	verify_path_is_portable					(logical_path);
	threading::mutex_raii		raii		(m_mount_mutex);
	path_string	file_logical_path		=	logical_path;

	find_disk_mount_by_logical_path_predicate	predicate(file_logical_path);
	m_mount_history.for_each				(predicate);
	mount_history * const mount_history	=	predicate.result;

	CURE_ASSERT								(mount_history, return false);

	* out_physical_path					=	mount_history->physical_path;
	* out_physical_path					+=	'/';
	out_physical_path->append				(file_logical_path.c_str() + mount_history->logical_path.length() + 1,
											 file_logical_path.c_str() + file_logical_path.length());

	if ( out_opt_mount_history )
		* out_opt_mount_history			=	mount_history;

	return									true;
}

} // namespace fs 
} // namespace xray 

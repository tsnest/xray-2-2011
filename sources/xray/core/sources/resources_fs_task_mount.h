////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_TASK_MOUNT_H_INCLUDED
#define RESOURCES_FS_TASK_MOUNT_H_INCLUDED

#include <xray/resources_fs.h>
#include <xray/resources_fs_task.h>
#include <xray/vfs/mount_args.h>
#include <xray/resources_mount_ptr.h>

namespace xray {
namespace resources {

class fs_task_mount : public fs_task, public core::noncopyable
{
public:
	fs_task_mount						(fs_new::virtual_path_string const & 	virtual_path,
										 fs_new::native_path_string const & 	physical_path,
										 pcstr									descriptor,
										 fs_new::watcher_enabled_bool			watcher_enabled,
										 query_mount_callback const &			callback,
										 recursive_bool							recursive,
										 memory::base_allocator *				allocator);

	fs_task_mount						(fs_new::virtual_path_string const & 	virtual_path,
										 fs_new::native_path_string const & 	fat_physical_path,
										 fs_new::native_path_string const &		archive_physical_path,
										 pcstr									descriptor,
										 query_mount_callback const &			callback,
										 memory::base_allocator *				allocator);

	virtual void	execute_may_destroy_this	();
	virtual void	call_user_callback			();

private:
	fs_new::virtual_path_string 		m_virtual_path;
	fs_new::native_path_string 			m_physical_path;
	fs_new::native_path_string & 		m_fat_physical_path;
	fs_new::native_path_string 			m_archive_physical_path;
	fixed_string<32>					m_descriptor;
	query_mount_callback				m_callback;

	fs_new::watcher_enabled_bool		m_watcher_enabled;
	query_mount_callback				m_mount_callback;
	bool								m_reverse_byte_order;
	recursive_bool						m_recursive;

	mount_ptr							m_mount_ptr;

}; // class fs_task_mount

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_TASK_MOUNT_H_INCLUDED
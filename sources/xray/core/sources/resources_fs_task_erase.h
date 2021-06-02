////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_FS_TASK_ERASE_H_INCLUDED
#define RESOURCES_FS_TASK_ERASE_H_INCLUDED

#include <xray/resources_fs.h>
#include <xray/vfs/mount_args.h>
#include <xray/resources_fs_task.h>

namespace xray {
namespace resources {

class fs_task_erase : public fs_task 
{
public:
					fs_task_erase				(fs_new::native_path_string const &		physical_path,
												 fs_new::virtual_path_string const &	virtual_path,
												 query_erase_callback const &			callback, 
												 memory::base_allocator * const			allocator);
	virtual void	execute_may_destroy_this	();
	virtual void	call_user_callback			();

private:
	void			on_hot_unmounted			(vfs::mount_result result);
	
private:
	fs_new::native_path_string			m_physical_path;
	fs_new::virtual_path_string			m_virtual_path;
	query_erase_callback				m_callback;

}; // class fs_task_erase

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_FS_TASK_ERASE_H_INCLUDED
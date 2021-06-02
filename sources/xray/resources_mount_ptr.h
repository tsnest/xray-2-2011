////////////////////////////////////////////////////////////////////////////
//	Created		: 09.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_MOUNT_PTR_H_INCLUDED
#define XRAY_RESOURCES_MOUNT_PTR_H_INCLUDED

#include <xray/vfs/mount_ptr.h>
#include <xray/resources_fs_task.h>

namespace xray {
namespace resources {

class fs_task_unmount;

class XRAY_VFS_API intrusive_fs_task_unmount_base
{
public:
	u32			get_reference_count		() const { return m_reference_count; }

	intrusive_fs_task_unmount_base		() : m_reference_count(0) {}
	void		destroy					(fs_task_unmount * object) const;
	threading::atomic32_type			m_reference_count;
};

class mount_holder;

typedef intrusive_ptr<fs_task_unmount, intrusive_fs_task_unmount_base, threading::simple_lock >	
		mount_ptr;

template class XRAY_CORE_API xray::intrusive_ptr< xray::resources::fs_task_unmount, 
												  xray::resources::intrusive_fs_task_unmount_base, 
												  xray::threading::simple_lock >;

class fs_task_unmount : public fs_task, 
						public intrusive_fs_task_unmount_base
{
public:
	fs_task_unmount						(vfs_sub_fat_resource_ptr const & sub_fat_ptr);

	virtual void	execute_may_destroy_this	();

	void			link_mount_ptr		(mount_ptr ptr)
	{
		ptr->m_next						=	m_next;
		m_next							=	ptr;
	}

	vfs::vfs_mount_ptr	get_vfs_mount_ptr	() { return m_sub_fat_ptr->mount_ptr; }

private:
	void   unmount_children				(vfs::vfs_mount * sub_fat);

private:
	vfs_sub_fat_resource_ptr			m_sub_fat_ptr;

	mount_ptr							m_next;

	friend class						intrusive_fs_task_unmount_base;
	friend class						resources_manager; // temp for debug

}; // class fs_task_unmount

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_MOUNT_PTR_H_INCLUDED
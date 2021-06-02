////////////////////////////////////////////////////////////////////////////
//	Created		: 11.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_VFS_SUB_FAT_RESOURCE_H_INCLUDED
#define XRAY_VFS_SUB_FAT_RESOURCE_H_INCLUDED

#include <xray/vfs/mount_ptr.h>
#include <xray/resources_resource_ptr.h>

namespace xray {
namespace resources {

class vfs_sub_fat_resource : public unmanaged_resource
{
public:
	typedef resource_ptr<vfs_sub_fat_resource, resources::unmanaged_intrusive_base>		pointer;

public:
	vfs::vfs_mount_ptr					mount_ptr;
	pointer								parent; 
};

typedef vfs_sub_fat_resource::pointer		vfs_sub_fat_resource_ptr;
typedef	child_resource_ptr	<vfs_sub_fat_resource, resources::unmanaged_intrusive_base>	vfs_sub_fat_child_resource_ptr;

vfs_sub_fat_resource_ptr	get_sub_fat_resource (vfs::vfs_mount_ptr mount_ptr);

#if !XRAY_PLATFORM_PS3
template class XRAY_CORE_API	child_resource_ptr	<vfs_sub_fat_resource, resources::unmanaged_intrusive_base>;
#endif // #if !XRAY_PLATFORM_PS3

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_VFS_SUB_FAT_RESOURCE_H_INCLUDED
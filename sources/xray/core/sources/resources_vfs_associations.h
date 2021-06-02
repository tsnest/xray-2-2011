////////////////////////////////////////////////////////////////////////////
//	Created		: 15.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_VFS_ASSOCIATIONS_H_INCLUDED
#define RESOURCES_VFS_ASSOCIATIONS_H_INCLUDED

namespace xray {
namespace resources {

void					set_associated							(vfs::vfs_iterator it, resource_base * resource);
bool  					is_associated							(vfs::vfs_iterator it);
bool  					is_associated_with						(vfs::vfs_iterator it, resource_base * resource);
managed_resource_ptr	get_associated_managed_resource_ptr		(vfs::vfs_iterator it);
unmanaged_resource_ptr	get_associated_unmanaged_resource_ptr	(vfs::vfs_iterator it);
query_result *			get_associated_query_result				(vfs::vfs_iterator it);
bool					try_clean_associated					(vfs::vfs_iterator it, u32 allow_reference_count);

} // namespace resources 
} // namespace xray

#endif // #ifndef RESOURCES_VFS_ASSOCIATIONS_H_INCLUDED
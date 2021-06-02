////////////////////////////////////////////////////////////////////////////
//	Created		: 19.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_MOUNT_REFERERS_H_INCLUDED
#define VFS_MOUNT_REFERERS_H_INCLUDED

#include <xray/fs/portable_path_string.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/vfs/mount_ptr.h>
#include <xray/vfs/mount_args.h>

namespace xray {
namespace vfs {

struct	mount_referer;

struct mount_referer_base
{
	mount_referer *				next;
};

typedef intrusive_list<mount_referer_base, mount_referer *, & mount_referer_base::next, threading::mutex>	
	ready_referers_list;

struct mount_referer : mount_referer_base
{
	vfs_mount_ptr						result;
	query_mount_callback				callback;
	memory::base_allocator *			allocator;
	ready_referers_list *				ready_list;

	mount_referer() : ready_list(0), allocator(NULL) {}
};

ready_referers_list *	get_ready_referers_list			(memory::base_allocator * allocator);
void					dispatch_ready_referers_list	();
void					free_ready_referers_list		();

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_MOUNT_REFERERS_H_INCLUDED
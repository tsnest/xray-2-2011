////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_NODES_ASSOCIATION_H_INCLUDED
#define VFS_NODES_ASSOCIATION_H_INCLUDED

#include <xray/vfs/api.h>

namespace xray {
namespace vfs {

struct fat_node_info;

struct XRAY_VFS_API vfs_association
{
	vfs_association						(u32 type) : type(type) {}
	virtual ~vfs_association			() {}

	u32 type;
};

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_NODES_ASSOCIATION_H_INCLUDED
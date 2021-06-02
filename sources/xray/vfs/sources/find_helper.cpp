////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "find.h"
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

vfs_iterator::type_enum	 iterator_type	(find_enum find_flags)
{
	return									(find_flags & find_recursively) ? vfs_iterator::type_recursive : vfs_iterator::type_non_recursive;
}

void   make_iterator					(vfs_locked_iterator & out_iterator, find_env & env)
{
	out_iterator.assign						(env.node, env.hashset(), env.find_flags & find_recursively ?
																	vfs_iterator::type_recursive : 
																	vfs_iterator::type_non_recursive,
											 env.mount_operation_id);
}



} // namespace vfs
} // namespace xray
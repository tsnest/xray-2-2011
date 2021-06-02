////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_BRANCH_LOCKS_H_INCLUDED
#define VFS_BRANCH_LOCKS_H_INCLUDED

#include "nodes.h"

namespace xray {
namespace vfs {

lock_type_enum   soften_lock			(lock_type_enum lock);
bool	has_lock						(base_node<> * node, lock_type_enum lock_type);
bool	lock_node						(base_node<> * node, lock_type_enum lock_type, lock_operation_enum operation);
void	unlock_node						(base_node<> * node, lock_type_enum lock_type);
void	upgrade_node					(base_node<> * node, lock_type_enum from_lock, lock_type_enum to_lock);
bool	lock_branch						(base_node<> * node, lock_type_enum lock_type, lock_operation_enum operation);
void	unlock_branch					(base_node<> * node, lock_type_enum lock_type);
void	upgrade_branch					(base_node<> * node, lock_type_enum from_lock, lock_type_enum to_lock);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_BRANCH_LOCKS_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "branch_locks.h"

namespace xray {
namespace vfs {

bool   has_lock							(base_node<> * node, lock_type_enum lock_type)
{
	if ( node->is_folder() )
		return								node_cast<base_folder_node>(node)->has_lock(lock_type);
	else if ( node->get_parent() )
		return								node->get_parent()->has_lock(soften_lock(lock_type));

	return									false;
}

lock_type_enum   soften_lock			(lock_type_enum lock)
{
	if ( lock == lock_type_read )
		return								lock_type_soft_read;
	if ( lock == lock_type_write )
		return								lock_type_soft_write;

	return									lock;
}

bool	lock_node						(base_node<> * node, lock_type_enum lock_type, lock_operation_enum operation)
{
	base_folder_node<> * folder_node	=	node->is_link() ? NULL : node_cast<base_folder_node>(node);

	if ( folder_node )
		if ( !folder_node->lock	(lock_type, operation) )
			return							false;

	return									true;
}

void	unlock_node						(base_node<> * node, lock_type_enum lock_type)
{
	base_folder_node<> * folder_node	=	node->is_link() ? NULL : node_cast<base_folder_node>(node);

	if ( folder_node )
		folder_node->unlock					(lock_type);
}

void   upgrade_node						(base_node<> * node, lock_type_enum from_lock, lock_type_enum to_lock)
{
	if ( !node )
		return;
	base_folder_node<> * folder_node	=	node->is_link() ? NULL : node_cast<base_folder_node>(node);
	if ( folder_node )
		folder_node->upgrade_lock			(from_lock, to_lock);
}

bool   lock_branch						(base_node<> * node, lock_type_enum lock_type, lock_operation_enum operation)
{
	base_folder_node<> * parent			=	node->get_parent();

	lock_type_enum const soft_lock		=	soften_lock(lock_type);

	if ( parent	)
		if ( !lock_branch(node_cast<base_node>(parent), soft_lock, operation) )
			return							false;

	if ( !lock_node(node, lock_type, operation) )
	{
		if ( parent )
			unlock_branch					(node_cast<base_node>(parent), soft_lock);
		return								false;
	}

	return									true;
}

void   unlock_branch					(base_node<> * node, lock_type_enum lock_type)
{
	if ( !node )
		return;

	unlock_node								(node, lock_type);

	base_folder_node<> * parent			=	node->get_parent();
	if ( parent	)
		unlock_branch						(node_cast<base_node>(parent), soften_lock(lock_type));
}

void   upgrade_branch					(base_node<> * node, lock_type_enum from_lock, lock_type_enum to_lock)
{
	base_folder_node<> * folder_node	=	node_cast<base_folder_node>(node);
	base_folder_node<> * parent			=	node->get_parent();

	if ( !parent )
	{
		if ( folder_node )
			folder_node->upgrade_lock		(from_lock, to_lock);
		return;
	}

	unlock_node								(node, from_lock);

	upgrade_branch							(node_cast<base_node>(parent), soften_lock(from_lock), soften_lock(to_lock));

	lock_node								(node, to_lock, lock_operation_lock);
}

} // namespace vfs
} // namespace xray
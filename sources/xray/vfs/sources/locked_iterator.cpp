////////////////////////////////////////////////////////////////////////////
//	Created		: 04.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/locked_iterator.h>
#include "find.h"

namespace xray {
namespace vfs {

void   vfs_locked_iterator::assign	(base_node<> * node, vfs_hashset * hashset, type_enum type, u32 mount_operation_id)
{
	if ( m_node == node )
		return;

	unlock_and_decref_if_needed				();
	* (vfs_iterator *)(this)			=	vfs_iterator(node, NULL, hashset, type);

	this->mount_operation_id			=	mount_operation_id;

	if ( !m_node )
		return;
}

vfs_locked_iterator::~vfs_locked_iterator ()
{
	clear									();	
}

void   vfs_locked_iterator::clear	()
{
	unlock_and_decref_if_needed				();
	m_node								=	NULL;
	m_link_target						=	NULL;
}

void   vfs_locked_iterator::grab	(vfs_locked_iterator const & other)
{
	unlock_and_decref_if_needed				();
	m_node								=	other.m_node;
	m_link_target						=	other.m_link_target;
	m_hashset							=	other.m_hashset;
	m_type								=	other.m_type;
	mount_operation_id					=	other.mount_operation_id;
	((vfs_locked_iterator &)other).m_node	=	NULL;
}

void   vfs_locked_iterator::unlock_and_decref_if_needed ()
{
	if ( !m_node )
		return;

  	unlock_and_decref_recursively		(m_node, lock_type_read,
  										 m_type == type_recursive ? find_recursively : (find_enum)0, m_hashset, 
										 mount_operation_id);
}

void   vfs_locked_iterator::clear_without_unpin ()
{
	m_node								=	NULL;
	m_link_target						=	NULL;
}

} // namespace vfs
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: 
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "overlapped_node_iterator.h"
#include <xray/fs/virtual_path_string.h>
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

overlapped_node_iterator::overlapped_node_iterator (overlapped_node_initializer const & initializer)
										:	path(initializer.path), node(initializer.node), 
											lock_type(initializer.lock_type), hashset_lock(initializer.hashset_lock) 
{ 
}

void   overlapped_node_iterator::operator ++ ()
{
	ASSERT									(node != NULL);

	node								=	node->get_hashset_next();
	node								=	vfs_hashset::skip_nodes_with_wrong_path (node, path);
}

overlapped_node_iterator::~overlapped_node_iterator ()
{
	clear									();
}

void   overlapped_node_iterator::clear ()
{
	if ( hashset_lock )
		hashset_lock->unlock				(to_threading_lock_type(lock_type));
	hashset_lock						=	NULL;
	lock_type							=	lock_type_uninitialized;
	node								=	NULL;
}

} // namespace vfs
} // namespace xray

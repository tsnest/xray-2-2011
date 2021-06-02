////////////////////////////////////////////////////////////////////////////
//	Created		: 04.03.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_OVERLAPPED_NODE_ITERATOR_H_INCLUDED
#define VFS_OVERLAPPED_NODE_ITERATOR_H_INCLUDED

#include <xray/vfs/platform_configuration.h>
#include "base_folder_node.h"

namespace xray {
namespace vfs {

class overlapped_node_initializer
{
public:
	overlapped_node_initializer()		:	path(NULL), node(NULL), lock_type(lock_type_uninitialized),
											hashset_lock(NULL){}
private:
	pcstr								path;
	base_node<> *						node;
	lock_type_enum						lock_type;
	threading::reader_writer_lock *		hashset_lock;

	friend class						overlapped_node_iterator;
	friend class						vfs_hashset;
};

typedef std::pair<overlapped_node_initializer, overlapped_node_initializer>		overlapped_nodes_pair;


class	vfs_hashset;

class overlapped_node_iterator			: private boost::noncopyable
{
public:
	overlapped_node_iterator			(overlapped_node_initializer const & initializer);

	void				operator ++		();
	bool				operator ==		(overlapped_node_iterator const & other) const { return node == other.node; }

	~overlapped_node_iterator			();

	base_node<> *		c_ptr			() { return node; }
	base_node<> *		operator -> 	() { return node; }
	base_node<> const *	operator -> 	() const { return node; }
	base_node<> &		operator *		() { return * node; }
	base_node<> const &	operator *		() const { return * node; }
	operator bool						() const { return node != NULL; }

	void		clear					();
private:
	pcstr								path;
	base_node<> *						node;
	lock_type_enum						lock_type;
	threading::reader_writer_lock *		hashset_lock;

}; 

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_OVERLAPPED_NODE_ITERATOR_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 14.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_HASHSET_H_INCLUDED
#define VFS_HASHSET_H_INCLUDED

#include <xray/vfs/platform_configuration.h>
#include <xray/threading_reader_writer_lock.h>
#include <xray/vfs/types.h>
#include <xray/vfs/base_node.h>

namespace xray {
namespace vfs {

class overlapped_node_initializer;

// theres a big assumption of how this hashset should be used:
// nodes to insert in hash should go in order in which they're mounted (this is natural order)
class vfs_hashset
{
public:
	bool			find_and_lock_branch	(base_node<> * & out_locked_branch, pcstr path, lock_type_enum lock_type, lock_operation_enum lock_operation = lock_operation_lock);
	bool			find_and_lock_branch	(base_node<> * & out_locked_branch, pcstr path, u32 hash, lock_type_enum lock_type, lock_operation_enum lock_operation = lock_operation_lock);

	typedef std::pair< overlapped_node_initializer, overlapped_node_initializer >	begin_end_pair;
	
	begin_end_pair	equal_range			(pcstr path, u32 hash, lock_type_enum lock_type);
	begin_end_pair	equal_range			(pcstr path, lock_type_enum lock_type);

	void			insert				(u32 hash, base_node<> * node, u32 mount_id);
	void			erase				(u32 hash, base_node<> * node);
	void			replace				(u32 hash, base_node<> * with_node, base_node<> * what_node, u32 mount_id);

	void			check_consistency	(base_node<> * node, pcstr path);

// this functions should be used with caution. they depend on branch being previously locked
	bool			find_no_branch_lock	(base_node<> * & out_locked_node, pcstr path, lock_type_enum lock_type, lock_operation_enum lock_operation = lock_operation_lock);
	bool			find_no_branch_lock	(base_node<> * & out_locked_node, pcstr path, u32 hash, lock_type_enum lock_type, lock_operation_enum lock_operation = lock_operation_lock);

	enum			check_locks_bool	{ check_locks_false, check_locks_true };
	base_node<> *   find_no_lock		(pcstr path, check_locks_bool = check_locks_true);

private:
	enum {
		hash_locks_count	= 32,
	};

	threading::reader_writer_lock &		get_hashlock (u32 hash) { return m_hashlocks[hash % hash_locks_count]; }
	static	base_node<> *				skip_nodes_with_wrong_path	(base_node<> * node, pcstr path);

private:
	typedef	xray::detail::fixed_size_policy<1024 * 32>	size_policy;
	typedef	hash_multiset<	base_node<>, 
							base_node<>::node_pointer, 
							& base_node<>::m_hashset_next, 
							size_policy	>	
										hashset_impl;
	threading::reader_writer_lock		m_hashlocks[hash_locks_count];

	hashset_impl						m_hashset;

	friend class						overlapped_node_iterator;
};

base_node<> *   find_referenced_link_node	(base_node<> const * node);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_HASHSET_H_INCLUDED
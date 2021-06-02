////////////////////////////////////////////////////////////////////////////
//	Created		: 28.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/hashset.h>
#include "base_folder_node.h"
#include "branch_locks.h"
#include "overlapped_node_iterator.h"
#include <xray/fs/path_part_iterator.h>
#include <xray/fs/virtual_path_string.h>
#include <xray/fs/path_string_utils.h>
#include <xray/vfs/virtual_file_system.h>
#include "nodes.h"

namespace xray {
namespace vfs {

using namespace fs_new;

bool   vfs_hashset::find_and_lock_branch	(base_node<> * & out_locked_branch, pcstr path, u32 hash, lock_type_enum lock_type, lock_operation_enum lock_operation)
{
	ASSERT									(lock_type == lock_type_read || lock_type == lock_type_write);

	while ( identity(true) )
	{
		begin_end_pair	begin_end		=	equal_range(path, hash, lock_type_read);
		overlapped_node_iterator	it		(begin_end.first);
		base_node<> *	node			=	it.c_ptr();
		if ( !node )
		{
			out_locked_branch			=	node;
			return							true;
		}

		if ( lock_branch(node, lock_type, lock_operation_try_lock) )
		{
			out_locked_branch			=	node;
			return							true;
		}

		if ( lock_operation == lock_operation_try_lock )
			return							false;

		threading::yield					();
	}

	return									false;
}

bool   vfs_hashset::find_and_lock_branch	(base_node<> * & out_locked_branch, pcstr const path, lock_type_enum lock_type, lock_operation_enum lock_operation)
{
	u32 const path_length				=	strings::length(path);
	u32 const hash						=	path_crc32(path, path_length);
	return									find_and_lock_branch(out_locked_branch, path, hash, lock_type, lock_operation);
}

bool   vfs_hashset::find_no_branch_lock	(base_node<> * & out_locked_node, pcstr const path, u32 const hash, lock_type_enum lock_type, lock_operation_enum lock_operation)
{
	base_node<> *	node				=	NULL;
	while ( identity(true) )
	{
		begin_end_pair	begin_end		=	equal_range(path, hash, lock_type_read);
		overlapped_node_iterator	it		(begin_end.first);
		node							=	it.c_ptr();
		if ( !node )
		{
			out_locked_node				=	NULL;
			return							true;
		}

		if ( lock_node(node, lock_type, lock_operation_try_lock) )
			break;

		if ( lock_operation == lock_operation_try_lock )
			return							false;

		threading::yield					();
	}

	for ( base_folder_node<> *	ancestor	=	node->get_parent();
								ancestor;
								ancestor	=	node_cast<base_node>(ancestor)->get_parent() )
	{
		R_ASSERT							(ancestor->has_some_lock(), "omg! you cannot search while not holding at least soft read lock!");
	}

	out_locked_node						=	node;
	return									true;
}

bool   vfs_hashset::find_no_branch_lock	(base_node<> * & out_locked_node, pcstr const path, lock_type_enum lock_type, lock_operation_enum lock_operation)
{
	u32 const hash						=	path_crc32(virtual_path_string(path));
	return									find_no_branch_lock(out_locked_node, path, hash, lock_type, lock_operation);
}

base_node<> *   vfs_hashset::find_no_lock	(pcstr const path, check_locks_bool const check_locks)
{
	u32 const hash						=	path_crc32(virtual_path_string(path));
	begin_end_pair	begin_end			=	equal_range(path, hash, lock_type_read);
	overlapped_node_iterator	it			(begin_end.first);
	base_node<> *	node				=	it.c_ptr();

	if ( !node )
		return								NULL;
	
	if ( check_locks == check_locks_true )
	{
		base_folder_node<> * it_check_node	=	node->is_folder() ? node_cast<base_folder_node>(node) : node->get_parent();
		
		bool branch_locked				=	false;
		for ( ; it_check_node; it_check_node	=	node_cast<base_node>(it_check_node)->get_parent() )
		{
			bool has_locks				=	it_check_node->has_some_lock();
			if ( branch_locked && !has_locks )
			{
				branch_locked			=	false;
				break;
			}

			branch_locked				=	has_locks;
		}
		R_ASSERT							(branch_locked, "omg! you cannot search while not holding at least soft read lock!");
	}
	
	return									node;
}

struct should_overlap_predicate
{
	should_overlap_predicate			(u32 mount_id) : mount_id(mount_id) {}

	bool	operator	()				(base_node<> * node) const
	{
		return								mount_id_of_node(node) < mount_id;
	}

	u32									mount_id;
};

void   vfs_hashset::insert				(u32 hash, base_node<> * node, u32 mount_id)
{
	threading::reader_writer_lock_raii		raii (get_hashlock(hash), threading::lock_type_write);
	
//	LOGI_INFO								("detail", "inserting to hash: '%s' [0x%08x]", node->get_name(), node);

	m_hashset.insert						(hash, node, should_overlap_predicate(mount_id));

#ifndef MASTER_GOLD
	virtual_path_string						node_path;
	node->get_full_path						(& node_path);
	check_consistency						(node, node_path.c_str());
#endif // #ifndef MASTER_GOLD
}

void   vfs_hashset::erase				(u32 hash, base_node<> * node)
{
	threading::reader_writer_lock_raii		raii (get_hashlock(hash), threading::lock_type_write);

//	LOGI_INFO								("detail", "erasing from hash: '%s' [0x%08x]", node->get_name(), node);

	m_hashset.erase							(hash, node);
}

void   vfs_hashset::replace				(u32 hash, base_node<> * with_node, base_node<> * what_node, u32 mount_id)
{
	threading::reader_writer_lock_raii		raii (get_hashlock(hash), threading::lock_type_write);
	m_hashset.erase							(hash, what_node);
	m_hashset.insert						(hash, with_node, should_overlap_predicate(mount_id));

#ifndef MASTER_GOLD
	virtual_path_string						node_path;
	with_node->get_full_path				(& node_path);
	check_consistency						(with_node, node_path.c_str());
#endif // #ifndef MASTER_GOLD
}

base_node<> *   vfs_hashset::skip_nodes_with_wrong_path	(base_node<> * node, pcstr path)
{
	while ( node )
	{
		virtual_path_string					full_path;
		node->get_full_path					(& full_path);
		if ( full_path == path )
			break;

		node							=	node->get_hashset_next();
	}

	return									node;
}

vfs_hashset::begin_end_pair	  vfs_hashset::equal_range (pcstr path, lock_type_enum lock_type)
{
	u32 const hash						=	path_crc32(virtual_path_string(path));
	return									equal_range(path, hash, lock_type);
}

vfs_hashset::begin_end_pair	  vfs_hashset::equal_range (pcstr path, u32 hash, lock_type_enum lock_type)
{
	threading::reader_writer_lock &	lock	=	get_hashlock(hash);
	lock.lock								(to_threading_lock_type(lock_type));

	overlapped_node_initializer				begin;
	begin.node							=	skip_nodes_with_wrong_path(* m_hashset.find(hash), path);
	check_consistency						(begin.node, path);
	if ( begin.node )
		begin.hashset_lock				=	& lock;
	else
		lock.unlock							(to_threading_lock_type(lock_type));

	begin.lock_type						=	lock_type;
	begin.path							=	path;

	return									std::make_pair(begin, overlapped_node_initializer());
}

void   vfs_hashset::check_consistency	(base_node<> * node, pcstr path)
{
	XRAY_UNREFERENCED_PARAMETERS			(path, node);
#ifndef MASTER_GOLD
	while ( node )
	{
		base_node<> * next				=	node->get_hashset_next();
		next							=	skip_nodes_with_wrong_path(next, path);
		if ( next )
			R_ASSERT						(!node->get_next_overlapped() || node->get_next_overlapped() == next);			
		
		node							=	next;
	}

#endif // #ifndef MASTER_GOLD
}

base_node<> *   find_referenced_link_node	(base_node<> const * node)
{
	if ( !node || !node->is_link() )
		return								NULL;

	if ( node->is_hard_link() )
	{
		hard_link_node<> const * const hard_link	=	node_cast<hard_link_node>(node);
		base_node<> * const out_node	=	hard_link->referenced;
		return								out_node;
	}

	mount_root_node_base<> * const mount_root	=	node->get_mount_root();
	vfs_hashset & hashset				=	mount_root->file_system->hashset;

	fs_new::virtual_path_string				full_path;
	node_cast<soft_link_node>(node)->absolute_path_to_referenced	(& full_path);

	base_node<> * const out_referenced_node	=	hashset.find_no_lock(full_path.c_str());

	R_ASSERT								(out_referenced_node);
	return									out_referenced_node;
}


} // namespace vfs
} // namespace xray

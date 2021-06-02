////////////////////////////////////////////////////////////////////////////
//	Created		: 01.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/virtual_file_system.h>
#include "nodes.h"

#define USER_ALLOCATOR						::xray::memory::g_mt_allocator
#include <xray/std_containers.h>
#undef USER_ALLOCATOR

#include <hash_map>

namespace xray {
namespace vfs {

struct hash_traits	{	size_t	operator () (base_node<> * node) const { return (size_t)node; }							};
struct equal_traits	{	bool	operator () (base_node<> * left, base_node<> * right) const { return left == right;	}	};

typedef ::std::hash_map< base_node<> *, base_node<> *, hash_traits, equal_traits, std_allocator< base_node<> * > >	node_hash_map;

void   assert_equal_trees				(base_node<> *			left_node, 
										 base_node<> *			right_node, 
										 node_hash_map &		nodes_map,
										 bool					exact_equality_test,
										 bool					crossed_links)
{
	R_ASSERT								(!left_node == !right_node);
	if ( !left_node && !right_node )
		return;

	if ( left_node->is_link() || right_node->is_link() )
		crossed_links					=	true;

	if ( !crossed_links )
	{
		node_hash_map::iterator	 hash_it	=	nodes_map.find(left_node < right_node ? left_node : right_node);
		if ( hash_it != nodes_map.end() )
		{
			base_node<> * key_in_hash	=	hash_it->first;
			R_ASSERT_U						(key_in_hash == left_node || key_in_hash == right_node);
			base_node<> * node_in_hash	=	hash_it->second;
			R_ASSERT_U						(node_in_hash == left_node || node_in_hash == right_node);
			return;
		}
	}

	R_ASSERT								(strings::equal(left_node->get_name(), right_node->get_name()));

	base_node<> * real_left				=	left_node->is_link() ? find_referenced_link_node(left_node) : left_node; 
	base_node<> * real_right			=	right_node->is_link() ? find_referenced_link_node(right_node) : right_node; 

	if ( exact_equality_test )
		R_ASSERT_CMP						(left_node->get_flags(), ==, right_node->get_flags());
	else
		R_ASSERT							(real_left->is_folder() == real_right->is_folder());

	if ( exact_equality_test && !left_node->is_mount_helper() )
		R_ASSERT							(get_node_physical_path(left_node) == get_node_physical_path(right_node));

	if ( real_left->is_file() )
	{
		R_ASSERT_CMP						(get_raw_file_size(real_left), ==, get_raw_file_size(real_right));

		if ( exact_equality_test )
			R_ASSERT_CMP					(get_file_offs(real_left), ==, get_file_offs(real_right));
	}

	if ( exact_equality_test )
		assert_equal_trees					(left_node->get_next_overlapped(), right_node->get_next_overlapped(), nodes_map, exact_equality_test, crossed_links);

	if ( real_left->is_folder() )
	{
		base_folder_node<> * left_folder	=	node_cast<base_folder_node>(real_left);
		base_folder_node<> * right_folder	=	node_cast<base_folder_node>(real_right);

		u32 left_children_count			=	0;
		for ( base_node<> * left_child	=	left_folder->get_first_child();
							left_child;
							left_child	=	left_child->get_next() )
		{
			base_node<> * right_child	=	right_folder->find_child(left_child->get_name(), NULL);

			base_node<> * left			=	left_child->is_erased() ? NULL : left_child;
			base_node<> * right			=	(right_child && right_child->is_erased()) ? NULL : right_child;

			assert_equal_trees				(left, right, nodes_map, exact_equality_test, crossed_links);
			++left_children_count;
		}

		R_ASSERT_CMP						(calculate_count_of_children(real_right), ==, left_children_count);
	}

	if ( !crossed_links )
	{
		nodes_map.insert					(left_node < right_node ? 
												std::make_pair(left_node, right_node) : 
												std::make_pair(right_node, left_node));
	}
}

void   virtual_file_system::assert_equal	(virtual_file_system & right, bool exact_equality_test)
{
	base_node<> * left_it				=	hashset.find_no_lock("", vfs_hashset::check_locks_false);
	base_node<> * right_it				=	right.hashset.find_no_lock("", vfs_hashset::check_locks_false);

	node_hash_map							nodes_map;
	assert_equal_trees						(left_it, right_it, nodes_map, exact_equality_test, false);
}

} // namespace vfs
} // namespace xray
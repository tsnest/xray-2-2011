////////////////////////////////////////////////////////////////////////////
//	Created		: 30.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include "fs_helper.h"
#include <xray/fs_path_iterator.h>
#include "resources_manager.h"

namespace xray {
namespace fs {

using namespace		resources;

fat_node<> *   file_system_impl::find_node_no_mount (pcstr const path_str, u32 * const out_hash) const
{
	verify_path_is_portable					(path_str);
	u32	const hash						=	path_crc32(path_str, strings::length(path_str));
	if ( out_hash )
		* out_hash						=	hash;

	node_hash_set::const_iterator	it	=	m_hash_set.find(hash);
	fat_node<> * candidate				=	( it != m_hash_set.cend() ) ? *it : NULL;

	fat_node<> const * root_node		=	m_root->cast_node();

	for ( ;candidate; candidate = candidate->get_next_overlapped() )
	{
		if ( is_erased_node(candidate) )
			continue;

		reverse_path_iterator it		=	path_str;
		reverse_path_iterator end_it	=	reverse_path_iterator::end();
		
		fat_node<> *	cur				=	candidate;
		while ( it != end_it )
		{
			if ( !it.equal (cur->m_name) )
				break;

			fat_node<> * parent			=	cur->m_parent;
			if ( !parent->cast_folder()->find_child(cur) )
				break;

			cur							=	parent;
			++it;
			if ( !cur )
				break;	
		}

		if ( it == end_it && cur == root_node )
			return							candidate;
	}

	return									NULL;
}

fat_node<> *   try_sync_mounting (fat_node<> * node, enum_flags<find_enum> find_flags, bool is_full_path, bool traversing_leafs)
{
	if ( node->is_sub_fat() )
	{
		if ( !(find_flags & find_sub_fat) )
			return								NULL;
	}

	if ( need_disk_mount(node, find_flags, is_full_path, traversing_leafs) )
	{
		if ( threading::current_thread_id() != resources::g_resources_manager->resources_thread_id() )
			return								NULL;

		bool const is_recursive				=	(find_flags & find_recursively) && is_full_path;
		fat_node<> * out_result				=	g_fat->impl()->continue_mount_disk	
													(node, NULL, (file_system::recursive_bool)is_recursive);

		R_ASSERT								(out_result);
		return									out_result;
	}

	return										node;
}

fs_iterator   make_iterator (fat_node<> * node, find_enum find_flags)
{
	return											fs_iterator(node, 
																find_referenced_link_node(node),
																find_flags & find_recursively ?
																	fs_iterator::type_recursive : 
																	fs_iterator::type_non_recursive
																);
}

} // namespace fs
} // namespace xray
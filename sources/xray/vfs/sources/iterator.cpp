////////////////////////////////////////////////////////////////////////////
//	Created		: 04.05.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/vfs/iterator.h>
#include "nodes.h"
#include <xray/vfs/hashset.h>

namespace xray {
namespace vfs {

using namespace fs_new;

vfs_iterator::vfs_iterator				() 
	: m_node(NULL), m_link_target(NULL), m_type(type_unset), m_hashset(NULL)
{
}

vfs_iterator::vfs_iterator				(base_node<> * node, base_node<> * link_target, vfs_hashset * hashset, type_enum type)
	: m_node(node), m_link_target(link_target), m_type(type), m_hashset(hashset)
{
	if ( node && node->is_erased() )
	{
		m_node							=	NULL;
		m_link_target					=	NULL;
	}
}

vfs_iterator::vfs_iterator				(vfs_iterator const & it) 
	: m_node(it.m_node), m_link_target(it.m_link_target), m_type(it.m_type), m_hashset(it.m_hashset)
{
}

vfs_iterator	  vfs_iterator::end		()
{
	return									vfs_iterator(NULL, NULL, NULL, type_not_scanned);
}

vfs_iterator	  vfs_iterator::find_child	(virtual_path_string const & relative_path) const
{
	R_ASSERT								(m_node);
	R_ASSERT								(relative_path.length());
	R_ASSERT								(m_type != type_not_scanned);

	if ( relative_path.find(virtual_path_string::separator) != u32(-1) )
		R_ASSERT							(m_type == type_recursive, "this iterator was not requested as recursive, cannot access children of his child");

	virtual_path_string						child_path;
	m_node->get_full_path					(& child_path);

	child_path.append_path					(relative_path);

	base_node<> * const child_node		=	m_hashset->find_no_lock(child_path.c_str(), vfs_hashset::check_locks_true);

	return									vfs_iterator(child_node, find_referenced_link_node(child_node),
													m_hashset, m_type == type_recursive ? type_recursive : type_not_scanned);
}

pcstr   vfs_iterator::get_name			() const
{
	return									m_node->get_name();
}

virtual_path_string	  vfs_iterator::get_virtual_path () const
{
	virtual_path_string						path;
	m_node->get_full_path					(& path);
	return									path;
}

native_path_string   vfs_iterator::get_physical_path () const
{
	return									get_node_physical_path(m_node);
}

u32   vfs_iterator::get_nodes_count		() const
{
	R_ASSERT								(m_type != type_not_scanned);
	return									calculate_count_of_nodes(data_node());
}

u32   vfs_iterator::get_children_count	() const
{
	vfs::base_node<> * const node		=	data_node();
	if ( !node )
		return								0;
	return									calculate_count_of_children(node);
}

bool   vfs_iterator::is_file			() const
{
	return									data_node()->is_file();
}

bool   vfs_iterator::is_folder			() const
{
	return									data_node()->is_folder();
}

bool   vfs_iterator::is_physical		() const
{
	return									data_node()->is_physical();
}

bool   vfs_iterator::is_archive			() const
{
	return									data_node()->is_archive();
}

bool   vfs_iterator::is_replicated		() const
{
	return									m_node->is_replicated();
}

bool   vfs_iterator::is_compressed		() const
{
	return									data_node()->is_compressed();
}

bool    vfs_iterator::is_inlined		() const
{
	return									m_node->is_inlined();
}

bool   vfs_iterator::is_link			() const
{
	return									m_node->is_link();
}

bool   vfs_iterator::link_target_is_inlined	() const
{
	return									data_node()->is_inlined();
}

u32   vfs_iterator::get_file_size		() const
{
	return									vfs::get_file_size(data_node());
}

u32   vfs_iterator::get_compressed_file_size () const
{
	return									vfs::get_compressed_file_size(data_node());
}

u32   vfs_iterator::get_raw_file_size	() const
{
	return									vfs::get_raw_file_size(data_node());
}

file_size_type	 vfs_iterator::get_file_offs () const
{
	return									vfs::get_file_offs(data_node());
}

vfs_iterator   vfs_iterator::children_begin () const
{
	R_ASSERT_CMP							(m_type, !=, type_not_scanned);
	
	base_node<> * const child_node		=	m_node ? data_node()->get_first_child() : NULL;
	base_node<> * const child_link_target	=	find_referenced_link_node(child_node);
	vfs_iterator const out					(child_node, child_link_target,
											 m_hashset,
											 (m_type & type_recursive) ? type_recursive : type_not_scanned);
	return									out;
}

vfs_iterator   vfs_iterator::children_end	() const
{
	R_ASSERT_CMP							(m_type, !=, type_not_scanned);
	return									vfs_iterator::end();
}

vfs_iterator   vfs_iterator::operator ++ ()
{
	while ( m_node && m_node->is_erased() )
		m_node							=	m_node->get_next();

	m_node								=	m_node->get_next();

	if ( m_node )
		m_link_target					=	find_referenced_link_node(m_node);
	else
		m_link_target					=	NULL;

	return									*this;
}

vfs_iterator   vfs_iterator::operator ++	(int)
{
	vfs_iterator	prev_value			=	*this;
	++*this;
	return									prev_value;
}

bool   vfs_iterator::operator ==		(const vfs_iterator& it) const
{
	return									m_node == it.m_node;
}

bool   vfs_iterator::operator !=		(const vfs_iterator& it) const
{
	return									!(*this == it);
}

bool   vfs_iterator::operator <			(const vfs_iterator it) const
{
	return									m_node < it.m_node;
}

u32   vfs_iterator::get_file_hash		() const
{
	CURE_ASSERT								(m_node, return 0);
	return									vfs::get_file_hash(data_node());
}

bool   vfs_iterator::get_inline_data	(const_buffer * out_buffer) const
{
	CURE_ASSERT								(m_node, return NULL);
 	return									vfs::get_inline_data(data_node(), out_buffer);
}

void   vfs_iterator::access_association	(association_callback const & callback) 
{ 
	R_ASSERT								(data_node()); 
	data_node()->access_association			(callback); 
}

bool   vfs_iterator::is_expanded		() const
{
	R_ASSERT								(data_node()); 
	return									data_node()->is_expanded();
}

bool   vfs_iterator::is_archive_check_overlapped	() const
{
	R_ASSERT								(data_node()); 
	return									data_node()->is_archive_check_overlapped();
}

} // namespace vfs
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 03.03.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if 0 

#include "resources_manager.h"
#include <xray/resources_fs.h>
#include <xray/os_preinclude.h>
#include <xray/os_include.h>
#include <xray/fs_utils.h>
#include "fs_nodes.h"
#include "fs_impl_finding.h"

namespace xray {
namespace resources {

using namespace fs;

fs_iterator	  fs_iterator::end ()
{
	return						fs_iterator(NULL, NULL, type_not_scanned);
}

fs_iterator	  fs_iterator::find_child (pcstr relative_path)
{
	R_ASSERT					(m_fat_node);
	R_ASSERT					(m_type != type_not_scanned);
	if ( strchr(relative_path, '/') )
		R_ASSERT				(m_type == type_recursive, "this iterator was not requested as recursive, cannot access children of his child");

	path_string					full_path;
	get_full_path				(full_path);
	full_path				+=	'/';
	full_path				+=	relative_path;

	find_results_struct			child_find_struct(find_results_struct::type_no_pin);
	fs_iterator					out_result;
	bool const sync_result	=	g_fat->try_find_sync_no_pin(& out_result, 
															& child_find_struct,
															full_path.c_str());
	R_ASSERT_U					(sync_result);

	return						fs_iterator(out_result.get_fat_node(), out_result.get_link_target(),
											m_type == type_recursive ? type_recursive : type_not_scanned);
}

pcstr   fs_iterator::get_name () const
{
	return						m_fat_node->get_name();
}

void   fs_iterator::get_full_path (path_string & path) const
{
	m_fat_node->get_full_path	(path);
}

path_string	  fs_iterator::get_full_path () const
{
	path_string					out_full_path;
	get_full_path				(out_full_path);
	return						out_full_path;
}

void   fs_iterator::get_disk_path (path_string & path) const
{
	g_fat->get_disk_path		(m_fat_node, path);
}

u32   fs_iterator::get_num_children () const
{
	R_ASSERT					(m_type != type_not_scanned);

	return						follow_link_node()->get_num_children();
}

u32   fs_iterator::get_num_nodes () const
{
	return						follow_link_node()->get_num_nodes();
}

bool   fs_iterator::is_folder () const
{
	return						m_fat_node->is_folder();
}

bool   fs_iterator::is_disk () const
{
	return						follow_link_node()->is_disk();
}

bool   fs_iterator::is_db () const
{
	return						follow_link_node()->is_db();
}

bool   fs_iterator::is_replicated () const
{
	return						m_fat_node->is_replicated();
}

bool   fs_iterator::is_compressed () const
{
	return						follow_link_node()->is_compressed();
}

bool    fs_iterator::is_inlined () const
{
	return						follow_link_node()->is_inlined();
}

bool   fs_iterator::is_inlined_follow_link	() const
{
	return						follow_link_node()->has_flags_follow_link(file_system::is_inlined);
}

u32   fs_iterator::get_file_size () const
{
	return						follow_link_node()->get_file_size();
}

u32   fs_iterator::get_compressed_file_size () const
{
	return						follow_link_node()->get_compressed_file_size();
}

u32   fs_iterator::get_raw_file_size () const
{
	return						follow_link_node()->get_raw_file_size();
}

file_size_type	 fs_iterator::get_file_offs () const
{
	return						follow_link_node()->get_file_offs();
}

fs_iterator   fs_iterator::children_begin () const
{
	R_ASSERT_CMP				(m_type, !=, type_not_scanned);
	
	fat_node<> * const child_node	=	m_fat_node ? follow_link_node()->get_first_child() : NULL;
	fat_node<> * const child_link_target	=	fs::find_referenced_link_node(child_node);
	fs_iterator const out		(child_node, child_link_target,
								 (m_type & type_recursive) ? type_recursive : type_not_scanned);
	return						out;
}

fs_iterator   fs_iterator::children_end	() const
{
	R_ASSERT_CMP				(m_type, !=, type_not_scanned);
	return						fs_iterator::end();
}

fs_iterator   fs_iterator::operator ++ ()
{
	m_fat_node				=	m_fat_node->get_next();
	m_link_target			=	fs::find_referenced_link_node(m_fat_node);

	return						*this;
}

fs_iterator   fs_iterator::operator ++	(int)
{
	fs_iterator	prev_value	=	*this;
	++*this;
	return						prev_value;
}

bool   fs_iterator::operator == (const fs_iterator& it) const
{
	return						m_fat_node == it.m_fat_node;
}

bool   fs_iterator::operator !=	(const fs_iterator& it) const
{
	return						!(*this == it);
}

bool   fs_iterator::operator <	(const fs_iterator it) const
{
	return						m_fat_node < it.m_fat_node;
}

void   fs_iterator::set_is_locked (bool lock)
{
	set_fat_node_is_locked		(follow_link_node(), lock);
}

void   fs_iterator::set_associated (resource_base * const resource)
{
	CURE_ASSERT						(m_fat_node, return);
	follow_link_node()->set_associated	(resource);
}

bool   fs_iterator::get_hash (u32 * out_hash) const
{
	ASSERT						(m_fat_node);
	return						follow_link_node()->get_hash(out_hash);
}

managed_resource_ptr   fs_iterator::get_associated_managed_resource_ptr () const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->get_associated_managed_resource_ptr();
}

unmanaged_resource_ptr   fs_iterator::get_associated_unmanaged_resource_ptr () const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->get_associated_unmanaged_resource_ptr();
}

query_result *   fs_iterator::get_associated_query_result () const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->get_associated_query_result();
}

bool   fs_iterator::get_inline_data (const_buffer * out_buffer) const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->get_inline_data(out_buffer);
}

bool   fs_iterator::is_associated () const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->is_associated();
}

bool   fs_iterator::is_associated_with (resource_base * const resource_base) const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->is_associated_with(resource_base);
}

bool   fs_iterator::try_clean_associated (u32 allow_reference_count) const
{
	CURE_ASSERT					(m_fat_node, return NULL);
	return						follow_link_node()->try_clean_associated(allow_reference_count);
}

void   sub_fat_pin_fs_iterator::assign (fs_iterator it)
{
	assign_impl								(it.get_fat_node(), it.get_link_target(), it.m_type, true);
}

void   sub_fat_pin_fs_iterator::assign_no_pin (fs_iterator it)
{
	assign_no_pin							(it.get_fat_node(), it.get_link_target(), it.m_type);
}

void   sub_fat_pin_fs_iterator::assign_no_pin (fat_node<> * node, fat_node<> * link_target, type_enum type)
{
	assign_impl								(node, link_target, type, false);
}

void   sub_fat_pin_fs_iterator::assign (fat_node<> * node, fat_node<> * link_target, type_enum type)
{
	assign_impl								(node, link_target, type, true);
}

void   sub_fat_pin_fs_iterator::assign_impl (fat_node<> * node, fat_node<> * link_target, type_enum type, bool do_pin)
{
	if ( m_fat_node == node )
		return;

	unpin_sub_fats_if_needed				();
	* (fs_iterator *)(this)				=	fs_iterator(node, link_target, type);

	if ( !m_fat_node )
		return;

	if ( do_pin )
	{
		pin_sub_fats_of_node			(+1, m_fat_node, 
											 m_type == type_recursive ? find_recursively : (find_enum)0);
	}
}

sub_fat_pin_fs_iterator::~sub_fat_pin_fs_iterator ()
{
	unpin_sub_fats_if_needed	();
}

void   sub_fat_pin_fs_iterator::grab (sub_fat_pin_fs_iterator & other)
{
	unpin_sub_fats_if_needed	();
	m_fat_node				=	other.m_fat_node;
	m_link_target			=	other.m_link_target;
	other.m_fat_node		=	NULL;
}

void   sub_fat_pin_fs_iterator::unpin_sub_fats_if_needed ()
{
	if ( !m_fat_node )
		return;

	pin_sub_fats_of_node				(-1, m_fat_node, 
											 m_type == type_recursive ? find_recursively : (find_enum)0);
}

void   sub_fat_pin_fs_iterator::clear_without_unpin ()
{
	R_ASSERT					(m_fat_node);
	m_fat_node				=	NULL;
	m_link_target			=	NULL;
}

} // namespace resources
} // namespace xray


#endif // #if 0
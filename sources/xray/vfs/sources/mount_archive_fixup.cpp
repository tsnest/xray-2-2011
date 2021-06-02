////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "mount_archive.h"

namespace xray {
namespace vfs {

using namespace fs_new;

struct fixup_node
{
	base_node<> *						node;
	pstr								buffer_origin;
	mount_root_node_base<> *			mount_root;

	fixup_node	(base_node<> * const node, pstr const buffer_origin, mount_root_node_base<> * mount_root)
		: node(node), buffer_origin(buffer_origin), mount_root(mount_root)
	{
		if ( node->is_soft_link() )
			fixup_soft_link_node			();
		else if ( node->is_hard_link() )
			fixup_hard_link_node			();
		else if ( node->is_mount_root() )
			fixup_folder_mount_root			();
		else if ( node->is_folder() )
			fixup_folder_node				();
		else if ( node->is_external_sub_fat() )
			fixup_external_subfat_node		();
		else if ( node->is_inlined() )
			fixup_inlined_node				();
		else if ( node->is_erased() )
			fixup_erased_node				();

		fixup_base_node						();
	}

private:
	void   fixup_base_node				()
	{
		size_t const parent_offs		=	(size_t)((base_folder_node<> *)node->get_parent());
		node->set_parent					(parent_offs ? (base_folder_node<> *)(buffer_origin + parent_offs) : NULL);

		size_t const next_offs			=	(size_t)((base_node<> *)node->get_next());
		node->set_next						(next_offs ? (base_node<> *)(buffer_origin + next_offs) : NULL);

		if ( node_cast<base_node>(mount_root) != node )
		{
			base_node<>::mount_root_pointer		mount_root_pointer;
			mount_root_pointer			=	mount_root;
			node->set_mount_root			(mount_root_pointer);
		}
		else
			node->set_mount_helper_parent	(NULL);
	}

	void   fixup_erased_node			()
	{
		erased_node<> * const erased	=	node_cast<erased_node>(node);
		R_ASSERT							(erased);
		XRAY_UNREFERENCED_PARAMETER			(erased);
	}

	void   fixup_external_subfat_node	()
	{
		external_subfat_node<> * const external_subfat	=	node_cast<external_subfat_node>(node);
		R_ASSERT							(external_subfat);
		size_t const relative_path_offs	=	(size_t)((pvoid)external_subfat->relative_path_to_external);
		R_ASSERT							(relative_path_offs);
		external_subfat->relative_path_to_external	=	(pstr)(buffer_origin + relative_path_offs);
	}

	void   fixup_soft_link_node			()
	{
		soft_link_node<> * soft_link	=	node_cast<soft_link_node>(node);
		size_t const relative_path_offs	=	(size_t)((pvoid)soft_link->relative_path);
		R_ASSERT							(relative_path_offs);
		soft_link->relative_path		=	(pstr)(buffer_origin + relative_path_offs);
	}

	void   fixup_hard_link_node			()
	{
		hard_link_node<> * hard_link	=	node_cast<hard_link_node>(node);
		size_t const reference_offs		=	(size_t)((base_node<> *)hard_link->referenced);
		hard_link->referenced			=	reference_offs ? (base_node<> *)(buffer_origin + reference_offs) : NULL;
	}

	void   fixup_inlined_node			()
	{
		const_buffer							inline_data;
		if ( get_inline_data(node, & inline_data) )
		{
			size_t const data_offs		=	(size_t)inline_data.c_ptr();
			R_ASSERT						(data_offs);
			set_inline_data					(node, memory::buffer(buffer_origin + data_offs, inline_data.size()));
		}
		else
			UNREACHABLE_CODE();
	}

	void   fixup_folder_node			()
	{
		base_folder_node<> * folder_node	=	node_cast<base_folder_node>(node);

		size_t const	 child_offs		=	reinterpret_cast<size_t>((base_node<> *)folder_node->get_first_child());
		folder_node->set_first_child		(child_offs ? reinterpret_cast<base_node<> *>(buffer_origin + child_offs) : NULL);
	}

	void   fixup_folder_mount_root		()
	{
		fixup_folder_node					();

		archive_folder_mount_root_node<> * const mount_root	=	node_cast<archive_folder_mount_root_node>(node);
		new (& mount_root->virtual_path_holder)		virtual_path_string;
		new (& mount_root->fat_path_holder)			native_path_string;
		new (& mount_root->archive_path_holder)		native_path_string;
		new (& mount_root->descriptor)				fixed_string<32>;
		mount_root->node					=	node;
	}
}; // struct fixup_node

void   archive_mounter::recursive_fixup_node	(base_node<> * const node, pstr const buffer_origin)
{
	if ( m_reverse_byte_order )
		node->reverse_bytes_for_final_class	(reverse_direction_to_native);

	fixup_node								(node, buffer_origin, m_mount_root_base);

	base_folder_node<> * node_as_folder	=	node->is_link() ? NULL : node_cast<base_folder_node>(node);
	base_node<> * it_child				=	node_as_folder ? node_as_folder->get_first_child() : NULL;
	while ( it_child )
	{
		recursive_fixup_node				(it_child, buffer_origin);
		it_child						=	it_child->get_next();
	}
}

} // namespace vfs
} // namespace xray
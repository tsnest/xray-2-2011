////////////////////////////////////////////////////////////////////////////
//	Created		: 04.11.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "saving_info_tree.h"
#include <xray/fs/path_string_utils.h>
#include <xray/fs/device_utils.h>

namespace xray {
namespace vfs   {

//-------------------------------------------------------
// struct fat_node_info
//-------------------------------------------------------

static fat_node_info const *   top_parent (fat_node_info const * node)
{
	fat_node_info const * out_parent	=	node;
	while ( out_parent->parent )
		out_parent						=	out_parent->parent;
	return									out_parent;
}

fat_node_info const *   fat_node_info::get_subfat_root () const
{
	fat_node_info const *	child		=	this;
	for ( fat_node_info const *	it_node	=	parent;
							it_node;
							child		=	it_node,
							it_node		=	it_node->parent	)
	{
		if ( it_node->color != child->color )
			return							child;
	}

	return									child;
}

bool   fat_node_info::same_color		(fat_node_info const * other) const
{
	if ( color != other->color )
		return								false;

	fat_node_info const * const self_subfat_root	=	get_subfat_root();
	fat_node_info const * const other_subfat_root	=	other->get_subfat_root();
	if ( self_subfat_root != other_subfat_root )
		return								false;

	R_ASSERT_U								(top_parent(this) == top_parent(other));
	return									true;
}

void   fat_node_info::append_child		(fat_node_info * child)
{
	child->parent						=	this;
	if ( !first_child )
	{
		first_child						=	child;
		return;
	}
	
	fat_node_info * last_child			=	first_child;
	while ( last_child->next )
		last_child						=	last_child->next;

	last_child->next					=	child;
}

void   fat_node_info::swap_child_with_next_child (fat_node_info * child)
{
	fat_node_info * const next_node		=	child->next;
	ASSERT									(next_node);
	if ( child == first_child )
	{
		child->next						=	next_node->next;
		next_node->next					=	child;
		first_child						=	next_node;
		return;
	}

	fat_node_info * previous_node		=	first_child;
	while ( previous_node->next != child )
		previous_node					=	previous_node->next;
	
	previous_node->next					=	next_node;
	child->next							=	next_node->next;
	next_node->next						=	child;
}

fat_node_info *   allocate_new_info_node (mutable_buffer &	allocator)
{
	fat_node_info * const out_node		=	(fat_node_info *)allocator.c_ptr();
	new ( out_node )						fat_node_info;
	allocator							+=	sizeof(fat_node_info);
	return									out_node;
}

fat_node_info *   make_info_tree		(fs_new::synchronous_device_interface &	device, 
										 base_node<> *							node, 
										 mutable_buffer &						allocator_for_info)
{
	fat_node_info * const out_node		=	allocate_new_info_node(allocator_for_info);
	out_node->node						=	node;
	vfs_association * const previous_association	=	node->get_association_unsafe();
	R_ASSERT_U								(!previous_association);
	node->set_association_unsafe			(out_node);

	if ( node->is_folder() )
	{
		base_folder_node<> * folder		=	node_cast<base_folder_node>(node);
		for ( base_node<> *	it_child	=	folder->get_first_child();
							it_child;
							it_child	=	it_child->get_next() )
 		{
			fat_node_info * info_child	=	make_info_tree(device, it_child, allocator_for_info);
			out_node->hash				=	fs_new::crc32(out_node->hash, info_child->hash);
			out_node->append_child			(info_child);
		}
	}

	if ( !node->is_erased() && !node->is_folder() )
	{
		fs_new::native_path_string const physical_path	=	get_node_physical_path (node);
		bool const calculated_file_hash	=	fs_new::calculate_file_hash(device, physical_path, & out_node->hash);
		CURE_ASSERT							(calculated_file_hash, out_node->hash = 0);
	}

	return									out_node;
}

void   calculate_sizes_for_folder_node	(base_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	u32 size							=	node->sizeof_with_name();
	base_node<> *		it_child		=	node_cast<base_folder_node>(node)->get_first_child();
	fat_node_info * it_info_child		=	node_info->first_child;
	while ( it_child )
	{
		calculate_sizes_for_info_tree		(it_child, it_info_child, inline_data);
		size							+=	it_info_child->fat_size_with_children;

		it_child						=	it_child->get_next();
		it_info_child					=	it_info_child->next;
	}

	node_info->fat_size_with_children	=	size;
	node_info->fat_size					=	node->sizeof_with_name();
}

void   calculate_sizes_for_file_node	(base_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	u32 size							=	sizeof(archive_file_node<>) + strings::length(node->get_name());
	
	if ( !node->is_erased() )
	{
		u32 node_raw_file_size			=	get_raw_file_size(node);
		if ( inline_data.try_fit_for_inlining(node->get_name(), node_raw_file_size, 0) )
			size						+=	node_raw_file_size;
	}

	node_info->fat_size_with_children	=	size;
	node_info->fat_size					=	size;
}

void   calculate_sizes_for_info_tree	(base_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	if ( node->is_link() )
		return;

	if ( node->is_folder() )
		calculate_sizes_for_folder_node		(node, node_info, inline_data);
	else 
		calculate_sizes_for_file_node		(node, node_info, inline_data);
}

void   swap_child_with_next_child		(base_folder_node<> * parent, base_node<> * child, base_node<> * prev_child)
{
	base_node<> * next_child			=	child->get_next();
	child->set_next							(next_child->get_next());
	next_child->set_next					(child);

	if ( !prev_child )
	{
		R_ASSERT							(parent->get_first_child() == child);
		parent->set_first_child				(next_child);
	}
	else
		prev_child->set_next				(next_child);

}

bool   try_swap_some_children			(base_node<> * node, fat_node_info * node_info)
{
	bool out_swapped					=	false;
	base_folder_node<> * node_folder	=	node_cast<base_folder_node>(node);
	base_node<> *		it_child		=	node_folder->get_first_child();
	base_node<> *		it_prev_child	=	NULL;
	fat_node_info * it_info_child		=	node_info->first_child;
	while ( it_child )
	{
		R_ASSERT							(it_child == it_info_child->node);
		base_node<> * const		next_child		=	it_child->get_next();
		fat_node_info * const	next_info_child	=	it_info_child->next;

		if ( !next_child )
			break;
			
		if ( next_info_child->fat_size_with_children < it_info_child->fat_size_with_children )
		{
			swap_child_with_next_child		(node_folder, it_child, it_prev_child);
			it_prev_child				=	next_child;
			node_info->swap_child_with_next_child	(it_info_child);
			out_swapped					=	true;
			continue;
		}

		it_prev_child					=	it_child;
		it_child						=	next_child;
		it_info_child					=	next_info_child;
	}

	return									out_swapped;
}

void   sort_info_tree_folder			(base_node<> * node, fat_node_info * node_info)
{
	ASSERT									(node->is_folder());

	while ( try_swap_some_children(node, node_info) ) ;
}

void   sort_info_tree					(base_node<> * node, fat_node_info * node_info)
{
	if ( !node->is_folder() )
		return;

	sort_info_tree_folder					(node, node_info);

	base_node<> *		it_child		=	node_cast<base_folder_node>(node)->get_first_child();
	fat_node_info * it_info_child		=	node_info->first_child;
	while ( it_child )
	{
		sort_info_tree						(it_child, it_info_child);

		it_child						=	it_child->get_next();
		it_info_child					=	it_info_child->next;
	}
}

void   apply_color						(fat_node_info *	node_info, u32 color)
{
	node_info->set_color					(color);
	for ( fat_node_info *	it_child	=	node_info->first_child;
							it_child;
							it_child	=	it_child->next )
	{
		apply_color							(it_child, color);
	}
}

void   advance_color					(u32 & allowed_size, u32 size_to_subtract, 
										 u32 max_size_per_color, u32 & color)
{
	if ( allowed_size < size_to_subtract )
	{
		++color;
		allowed_size					=	max_size_per_color;
	}
	else
		allowed_size					-=	size_to_subtract;
}

void   paint_subfats 					(fat_node_info *	node_info, 
										 u32 &				allowed_size, 
										 u32				max_size_per_color, 
										 u32 &				color)
{
	bool coloured_current				=	false;
	u32 color_for_self_and_leaf			=	u32(-1);

	for ( fat_node_info *	it_child	=	node_info->first_child;
							it_child;
							it_child	=	it_child->next )
	{
		bool const child_is_leaf		=	(it_child->first_child == 0);
		if ( child_is_leaf )
		{
			if ( color_for_self_and_leaf == u32(-1) )
			{
				if ( it_child != node_info->first_child )
				{
					++color;
					allowed_size		=	max_size_per_color;
				}
				color_for_self_and_leaf	=	color;
			}

			it_child->set_color				(color_for_self_and_leaf);
			if ( color == color_for_self_and_leaf )
				advance_color				(allowed_size, it_child->fat_size, max_size_per_color, color);
		}
		else
		{
			paint_subfats					(it_child, allowed_size, max_size_per_color, color);		
			if ( coloured_current && color_for_self_and_leaf != color )
			{
				++color;
				allowed_size			=	max_size_per_color;
			}
		}

		if ( !coloured_current )
		{
			if ( child_is_leaf )
			{
				node_info->set_color		(color_for_self_and_leaf);
				if ( color == color_for_self_and_leaf )
					advance_color			(allowed_size, node_info->fat_size, max_size_per_color, color);

				coloured_current		=	true;
			}
			else
			{
				node_info->set_color		(color);
				color_for_self_and_leaf	=	color;

				advance_color				(allowed_size, node_info->fat_size, max_size_per_color, color);
				coloured_current		=	true;
			}
		}
	}

	if ( coloured_current )
		return;

	advance_color								(allowed_size, node_info->fat_size, max_size_per_color, color);
	node_info->set_color						(color);
}

u32		split_into_archive_parts		(fat_node_info * const	node_info, 
										 u32 const				archive_part_max_size)
{
	u32 out_parts_count					=	0;
	node_info->archive_size_with_children	=	0;
	if ( !node_info->node->is_erased() && node_info->node->is_file() )
		node_info->archive_size_with_children	+=	get_file_size(node_info->node);

	for ( fat_node_info *	it_child	=	node_info->first_child;
							it_child;
							it_child	=	it_child->next )
	{
		out_parts_count					+=	split_into_archive_parts(it_child, archive_part_max_size);
		node_info->archive_size_with_children	+=	it_child->archive_size_with_children;
	}

	bool const is_subfat				=	node_info->parent != NULL &&
											node_info->get_color() != node_info->parent->get_color();

	if ( is_subfat && node_info->archive_size_with_children > archive_part_max_size )
	{
		node_info->is_archive_part		=	true;
		node_info->archive_size_with_children	=	0;
		++out_parts_count;
	}
	
	return									out_parts_count;
}

} // namespace vfs
} // namespace xray



////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_utils.h>
#include "fs_impl_saving_info_tree.h"
#include "fs_impl.h"
#include "fs_helper.h"

namespace xray {
namespace fs   {

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

bool   fat_node_info::same_color (fat_node_info const * other) const
{
	if ( color != other->color )
		return								false;

	R_ASSERT_U								(top_parent(this) == top_parent(other));
	return									true;
}

void   fat_node_info::append_child (fat_node_info * child)
{
	child->parent						=	this;
	if ( !first_child )
	{
		first_child						=	child;
		return;
	}
	
	fat_node_info * last_child		=	first_child;
	while ( last_child->next )
		last_child						=	last_child->next;

	last_child->next					=	child;
}

void   fat_node_info::swap_child_with_next_child (fat_node_info * child)
{
	fat_node_info * const next_node	=	child->next;
	ASSERT									(next_node);
	if ( child == first_child )
	{
		child->next						=	next_node->next;
		next_node->next					=	child;
		first_child						=	next_node;
		return;
	}

	fat_node_info * previous_node	=	first_child;
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

fat_node_info *   make_info_tree (file_system_impl & fs_impl, fat_node<> * node, mutable_buffer & allocator_for_info)
{
	fat_node_info * const out_node		=	allocate_new_info_node(allocator_for_info);
	out_node->node						=	node;

	if ( node->is_folder() )
	{
		for ( fat_node<> *	it_child	=	node->cast_folder()->get_first_child();
							it_child;
							it_child	=	it_child->get_next() )
 		{
			fat_node_info * info_child	=	make_info_tree(fs_impl, it_child, allocator_for_info);
			out_node->hash				=	crc32(out_node->hash, info_child->hash);
			out_node->append_child			(info_child);
		}
	}

	if ( !node->is_folder() )
	{
			path_string						disk_path;
			fs_impl.get_disk_path			(node, disk_path);
		bool const calculated_file_hash	=	calculate_file_hash(& out_node->hash, disk_path.c_str());
		CURE_ASSERT							(calculated_file_hash, out_node->hash = 0);
	}

	return									out_node;
}

void   calculate_sizes_for_folder_node (fat_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	u32 size							=	node->get_sizeof();
	fat_node<> *		it_child		=	node->cast_folder()->get_first_child();
	fat_node_info * it_info_child	=	node_info->first_child;
	while ( it_child )
	{
		calculate_sizes_for_info_tree		(it_child, it_info_child, inline_data);
		size							+=	it_info_child->size_with_children;

		it_child						=	it_child->get_next();
		it_info_child					=	it_info_child->next;
	}

	node_info->size_with_children		=	size;
	node_info->size						=	node->get_sizeof();
}

void   calculate_sizes_for_file_node (fat_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	u32 size							=	node->get_sizeof();
	if ( inline_data.try_fit_for_inlining(node->get_name(), node->get_raw_file_size(), 0) )
		size							+=	node->get_raw_file_size();

	node_info->size_with_children		=	size;
	node_info->size						=	size;
}

void   calculate_sizes_for_info_tree (fat_node<> * node, fat_node_info * node_info, fat_inline_data & inline_data)
{
	if ( node->is_folder() )
		calculate_sizes_for_folder_node		(node, node_info, inline_data);
	else 
		calculate_sizes_for_file_node		(node, node_info, inline_data);
}

bool   try_swap_some_children (fat_node<> * node, fat_node_info * node_info)
{
	bool out_swapped					=	false;
	fat_node<> *		it_child		=	node->cast_folder()->get_first_child();
	fat_node_info * it_info_child		=	node_info->first_child;
	while ( it_child )
	{
		fat_node<> * const			next_child		=	it_child->get_next();
		fat_node_info * const	next_info_child	=	it_info_child->next;

		if ( !next_child )
			break;
			
		if ( next_info_child->size_with_children < it_info_child->size_with_children )
		{
			node->cast_folder()->swap_child_with_next_child(it_child);
			node_info->swap_child_with_next_child	(it_info_child);
			out_swapped					=	true;
			continue;
		}

		it_child						=	next_child;
		it_info_child					=	next_info_child;
	}

	return									out_swapped;
}

void   sort_info_tree_folder (fat_node<> * node, fat_node_info * node_info)
{
	ASSERT									(node->is_folder());

	while ( try_swap_some_children(node, node_info) ) ;
}

void   sort_info_tree (fat_node<> * node, fat_node_info * node_info)
{
	if ( !node->is_folder() )
		return;

	sort_info_tree_folder					(node, node_info);

	fat_node<> *		it_child		=	node->cast_folder()->get_first_child();
	fat_node_info * it_info_child		=	node_info->first_child;
	while ( it_child )
	{
		sort_info_tree						(it_child, it_info_child);

		it_child						=	it_child->get_next();
		it_info_child					=	it_info_child->next;
	}
}

void   apply_color (fat_node_info *	node_info, u32 color)
{
	node_info->set_color					(color);
	for ( fat_node_info *	it_child	=	node_info->first_child;
								it_child;
								it_child	=	it_child->next )
	{
		apply_color							(it_child, color);
	}
}

void   advance_color (u32 &	allowed_size, 
					 u32	size_to_subtract,
					 u32	max_size_per_color, 
					 u32 &	color)
{
	if ( allowed_size < size_to_subtract )
	{
		++color;
		allowed_size					=	max_size_per_color;
	}
	else
	{
		allowed_size					-=	size_to_subtract;
	}
}

void   paint_info_tree (fat_node_info *	node_info, 
						u32 &				allowed_size, 
						u32					max_size_per_color, 
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
				advance_color				(allowed_size, it_child->size, max_size_per_color, color);
		}
		else
		{
			paint_info_tree					(it_child, allowed_size, max_size_per_color, color);		
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
				node_info->set_color			(color_for_self_and_leaf);
				if ( color == color_for_self_and_leaf )
					advance_color				(allowed_size, node_info->size, max_size_per_color, color);

				coloured_current			=	true;
			}
			else if ( allowed_size >= node_info->size )
			{
				node_info->set_color			(color);
				color_for_self_and_leaf		=	color;

				advance_color					(allowed_size, node_info->size, max_size_per_color, color);
				coloured_current			=	true;
			}
		}
	}

	if ( coloured_current )
		return;

	advance_color								(allowed_size, node_info->size, max_size_per_color, color);
	node_info->set_color						(color);
}

} // namespace fs
} // namespace xray



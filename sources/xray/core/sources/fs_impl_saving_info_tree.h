////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef FS_FILE_SYSTEM_IMPL_SAVING_INFO_TREE_H_INCLUDED
#define FS_FILE_SYSTEM_IMPL_SAVING_INFO_TREE_H_INCLUDED

#include "fs_file_system.h"

namespace xray {
namespace fs {

struct	fat_node_info;

struct fat_node_info
{
public:
	u32					size;
	u32					size_with_children;
	fat_node_info *		next;
	fat_node_info *		parent;
	fat_node_info *		first_child;
	fat_node_info *		next_in_hashset;
	u32					hash;

	fat_node<> *		node;
	pvoid				saved_node;
	file_size_type		pos;
	u32					compressed_size;
	bool				is_compressed;

	fat_node_info ()
		:	size(0), size_with_children(0), color(u32(-1)), next(NULL), first_child(NULL), 
			parent(NULL), hash(NULL), node(NULL), is_compressed(false), saved_node(NULL) { }

public:
	bool				same_color						(fat_node_info const *	other) const;
	void				append_child					(fat_node_info *	child);
	void				swap_child_with_next_child		(fat_node_info *	child);

	void				set_color						(u32 in_color) { color = in_color; }
	u32					get_color						() const { return color; }

private:
	u32					color;
};

typedef hash_multiset< fat_node_info, fat_node_info *, & fat_node_info::next_in_hashset, xray::detail::fixed_size_policy<1024*32> >	fat_node_info_set;


void					sort_info_tree					(fat_node<> *			node, 
														 fat_node_info *		node_info);
void					calculate_sizes_for_info_tree	(fat_node<> *			node, 
														 fat_node_info *		node_info, 
														 fat_inline_data &		inline_data);
fat_node_info *			make_info_tree					(file_system_impl &		fs_impl, 
														 fat_node<> *			node, 
														 mutable_buffer &		allocator_for_info);

void					paint_info_tree					(fat_node_info *		node_info, 
														 u32 &					allowed_size,
														 u32					max_size_per_color, 
														 u32 &					color);

} // namespace fs
} // namespace xray

#endif // #ifndef FS_FILE_SYSTEM_IMPL_SAVING_INFO_TREE_H_INCLUDED
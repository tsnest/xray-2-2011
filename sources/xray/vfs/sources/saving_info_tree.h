////////////////////////////////////////////////////////////////////////////
//	Created		: 06.10.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef VFS_SAVING_INFO_TREE_H_INCLUDED
#define VFS_SAVING_INFO_TREE_H_INCLUDED

#include "nodes.h"
#include <xray/vfs/saving_fat_inline_data.h>

namespace xray {
namespace vfs {

struct fat_node_info : public vfs_association
{
public:
	u32					fat_size;
	u32					fat_size_with_children;
	u32					archive_size_with_children;
	fat_node_info *		next;
	fat_node_info *		parent;
	fat_node_info *		first_child;
	fat_node_info *		next_in_hashset;
	u32					hash;
	fat_node_info *		link_target;

	base_node<> *		node;
	pvoid				saved_node;
	file_size_type		pos;
	u32					compressed_size;
	bool				is_compressed;
	bool				is_archive_part;
	bool				can_be_referenced;
	
	fat_node_info ()
		:	vfs_association(0), fat_size(0), fat_size_with_children(0),
			archive_size_with_children(0), color(u32(-1)), next(NULL), first_child(NULL), link_target(NULL), 
			parent(NULL), hash(NULL), node(NULL), is_compressed(false), is_archive_part(false), saved_node(NULL),
			pos(0), compressed_size(0), next_in_hashset(NULL), can_be_referenced(true) { }

public:
	bool				same_color						(fat_node_info const *	other) const;
	void				append_child					(fat_node_info *	child);
	void				swap_child_with_next_child		(fat_node_info *	child);

	void				set_color						(u32 in_color) { color = in_color; }
	u32					get_color						() const { return color; }
	fat_node_info const *	get_subfat_root				() const;


private:
	u32					color;
};

typedef hash_multiset< fat_node_info, fat_node_info *, & fat_node_info::next_in_hashset, 
					   xray::detail::fixed_size_policy<1024*32> >	fat_node_info_set;


void					sort_info_tree					(base_node<> *			node, 
														 fat_node_info *		node_info);
void					calculate_sizes_for_info_tree	(base_node<> *			node, 
														 fat_node_info *		node_info, 
														 fat_inline_data &		inline_data);
fat_node_info *			make_info_tree					(fs_new::synchronous_device_interface &	device, 
														 base_node<> *			node, 
														 mutable_buffer &		allocator_for_info);

void					paint_subfats					(fat_node_info *		node_info, 
														 u32 &					allowed_size,
														 u32					max_size_per_color, 
														 u32 &					color);

u32						split_into_archive_parts		(fat_node_info *		node_info, 
														 u32					archive_part_max_size);

} // namespace vfs
} // namespace xray

#endif // #ifndef VFS_SAVING_INFO_TREE_H_INCLUDED
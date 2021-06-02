////////////////////////////////////////////////////////////////////////////
//	Created		: 27.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "saving.h"
#include <xray/fs/device_utils.h>
#include <xray/fs/file_type_pointer.h>
#include <xray/compressor_ppmd.h>
#include <xray/vfs/virtual_file_system.h>

namespace xray {
namespace vfs {

using namespace fs_new;

fat_node_info *		strip_link			(fat_node_info * info)
{
	if ( !info->node->is_link() )
		return								info;

	base_node<> * const referenced		=	find_referenced_link_node(info->node);
	
	vfs_association * const association	=	referenced->get_association_unsafe();
	R_ASSERT								(association);

	fat_node_info * const out_info		=	static_cast_checked<fat_node_info *>(association);
	return									out_info;	
}

fat_node_info *   get_info_by_node		(base_node<> * node)
{
	vfs_association * const association	=	node->get_association_unsafe();
	R_ASSERT								(association);

	fat_node_info * const out_info		=	static_cast_checked<fat_node_info *>(association);
	return									out_info;
}

class compare_nodes
{
public:
	compare_nodes						(synchronous_device_interface *			device, 
										 compressor * const						compressor,
										 memory::base_allocator * const			allocator)
	:	m_device(device), m_compressor(compressor), m_allocator(allocator) {}

	bool   equal_nodes						(fat_node_info * const first_info, fat_node_info * const second_info);

private:
	bool   equal_nodes_impl					(fat_node_info * const first_info, fat_node_info * const second_info, bool check_name);
	bool   equal_folder_nodes 				(fat_node_info * const first_info, fat_node_info * const second_info);
	bool   equal_file_nodes					(base_node<> * const left, base_node<> * const right);
	void   read_file_data					(allocated_buffer * const out_buffer, base_node<> * const node);

private:
	synchronous_device_interface *		m_device;
	compressor *						m_compressor;
	memory::base_allocator *			m_allocator;
};

void   compare_nodes::read_file_data	(allocated_buffer * const out_buffer, base_node<> * const node)
{
	native_path_string const file_path	=	get_node_physical_path(node);

	file_type_pointer const file			(file_path, * m_device, file_mode::open_existing, file_access::read);
	R_ASSERT								(file, "cannot open file '%s'", file_path.c_str());
	if ( node->is_compressed() )
	{
		decompress_node						(node, out_buffer, * m_device, m_allocator, m_compressor);
	}
	else
	{
		u32 const file_size				=	get_file_size(node);
		out_buffer->assign					(file_size, m_allocator, "file_data");

		file_size_type const file_offs	=	get_file_offs(node);
		(* m_device)->seek					(file, file_offs, seek_file_begin);

		file_size_type const bytes_read	=	(* m_device)->read(file, out_buffer->c_ptr(), file_size);
		R_ASSERT_U							(bytes_read == file_size, "couldnot read %d bytes from file '%s'",
											 file_size, file_path.c_str());
	}
}

bool   compare_nodes::equal_file_nodes	(base_node<> * const left_node, base_node<> * const right_node)
{
	R_ASSERT								(!left_node->is_link() && !right_node->is_link());

	u32 const left_data_size			=	get_file_size(left_node);
	u32 const right_data_size			=	get_file_size(right_node);
	if ( left_data_size != right_data_size )
		return								false;

	allocated_buffer						left_data;			
	read_file_data							(& left_data, left_node);
	allocated_buffer						right_data;
	read_file_data							(& right_data, right_node);

	bool const files_equal				=	memory::compare(left_data, right_data) == 0;
	return									files_equal;
}

bool   compare_nodes::equal_folder_nodes 	(fat_node_info * const first_info, fat_node_info * const second_info)
{
	fat_node_info * it_child_of_first	=	first_info->first_child;
	fat_node_info * it_child_of_second	=	second_info->first_child;

	while ( it_child_of_first || it_child_of_second )
	{
		if ( !it_child_of_first || !it_child_of_second )
			return							false;

		if ( !equal_nodes_impl(it_child_of_first, it_child_of_second, true) )
			return							false;

		it_child_of_first				=	it_child_of_first->next;
		it_child_of_second				=	it_child_of_second->next;
	}

	return									true;
}

bool   compare_nodes::equal_nodes_impl	(fat_node_info * const first_info, fat_node_info * const second_info, bool check_name)
{
	fat_node_info * const real_first	=	strip_link(first_info);
	fat_node_info * const real_second	=	strip_link(second_info);

	base_node<> * const first_node		=	real_first->node;
	base_node<> * const second_node		=	real_second->node;

	if ( check_name )
	{
		pcstr const first_name			=	first_info->node->get_name();
		pcstr const second_name			=	second_info->node->get_name();
		if ( !strings::equal(first_name, second_name) )
			return							false;
	}

	if ( real_first->hash != real_second->hash )
		return								false;

	if ( first_node->is_erased() || second_node->is_erased() )
		return								first_node->is_erased() && second_node->is_erased();
	
	if ( first_node->is_folder() != second_node->is_folder() )
		return								false;

	if ( first_node->is_folder() )
		return								equal_folder_nodes		(real_first, real_second);

	return									equal_file_nodes		(first_node, second_node);
}

bool   compare_nodes::equal_nodes		(fat_node_info * const first_info, fat_node_info * const second_info)
{
	if ( first_info == second_info )
		return								true;
	if ( (first_info != NULL) != (second_info != NULL) )
		return								false;

	return									equal_nodes_impl(first_info, second_info, false);
}

bool   equal_nodes						(base_node<> * const 					first, 
										 base_node<> * const 					second,
										 synchronous_device_interface * const	device,
										 compressor * const						compressor,
										 memory::base_allocator * const			allocator)
{
	if ( (!!first) != (!!second) )
		return								false;
	if ( !first && !second )
		return								true;

	fat_node_info * const first_info	=	get_info_by_node(first);
	fat_node_info * const second_info	=	get_info_by_node(second);

	compare_nodes		compare				(device, compressor, allocator);
	return									compare.equal_nodes(first_info, second_info);
}

fat_node_info *   archive_saver::find_node_info_by_path (virtual_path_string const & node_path)
{
	base_node<> * const to_node			=	m_args.to_vfs->hashset.find_no_lock(node_path.c_str());

	fat_node_info * const out_info		=	get_info_by_node(to_node);

	virtual_path_string						path_to_check;
	out_info->node->get_full_path			(& path_to_check);
	R_ASSERT								(path_to_check == node_path);
	return									out_info;
}

void   set_cannot_be_refenced				(fat_node_info * const node_info)
{
	node_info->can_be_referenced		=	false;
	for ( fat_node_info *	it_child	=	node_info->first_child;
							it_child;
							it_child	=	it_child->next )
	{
		set_cannot_be_refenced				(it_child);
	}
}

bool   cause_cycle_linking				(virtual_path_string const & original_path, fat_node_info * const node_info)
{
	if ( node_info->link_target )
	{
		virtual_path_string					link_target_path;
		node_info->link_target->node->get_full_path	(& link_target_path);

		if ( path_starts_with(link_target_path.c_str(), original_path.c_str()) )
			return							true;
		
		return								cause_cycle_linking(original_path, node_info->link_target);
	}

	for ( fat_node_info *	it_child	=	node_info->first_child;
							it_child;
							it_child	=	it_child->next )
	{
		if ( cause_cycle_linking(original_path, it_child) )
			return							true;
	}

	return									false;
}

fat_node_info *   archive_saver::find_duplicate_folder_for_patch (base_node<> * node)
{
	virtual_path_string						node_path;
	node->get_full_path						(& node_path);
	fat_node_info * const original		=	find_node_info_by_path(node_path);

	base_node<> * const original_node	=	original->node;
	if ( original_node->is_folder() && !original_node->get_first_child() )
		return								NULL; // not making links between empty folders

	compare_nodes		compare				(m_env.device, m_args.compressor, m_args.allocator);

	for (	fat_node_info_set::iterator	it		=	m_env.node_info_set->find(original->hash),
										end_it	=	m_env.node_info_set->end();
										it		!=	end_it;
										++it )
	{
		fat_node_info * const it_info	=	* it;
	
		if ( !it_info->node->is_folder() )
			continue;

		if ( it_info == original )
			continue;

		if ( !it_info->can_be_referenced )
			continue;

		if ( !compare.equal_nodes(it_info, original) )
			continue;

		if ( cause_cycle_linking(node_path, it_info) )
			continue;

		virtual_path_string					duplicate_path;
		it_info->node->get_full_path		(& duplicate_path);

		base_node<> * const node_in_patch	=	m_hashset.find_no_lock(duplicate_path.c_str());
		if ( !node_in_patch )
		{
			set_cannot_be_refenced			(original);
			original->link_target		=	it_info;
			return							it_info;
		}

		fat_node_info * const patch_info	=	get_info_by_node(node_in_patch);

		if ( !patch_info->saved_node )
			return							NULL;

		set_cannot_be_refenced				(original);
		original->link_target			=	patch_info;
		return								patch_info;
	}

	return									NULL;
}

fat_node_info *   archive_saver::find_duplicate_folder (fat_node_info * node_info)
{
	base_node<> * const node			=	node_info->node;
	if ( node->is_folder() && !node->get_first_child() )
		return								NULL; // not making links between empty folders

	if ( saving_patch() )
		return								find_duplicate_folder_for_patch(node_info->node);	

	compare_nodes		compare				(m_env.device, m_args.compressor, m_args.allocator);

	for (	fat_node_info_set::iterator	it		=	m_env.node_info_set->find(node_info->hash),
										end_it	=	m_env.node_info_set->end();
										it		!=	end_it;
										++it )
	{
		fat_node_info * const it_info		=	* it;

		if ( !it_info->node->is_folder() )
			continue;

		if ( !compare.equal_nodes(it_info, node_info) )
			continue;

		node_info->link_target			=	it_info;
		return								it_info;
	}

	return									NULL;
}

fat_node_info *   archive_saver::find_duplicate_file (fat_node_info * node_info)
{
	synchronous_device_interface & device	=	m_args.device;

	base_node<> * const node			=	node_info->node;
	native_path_string const original_node_path	=	get_node_physical_path(node);
	file_size_type const original_file_offs		=	get_file_offs(node);

	file_type_pointer f_in					(original_node_path, device, file_mode::open_existing, file_access::read);
	R_ASSERT								(f_in);

	virtual_path_string						node_path;
	node->get_full_path						(& node_path);

	u32 const file_size					=	get_raw_file_size(node);

	fat_node_info *	duplicate_with_other_name	=	NULL;
	for ( fat_node_info_set::iterator	it		=	m_env.node_info_set->find(node_info->hash),
										end_it	=	m_env.node_info_set->end();
										it		!=	end_it;
										++it )
	{
		fat_node_info *	it_node			=	* it;

		bool const same_name			=	strings::equal(it_node->node->get_name(), node->get_name());
		if ( duplicate_with_other_name && !same_name )
			continue;

		if ( it_node->node->is_folder() )
			continue;

		if ( !it_node->can_be_referenced )
			continue;

		if ( saving_patch() )
		{
			virtual_path_string				it_node_path;
			it_node->node->get_full_path	(&it_node_path);
			if ( it_node_path == node_path )
				continue;

			base_node<> * const node_in_patch	=	m_hashset.find_no_lock(it_node_path.c_str());
			if ( node_in_patch )
			{
				it_node					=	get_info_by_node(node_in_patch);
				if ( !it_node->saved_node )
					continue;
			}
		}
		else if ( !it_node->saved_node )
			continue;

		if ( get_raw_file_size(it_node->node) != file_size )
			continue;

		native_path_string	path		=	get_node_physical_path(it_node->node);

		file_type_pointer	f_cur			(path, device, file_mode::open_existing, file_access::read);
		R_ASSERT_U							(f_cur);
		
		file_size_type const cur_offs	=	get_file_offs(it_node->node);
		device->seek						(f_cur, cur_offs, seek_file_begin);
		device->seek						(f_in, original_file_offs, seek_file_begin);
		bool const equal_files			=	files_equal(device, f_cur, f_in, file_size);

		if ( !equal_files )
			continue;

		if ( same_name )
		{
			node_info->can_be_referenced	=	false;
			node_info->link_target		=	it_node;
			return							it_node;
		}

		R_ASSERT							(!duplicate_with_other_name);
		duplicate_with_other_name		=	it_node;
	}

	node_info->link_target				=	duplicate_with_other_name;
	return									duplicate_with_other_name;
}

} // namespace vfs
} // namespace xray

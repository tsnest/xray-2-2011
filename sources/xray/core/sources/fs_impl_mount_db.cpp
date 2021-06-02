////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include <xray/fs_utils.h>
#include "fs_helper.h"
#include <xray/resources_unmanaged_allocator.h>

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// mounting db
//-----------------------------------------------------------------------------------

bool    file_system_impl::mount_db (	pcstr	logical_path,
							  			pcstr	fat_path, 
							  			pcstr	db_path,
							  			bool	need_replication,
										bool	store_in_mount_history,
										bool	is_automatically_mounted	)
{
	R_ASSERT								(is_automatically_mounted != store_in_mount_history);

	threading::mutex_raii		raii		(m_mount_mutex);
	if ( store_in_mount_history )
		on_mount							(mount_history::mount_type_db, logical_path, fat_path, file_system::watch_directory_false);

	if ( need_replication )
	{
		ASSERT								( m_replication_dir.length() );

		fs::path_string						replicated_fat, replicated_db;
		replicate_path						(fat_path, replicated_fat);

		bool db_replicated				=	false;

		if ( file_exists(replicated_fat.c_str()) )
		{
			db_replicated				=	true;
			replicate_path					(db_path, replicated_db);
		}

		if ( db_replicated )
		{
			return mount_db_impl			(logical_path, fat_path, db_path, 
											 replicated_fat.c_str(), replicated_db.c_str(),
											 true, false, is_automatically_mounted);
		}
	}

	return mount_db_impl					(logical_path, fat_path, db_path, NULL, NULL, 
											 false, need_replication, is_automatically_mounted);
}

bool   file_system_impl::mount_db_impl (	pcstr	logical_path,
											pcstr	fat_path,
											pcstr	db_path,
											pcstr	replicated_fat_path,
											pcstr	replicated_db_path,
											bool	open_replicated,
											bool	need_replication,
											bool	is_automatically_mounted	)
{
	XRAY_UNREFERENCED_PARAMETER						(replicated_db_path);
	verify_path_is_portable							(logical_path);
	verify_path_is_portable							(fat_path);
	verify_path_is_portable							(db_path);

	find_by_db_path	find_predicate			(fat_path, logical_path);
	sub_fat_resource_ptr db				=	m_root_dbs.find_if(find_predicate);
	
	if ( db )
	{	
		//path_string	const db_file_path	=	db->archive_physical_path;

		//unmount_db						(logical_path, fat_path);
		//mount_db						(logical_path, fat_path, db_file_path.c_str(), 
		//								need_replication);
		return							true;
	}

	file_type * fat_file			=	NULL;
	if ( !open_file(&fat_file, open_file_read, (open_replicated ? replicated_fat_path : fat_path), false) )
		return							false;

	fat_header							header;
	read_file							(fat_file, & header, sizeof(header));
	bool const reverse_byte_order	=	header.is_big_endian() != platform::big_endian();
	if ( reverse_byte_order )
		header.reverse_bytes			();

	u32 raw_buffer_size				=	header.buffer_size + sizeof(header);
	u32 const full_db_size			=	sizeof(sub_fat_resource) + raw_buffer_size + 1;
	sub_fat_resource * new_db_raw	=	(sub_fat_resource *)UNMANAGED_ALLOC(char, full_db_size);
	R_ASSERT							(new_db_raw);
	new (new_db_raw)					sub_fat_resource(header, raw_buffer_size);
	
	sub_fat_resource_ptr new_db		=	new_db_raw;
	new_db->archive_physical_path	=	db_path;
	new_db->fat_physical_path		=	fat_path;
	new_db->virtual_path			=	logical_path;
	new_db->is_automatically_mounted	=	is_automatically_mounted;
	new_db->set_no_delete				();

	read_file							(fat_file, new_db->nodes_buffer(), new_db->nodes_buffer_size());

	if ( need_replication )
	{
		fs::path_string					replicated_path;
		replicate_path					(fat_path, replicated_path);
		make_dir_r						(replicated_path.c_str(), false);

		file_size_type					fat_file_length;
		if ( !calculate_file_size(&fat_file_length,fat_file) ) 
		{
			LOGI_INFO					("fs", "failed to mount fat \"%s\" on \"%s\"", 
										 open_replicated ? replicated_fat_path : fat_path, 
										 logical_path);
			close_file					(fat_file);
			return						false;
		}
		
		bool const repl_res			=	do_replication(replicated_path.c_str(), 
													   (pcbyte)new_db->raw_buffer, 
													   new_db->raw_buffer_size, 
													   0, 
													   fat_file_length);
		XRAY_UNREFERENCED_PARAMETER		(repl_res);
		R_ASSERT						(repl_res);

		LOGI_INFO						("fs", "replicated fat \"%s\" to \"%s\"", 
										 fat_path, 
										 replicated_path.c_str());
	}
	
	mount_sub_fat_resource_impl			(new_db.c_ptr());
	m_root_dbs.push_back				(new_db);

	close_file							(fat_file);

	LOGI_INFO							("fs", "mounted fat \"%s\" on \"%s\"", 
										 open_replicated ? replicated_fat_path : fat_path, 
										 logical_path);

	return								true;
}

sub_fat_resource *   file_system_impl::find_sub_fat_parent (sub_fat_resource * sub_fat)
{
	sub_fat_resource * closest_parent	=	NULL;
	u32 closest_level					=	0;

	u32 const sub_fat_level				=	sub_fat->virtual_path.count_of('/');

	for ( sub_fat_resource_tree_type::iterator	it		=	m_dbs_tree.begin(),
												end_it	=	m_dbs_tree.end();
												it		!=	end_it;
												++it )
	{
		sub_fat_resource * it_sub_fat	=	& * it;
		if ( sub_fat->archive_physical_path != it_sub_fat->archive_physical_path )
			continue;

		u32 const level					=	it_sub_fat->virtual_path.count_of('/');
		if ( level >= sub_fat_level )
			continue;
	
		if ( !closest_parent || closest_level < level )
		{
			closest_parent				=	it_sub_fat;
			closest_level				=	level;
		}	
	}

	return								closest_parent;
}

void   file_system_impl::mount_sub_fat_resource_impl (sub_fat_resource * new_db)
{
	bool const reverse_byte_order	=	(new_db->header->is_big_endian() != platform::big_endian());

	new_db->root_node				=	&reinterpret_cast<fat_folder_node<> *>
										(new_db->nodes_buffer())->m_base;
	
	u32	hash						=	path_crc32(new_db->virtual_path.c_str(), 
												   new_db->virtual_path.length());
	fixup_db_node						(new_db->nodes_buffer(), 
										 new_db->root_node, 
										 hash, 
										 reverse_byte_order);

	actualize_db						(new_db);

	sub_fat_resource * const parent_fat	=	find_sub_fat_parent(new_db);
	if ( parent_fat )
		parent_fat->add_child			(new_db);

	m_dbs_tree.insert					(* new_db);
	//LOGI_INFO							("fstemp", "m_dbs_tree.insert %s %s", new_db->logical_path, new_db->root_node->get_name());
}

void   file_system_impl::fixup_db_node	(char *			flat_buffer, 
										 fat_node<> *	work_node, 
										 u32			hash, 
										 bool			reverse_byte_order)
{
	u32	const my_hash			=	work_node->m_parent ? 
									crc32(work_node->m_name, strings::length(work_node->m_name), hash) : hash;

	if ( *work_node->m_name ) // && work_node->m_parent )
		m_hash_set.insert			(my_hash, work_node);
	
	bool work_node_is_folder	=	false;

	u8 const num_bytes_in_flags	=	sizeof(work_node->m_flags);

#pragma warning( push)
#pragma warning( disable : 6326 )
 	if ( !reverse_byte_order || identity(num_bytes_in_flags == 1) )
#pragma warning( pop )
 	{
 		work_node_is_folder		=	work_node->is_folder();
 	}
	else
	{
		ASSERT						(num_bytes_in_flags == 2, "more then 16 flags? add case here then");
		u16	flags				=	work_node->m_flags;
		if ( reverse_byte_order )
			reverse_bytes			(flags);
		work_node_is_folder		=	(flags & file_system::is_folder) != 0;
	}

	if ( work_node_is_folder && !work_node->is_link() )
	{
		fat_folder_node<> * cur_folder_node	=	work_node->cast_folder();

		if ( reverse_byte_order )
			cur_folder_node->reverse_bytes		();

		size_t const	 child_offs			=	reinterpret_cast<size_t>((fat_node<> *)cur_folder_node->m_first_child);
		cur_folder_node->m_first_child		=	child_offs ? reinterpret_cast<fat_node<> *>(flat_buffer + child_offs) : NULL;
	}
	else
	{
		if ( reverse_byte_order )
			work_node->reverse_bytes_for_real_class();

		if ( work_node->is_soft_link() )
		{
			fat_db_soft_link_node<> * soft_link_node	=	work_node->cast_db_soft_link_node();
			size_t const relative_path_to_referenced_offs	=	(size_t)((pvoid)soft_link_node->m_relative_path_to_referenced);
			R_ASSERT							(relative_path_to_referenced_offs);
			soft_link_node->m_relative_path_to_referenced	=	(pstr)(flat_buffer + relative_path_to_referenced_offs);
		}
		else if ( work_node->is_hard_link() )
		{
			fat_db_hard_link_node<> * hard_link_node	=	work_node->cast_db_hard_link_node();
			size_t const reference_offs	=	(size_t)((fat_node<> *)hard_link_node->m_reference_node);
			hard_link_node->m_reference_node	=	reference_offs ? (fat_node<> *)(flat_buffer + reference_offs) : NULL;
		}
		else if ( work_node->is_inlined() )
		{
			const_buffer inline_data;
			if ( work_node->get_inline_data(& inline_data) )
			{
				size_t const data_offs	=	(size_t)inline_data.c_ptr();
				R_ASSERT					(data_offs);
				work_node->set_inline_data	(memory::buffer(flat_buffer + data_offs, inline_data.size()));
			}
			else
				UNREACHABLE_CODE();
		}
	}

	size_t const parent_offs	=	(size_t)((fat_node<> *)work_node->m_parent);
	work_node->m_parent			=	parent_offs ? (fat_node<> *)(flat_buffer + parent_offs) : NULL;

	size_t const next_offs		=	(size_t)((fat_node<> *)work_node->m_next);
	work_node->m_next			=	next_offs ? (fat_node<> *)(flat_buffer + next_offs) : NULL;
	size_t const prev_offs		=	(size_t)((fat_node<> *)work_node->m_prev);
	work_node->m_prev			=	prev_offs ? (fat_node<> *)(flat_buffer + prev_offs) : NULL;
	
	fat_node<> *	child		=	work_node->is_link() ? NULL : work_node->get_first_child();
	while ( child )
	{
		fixup_db_node				(flat_buffer, child, my_hash, reverse_byte_order);
		child					=	child->get_next();
	}
}

void   file_system_impl::relink_hidden_nodes_to_parent (fat_node<> const * const work_node, u32 const hash, fat_node<> * const parent)
{
	fat_node<> * fixing_node	=	* m_hash_set.find(hash);

	for ( ; fixing_node; fixing_node = fixing_node->get_next_overlapped() )
	{
		if ( is_erased_node(fixing_node) )
			continue;

		if ( !fixing_node->same_path(work_node) )
			continue;

		fat_folder_node<> const * const fixing_node_parent	=	fixing_node->m_parent->cast_folder();

		if ( !fixing_node_parent->find_child(fixing_node) )
			fixing_node->m_parent	=	parent;
	}
}

void   deassociate_from_grm_and_fat_if_needed (fat_node<> * node);

void   file_system_impl::actualize_node (fat_node<> * work_node, u32 const hash, fat_folder_node<> * parent)
{
	fat_node<> * prev_node					=	parent->find_child(work_node->m_name);

	if ( prev_node == work_node )
		return;
	
	if ( prev_node )
	{
		if ( !prev_node->is_link() && !work_node->is_link() && 
			  prev_node->is_folder() && work_node->is_folder() )
		{
			fat_folder_node<> * work_folder	=	work_node->cast_folder();
			fat_folder_node<> * prev_folder	=	prev_node->cast_folder();
			// transfer work_node children to prev_node
			fat_node<> *	cur_child		=	work_node->get_first_child();
			while ( cur_child )
			{
				fat_node<> * next_child		=	cur_child->get_next();
				work_folder->unlink_child		(cur_child, true);
				cur_child->m_parent			=	work_node;

				u32 const child_hash		=	crc32(cur_child->m_name, strings::length(cur_child->m_name), hash);
				actualize_node					(cur_child, child_hash, prev_folder);
				cur_child					=	next_child;
			}

			// transfer prev_node children to work_node
			cur_child						=	prev_node->get_first_child();
			while ( cur_child )
			{
				fat_node<> * next_child		=	cur_child->get_next();
				prev_folder->unlink_child		(cur_child, true);
				work_folder->prepend_child		(cur_child);

				u32 const child_hash		=	crc32(cur_child->m_name, strings::length(cur_child->m_name), hash);
				relink_hidden_nodes_to_parent	(cur_child, child_hash, work_node);

				cur_child					=	next_child;
			}
		}

		deassociate_from_grm_and_fat_if_needed	(prev_node);
		parent->unlink_child					(prev_node, false);
		//prev_node->m_parent					=	(parent == m_root) ? m_root->cast_node() : work_node->m_parent;
	}	

	//LOGI_INFO									("fstemp", "adding node: %s (flags: %d)", work_node->get_name(), work_node->get_flags());

	parent->prepend_child						(work_node);
}

void   file_system_impl::actualize_db (sub_fat_resource * db)
{
	u32				 						 	hash;

	if ( strings::length(db->root_node->get_name()) != 0 )
	{
		path_string								mount_root_path;
		directory_part_from_path				(& mount_root_path, db->virtual_path.c_str());
		fat_folder_node<> * const mount_root	=	find_or_create_folder(mount_root_path.c_str(), & hash);
		actualize_node							(db->root_node, hash, mount_root);
		return;
	}

	fat_folder_node<> * const mount_root	=	find_or_create_folder(db->virtual_path.c_str(), & hash);

	fat_node<> *	cur_child				=	db->root_node->get_first_child();
	while ( cur_child )
	{
		fat_node<> * next_child				=	cur_child->get_next();
		u32	child_hash						=	crc32(cur_child->m_name, strings::length(cur_child->m_name), hash);
		actualize_node							(cur_child, child_hash, mount_root);
		cur_child							=	next_child;
	}
}

} // namespace fs 
} // namespace xray 

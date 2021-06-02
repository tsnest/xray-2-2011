////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include <xray/fs_utils.h>
#include "fs_helper.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// mounting db
//-----------------------------------------------------------------------------------

bool    file_system_impl::mount_db (	pcstr	logical_path,
							  			pcstr	fat_path, 
							  			pcstr	db_path,
							  			bool	need_replication)
{
	threading::mutex_raii		raii		(m_mount_mutex);
	on_mount								(mount_history::mount_type_db, logical_path, fat_path, file_system::watch_directory_false);

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
											 true, false);
		}
	}

	return mount_db_impl					(logical_path, fat_path, db_path, 
											 NULL, NULL, 
											 false, need_replication);
}

bool   file_system_impl::mount_db_impl (	pcstr	logical_path,
											pcstr	fat_path,
											pcstr	db_path,
											pcstr	replicated_fat_path,
											pcstr	replicated_db_path,
											bool	open_replicated,
											bool	need_replication)
{
	XRAY_UNREFERENCED_PARAMETER						(replicated_db_path);
	verify_path_is_portable							(logical_path);
	verify_path_is_portable							(fat_path);
	verify_path_is_portable							(db_path);

	db_data::iterator db_it
		=	std::find_if(m_db_data.begin(), m_db_data.end(), find_db_by_path(logical_path, fat_path));
	
	if ( db_it != m_db_data.end() )
	{	
		db_record const&  db		=	*db_it;
		path_string	const db_file_path	
									=	db.db_path;

		unmount_db						(logical_path, fat_path);
		mount_db						(logical_path, fat_path, db_file_path.c_str(), 
										need_replication);
		return							true;
	}

	db_record							new_db;
	new_db.fat_path					=	fat_path;
	new_db.db_path					=	db_path;
	new_db.logical_path				=	logical_path;

	FILE * fat_file					=	NULL;
	if ( !open_file(&fat_file, open_file_read, (open_replicated ? replicated_fat_path : fat_path), false) )
		return							false;

	db_header							header;
	fread								(& header, sizeof(header), 1, fat_file);
	bool const reverse_byte_order	=	header.is_big_endian() != platform::big_endian();

	if ( reverse_byte_order )
		header.reverse_bytes			();

	file_size_type const buffer_size	=	header.buffer_size;

	// new_db.flat_buffer will contain whole file_system
	new_db.flat_buffer_size			=	buffer_size + sizeof(header);

	// we pad allocation with 1 byte for the very special use with interlocked operations that might require this byte to be allocated
	new_db.flat_buffer				=	FS_ALLOC(char, (size_t)(new_db.flat_buffer_size + 1));

	* (db_header *)new_db.flat_buffer	=	header;
	fread								(new_db.flat_buffer + sizeof(header), (size_t)buffer_size, 1, fat_file);

	if ( need_replication )
	{
		fs::path_string					replicated_path;
		replicate_path					(fat_path, replicated_path);
		make_dir_r						(replicated_path.c_str(), false);

		file_size_type fat_file_length;
		if ( !calculate_file_size(&fat_file_length,fat_file) ) {
			LOGI_INFO					("file_system", "failed to mount fat \"%s\" on \"%s\"", 
										 open_replicated ? replicated_fat_path : fat_path, 
										 logical_path);
			fclose						( fat_file );
			return						false;
		}
		
		const bool repl_res			=	do_replication(replicated_path.c_str(), 
													   (pcbyte)new_db.flat_buffer, 
													   new_db.flat_buffer_size, 
													   0, 
													   fat_file_length);
		XRAY_UNREFERENCED_PARAMETER		( repl_res );
		R_ASSERT						( repl_res );

		LOGI_INFO						("file_system", "replicated fat \"%s\" to \"%s\"", 
										 fat_path, 
										 replicated_path.c_str());
	}

	new_db.root_node				=	&reinterpret_cast<fat_folder_node<> *>
										(new_db.flat_buffer+sizeof(header))->m_base;
	
	u32	hash						=	crc32(logical_path, strings::length(logical_path));
	fixup_db_node						(new_db.flat_buffer+sizeof(header), new_db.root_node, 
										hash, reverse_byte_order);

	db_it							=	std::find_if(	m_db_data.begin(), 
														m_db_data.end(), 
														find_db_by_ptr(new_db.root_node) );

	m_db_data.insert					(db_it, new_db);

	actualize_db						(new_db);
	fclose								(fat_file);

	LOGI_INFO							("file_system", "mounted fat \"%s\" on \"%s\"", 
										 open_replicated ? replicated_fat_path : fat_path, 
										 logical_path);

	return								true;
}

void   file_system_impl::fixup_db_node	(char *			flat_buffer, 
										 fat_node<> *	work_node, 
										 u32			hash, 
										 bool			reverse_byte_order)
{
	u32	const my_hash			=	crc32(work_node->m_name, strings::length(work_node->m_name), hash);

	if ( *work_node->m_name && work_node->m_parent )
		m_hash_set.insert			(my_hash, work_node);
	
	bool work_node_is_folder	=	false;

	u8 const num_bytes_in_flags	=	sizeof(work_node->m_flags);
	
	if ( !reverse_byte_order || identity(num_bytes_in_flags == 1) )
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

	if ( work_node_is_folder )
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

		if ( work_node->is_inlined() )
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
	
	fat_node<> *	child		=	work_node->get_first_child();
	while ( child )
	{
		fixup_db_node				(flat_buffer, child, my_hash, reverse_byte_order);
		child					=	child->get_next();
	}
}

void   file_system_impl::relink_hidden_nodes_to_parent (fat_node<> const * const work_node, u32 const hash, fat_node<> * const parent)
{
	fat_node<> * fixing_node	=	* m_hash_set.find(hash);

	while ( fixing_node )
	{
		if ( fixing_node->same_path	(work_node) )
		{
			fat_folder_node<> const * const fixing_node_parent	=	fixing_node->m_parent->cast_folder();

			if ( !fixing_node_parent->find_child(fixing_node) )
			{
				fixing_node->m_parent	=	parent;
			}
		}
		
		fixing_node	=	fixing_node->get_next_hash_node();
	}
}

void   file_system_impl::actualize_node (fat_node<> * work_node, u32 const hash, fat_folder_node<> * parent)
{
	fat_node<> * prev_node					=	parent->find_child(work_node->m_name);

	if ( prev_node == work_node )
		return;
	
	if ( prev_node )
	{
		if ( prev_node->is_folder() && work_node->is_folder() )
		{
			fat_folder_node<> * work_folder	=	work_node->cast_folder();
			fat_folder_node<> * prev_folder	=	prev_node->cast_folder();
			// transfer work_node children to prev_node
			fat_node<> *	cur_child		=	work_node->get_first_child();
			while ( cur_child )
			{
				fat_node<> * next_child		=	cur_child->get_next();
				work_folder->unlink_child		(cur_child);
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
				prev_folder->unlink_child		(cur_child);
				work_folder->prepend_child		(cur_child);

				u32 const child_hash		=	crc32(cur_child->m_name, strings::length(cur_child->m_name), hash);
				relink_hidden_nodes_to_parent	(cur_child, child_hash, work_node);

				cur_child					=	next_child;
			}
		}

		parent->unlink_child					(prev_node);
		prev_node->m_parent					=	(parent == m_root) ? m_root->cast_node() : work_node->m_parent;
	}

	parent->prepend_child						(work_node);
}

void   file_system_impl::actualize_db (db_record & db)
{
	u32				 						 	hash;
	fat_folder_node<> * 						mount_root
											=	find_or_create_folder(db.logical_path.c_str(), & hash);

	fat_node<> *	cur_child				=	db.root_node->get_first_child();
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

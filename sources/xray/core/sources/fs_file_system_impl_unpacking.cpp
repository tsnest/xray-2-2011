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
// unpacking
//-----------------------------------------------------------------------------------

bool   file_system_impl::unpack (pcstr logical_path, pcstr dest_dir_path, db_callback callback)
{
	threading::mutex_raii		raii		(m_mount_mutex);
	verify_path_is_portable					(logical_path);
	verify_path_is_portable					(dest_dir_path);

	u32		hash;
	fat_node<> *	unpack_root		=	find_node(logical_path, & hash);

	if ( !unpack_root )
		return false;

	if ( !make_dir(dest_dir_path) )
		return					false;

	path_string cur_path	=	dest_dir_path;
	
	save_nodes_environment		env;

	env.callback			=	callback;
	env.node_index			=	0;
	env.num_nodes			=	unpack_root->get_num_nodes();

	return unpack_node			(env, unpack_root, cur_path);
}

bool   file_system_impl::equal_db (fat_node<> const * node1, fat_node<> const * node2) const
{
	ASSERT						(node1->is_db() && node2->is_db());
	return						get_db(node1) == get_db(node2);
}

bool   file_system_impl::unpack_node (save_nodes_environment & env, fat_node<> * cur_node, path_string & dest_dir)
{
	u32 const prev_length	=	dest_dir.length();
	dest_dir				+=	"/"; 
	dest_dir				+=	cur_node->m_name;
	const bool is_root		=	(cur_node == m_root->cast_node());

	if ( !is_root && cur_node->is_folder() )
	{
		if ( !make_dir			(dest_dir.c_str()) )
		{
			return				false;
		}
	}
	else if ( !is_root )
	{
		FILE* f_out			=	NULL;
		if ( !open_file(&f_out, open_file_create | open_file_truncate | open_file_write, dest_dir.c_str()) )
			return				false;

		path_string				path;
		get_disk_path			(cur_node, path);
		FILE* f_in			=	NULL;
		if ( !open_file(&f_in, open_file_read, path.c_str()) )
		{
			fclose				(f_out);
			return				false;
		}

		fseek64					(f_in, cur_node->get_file_offs(), SEEK_SET);
		copy_data				(f_out, f_in, cur_node->get_raw_file_size());
		fclose					(f_in);		

		fclose					(f_out);
	}

	fat_node<> *	child	=	cur_node->get_first_child();
	while ( child )
	{
		unpack_node				(env, child, dest_dir);
		child				=	child->get_next();
	}

	dest_dir.set_length			(prev_length);

	++env.node_index;
	if ( env.callback )
		env.callback			(env.node_index, env.num_nodes, cur_node->get_name(), cur_node->get_flags());

	return						true;
}

} // namespace fs 
} // namespace xray 

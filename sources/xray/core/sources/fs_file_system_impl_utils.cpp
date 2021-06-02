////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// file_system_impl implementation
//-----------------------------------------------------------------------------------

bool   file_system_impl::db_record::contains (fat_node<> const * work_node) const
{
	return ((pcstr)work_node > flat_buffer) && 
		   ((pcstr)work_node < flat_buffer+flat_buffer_size);
}

void   file_system_impl::db_record::destroy ()
{
	FS_FREE (flat_buffer);
}

file_system_impl::file_system_impl () : m_root(NULL), m_on_resource_leaked_callback(NULL)
{	
	create_root();
}

void   file_system_impl::clear ()
{
	m_hash_set.clear				();
	free_node						(m_root->cast_node(), true);

	for ( db_data::iterator	it	=	m_db_data.begin();
							it	!=	m_db_data.end();
						  ++it	)
	{
		db_record&	record		=	*it;
		record.destroy();
	}

	m_root						=	NULL;
	create_root						();
}

file_system_impl::~file_system_impl ()
{
 	clear				();
 	m_hash_set.clear	();
 	free_node			(m_root->cast_node(), true);

	history_deleter_predicate	history_deleter_predicate;
	m_mount_history.for_each	(history_deleter_predicate);
	m_mount_history.pop_all_and_clear	();
}

file_system_impl::db_record *   file_system_impl::get_db (fat_node<> const * work_node) 
{
	db_data::iterator it	=	std::find_if(m_db_data.begin(), m_db_data.end(), find_node_in_db(work_node));

	if ( it != m_db_data.end() )
	{
		return &*it;
	}

	return NULL;
}

void   file_system_impl::get_disk_path (fat_node<> const * work_node, path_string & out_path, const bool replicate) const
{
	out_path.clear				();
	pcstr						file_path;

	fat_disk_file_node<> const * disk_file_node	=	NULL;
	fat_disk_folder_node<> const * disk_folder_node	=	NULL;
	if ( work_node->is_disk() )
	{
		if ( work_node->is_folder() )
		{
			disk_folder_node	=	work_node->cast_disk_folder();
			file_path			=	disk_folder_node->m_disk_path;
		}
		else
		{
			disk_file_node	=	work_node->cast_disk_file();
			file_path		=	disk_file_node->m_disk_path;
		}
	}
	else
	{
		const file_system_impl::db_record* db 
							=	get_db(work_node);
		ASSERT					(db);
		file_path			=	db->db_path.c_str();
	}

	if ( work_node->is_replicated() && replicate )
		replicate_path			(file_path, out_path);
	else
		out_path			=	file_path;
}

bool   file_system_impl::operator == (file_system_impl const & f) const
{
	return						* m_root->cast_node() == * f.m_root->cast_node();
}

bool   file_system_impl::is_main_fat () const
{
	return						g_fat->get_impl() == this;
}

void   file_system_impl::erase_disk_node (pcstr physical_path)
{
	path_string					logical_path;
	if ( !convert_physical_to_logical_path(& logical_path, physical_path, false) )
		return;

	unmount_disk				(logical_path.c_str(), physical_path, respect_mount_history_false);
}

void   file_system_impl::db_header::reverse_bytes ()
{ 
	xray::fs::reverse_bytes		(num_nodes); 
	xray::fs::reverse_bytes		(buffer_size); 
}

static const pcstr	big_endian_string		=	"big-endian";
static const pcstr	little_endian_string	=	"little-endian";

void   file_system_impl::db_header::set_big_endian () 
{ 
	strings::copy				(endian_string, big_endian_string); 
}

void   file_system_impl::db_header::set_little_endian () 
{ 
	strings::copy				(endian_string, little_endian_string);
}

bool   file_system_impl::db_header::is_big_endian () 
{ 
	R_ASSERT					(endian_string[0]); 
	return						strings::equal(endian_string, big_endian_string);
}

} // namespace fs 
} // namespace xray 

////////////////////////////////////////////////////////////////////////////
//	Created		: 21.12.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system_impl.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>

namespace xray {
namespace fs   {

//-----------------------------------------------------------------------------------
// replicating
//-----------------------------------------------------------------------------------

void   file_system_impl::replicate_path (pcstr path2replicate, path_string& out_path) const
{
	out_path				=	m_replication_dir;
	out_path				+=	"/";

	pcstr colon_pos			=	strchr(path2replicate, ':');
	if ( colon_pos )		
		out_path			+=	colon_pos + 2;
	else
		out_path			+=	path2replicate;

	verify_path_is_portable		(out_path.c_str());
}

void   file_system_impl::set_replication_dir (pcstr replication_dir)
{
	m_replication_dir	=	replication_dir;
}

bool   file_system_impl::replicate_file (fat_node<> * work_node, pcbyte data)
{
	ASSERT						( !work_node->is_replicated() );

	path_string					file_path, replicated_path;
	get_disk_path				(work_node, file_path);
	replicate_path				(file_path.c_str(), replicated_path);

	make_dir_r					(replicated_path.c_str(), false);

	bool const result		=	do_replication(	replicated_path.c_str(), data, 
												work_node->get_raw_file_size(), 
												work_node->get_file_offs(), 0 );
	if ( !result )
		return					false;

	commit_replication			(work_node);
	return						true;
}

bool   file_system_impl::do_replication  (pcstr							path, 
										  pcbyte						data, 
										  file_size_type const			size, 
										  file_size_type const			offs, 
										  file_size_type const			file_size)
{
	FILE *						f;
	if ( !open_file(&f, open_file_create | open_file_write, path) )
		return					false;

	fseek64						(f, offs, SEEK_SET);
	size_t written				= fwrite(data, 1, (size_t)size, f);
	XRAY_UNREFERENCED_PARAMETER	( written );
	R_ASSERT					(written == size);

	file_size_type				old_length;
	fs::calculate_file_size		(& old_length, f);

	if ( file_size > old_length )
	{
		fseek64					(f, file_size-1, SEEK_SET);
		fputc					(1, f);
	}

	fclose						(f);
	return						true;

}

void   file_system_impl::commit_replication (fat_node<> * work_node)
{
	ASSERT							(!work_node->is_replicated());

	db_record * db				=	work_node->is_db() ? get_db(work_node) : NULL;

	COMPILE_ASSERT					(sizeof(long) == sizeof(u32),
									 use_32_bit_version_of_interlocked_or_on_this_platform);
	if ( db )
	{
		FILE*	f_out			=	NULL;

		path_string					replicated_fat_path;
		replicate_path				(db->fat_path.c_str(), replicated_fat_path);

		bool const result		=	open_file(&f_out, open_file_write, replicated_fat_path.c_str());
		R_ASSERT					(result);
		if ( !result )
			return;

		// 
		threading::interlocked_or	((threading::atomic32_type&)work_node->ref_flags_as_32bit(), file_system::is_replicated << detail::bits_to_flags_byte());

		u32 const flags_offs	=	(u32)((char *)&work_node->m_flags - (char *)db->flat_buffer);
		fseek						(f_out, flags_offs, SEEK_SET);
		fwrite						(& work_node->m_flags, 1, sizeof(work_node->m_flags), f_out);
		fclose						(f_out);
	}
	else
	{
		threading::interlocked_or	((threading::atomic32_type&)work_node->ref_flags_as_32bit(), file_system::is_replicated << detail::bits_to_flags_byte());
	}
}



} // namespace fs 
} // namespace xray 

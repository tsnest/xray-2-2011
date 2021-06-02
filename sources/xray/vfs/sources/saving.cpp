////////////////////////////////////////////////////////////////////////////
//	Created		: 26.04.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "saving.h"
#include <xray/compressor.h>
#include <xray/fs/file_type.h>

namespace xray {
namespace vfs {

using namespace		fs_new;

bool	create_archive_files			(native_path_string const & 		archive_path, 
										 native_path_string const & 		fat_path,
										 synchronous_device_interface	&	device,
										 file_type * &						out_db_file,
										 file_type * &						out_fat_file)
{
	if ( archive_path.length() )
	{
		if ( !device->open(& out_db_file, archive_path, fs_new::file_mode::create_always, fs_new::file_access::write) )
			return							false;
	}

	bool fat_in_db						=	!fat_path.length() || 
											(archive_path.length() && archive_path == fat_path);
		
	if ( fat_in_db )
	{
		out_fat_file					=	 out_db_file;
	}	
	else
	{
		if ( !device->open(& out_fat_file, fat_path, fs_new::file_mode::create_always, fs_new::file_access::write) )
		{
			if ( out_db_file )
				device->close				(out_db_file);
			return							false;
		}
	}

	return									true;
}

void	close_archive_files				(synchronous_device_interface	&	device,
										 file_type * &						db_file,
										 file_type * &						fat_file)
{
	device->close							(fat_file);
	if ( db_file && db_file != fat_file )
		device->close						(db_file);
}

archive_saver::archive_saver			(save_archive_args & args, vfs_hashset & hashset)
	: m_args(args), m_env(args.device), m_hashset(hashset), m_finished_successfully(false), 
	m_mount_root_offs(0), m_do_reverse_bytes(false), m_root_save_db(true)
{
	CURE_ASSERT								(args.fat_alignment != 0, return);

	if ( !create_archive_files(m_args.archive_path, m_args.fat_path, m_args.device, m_env.db_file, m_env.fat_file) )
		return;

	if ( !m_env.fat_file )
		return;

	m_finished_successfully				=	save_db();

	close_archive_files						(m_args.device, m_env.db_file, m_env.fat_file);
}

archive_saver::archive_saver			(save_archive_args &		args, 
										 vfs_hashset &				hashset,
										 saving_environment &		env)
	: m_args(args), m_env(env), m_hashset(hashset), m_finished_successfully(false), 
	m_mount_root_offs(0), m_do_reverse_bytes(false)
{
	m_finished_successfully				=	save_db();	
}

} // namespace vfs
} // namespace xray
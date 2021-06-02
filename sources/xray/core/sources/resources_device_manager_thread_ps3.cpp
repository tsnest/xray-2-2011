////////////////////////////////////////////////////////////////////////////
//	Created		: 07.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_device_manager.h"
#include <cell/cell_fs.h>

namespace xray {
namespace resources {

using namespace	fs;

// out: false if cannot open file, ready and fail flags are set
bool   device_manager::open_file ()
{
	using namespace							fs;

	ASSERT									(m_query);
	ASSERT									(!m_file_handle);

	if ( !m_query->is_save_type() )
	{
		fs_iterator it					=	m_query->get_fat_it();
		if ( it.is_end() || it.is_folder() )
		{
			m_query->set_error_type			(query_result::error_type_file_not_found);
			return							false;
		}
	}

	path_string	const native_path		=	m_query->absolute_physical_path();

	COMPILE_ASSERT							( sizeof(m_file_handle) >= sizeof(int), file_handle_type_is_less_than_int );

	CellFsErrno	error;
	if ( m_query->is_load_type() )
	{
		error							=	cellFsOpen (native_path.c_str(),
														CELL_FS_O_RDONLY,
														pointer_cast<int*>( &m_file_handle ),
														NULL,
 														0);
	}
	else
	{
		make_dir_r							(native_path.c_str(), false);
		error							=	cellFsOpen (native_path.c_str(),
														CELL_FS_O_RDWR | CELL_FS_O_CREAT | CELL_FS_O_TRUNC,
														pointer_cast<int*>( &m_file_handle ),
														NULL,
 														0);
	}

	if ( error != CELL_FS_SUCCEEDED ) {
		switch ( error ) {
			case CELL_FS_ENOTMOUNTED: {
				LOG_ERROR					( "File system corresponding to pathis not mounted" );
				break;
			}
			case CELL_FS_ENOENT: {
				LOG_ERROR					( "The file specified by path does not exist" );
				break;
			}
			case CELL_FS_EINVAL: {
				LOG_ERROR					( "The specified path is invalid" );
				break;
			}
			case CELL_FS_EMFILE: {
				LOG_ERROR					( "The number of file descriptors that can be opened at the same time has reached the maximum" );
				break;
			}
			case CELL_FS_EISDIR: {
				LOG_ERROR					( "The specified path is a directory" );
				break;
			}
			case CELL_FS_EIO: {
				LOG_ERROR					( "I/O error has occurred" );
				break;
			}
			case CELL_FS_ENOMEM: {
				LOG_ERROR					( "Memory is insufficient" );
				break;
			}
			case CELL_FS_ENOTDIR: {
				LOG_ERROR					( "Components in pathcontain something other than a directory" );
				break;
			}
			case CELL_FS_ENAMETOOLONG: {	
				LOG_ERROR					( "path or components in pathexceed the maximum length" );
				break;
			}
			case CELL_FS_EFSSPECIFIC: {
				LOG_ERROR					( "File system specific internal error has occurred" );
				break;
			}
			case CELL_FS_EFAULT: {
				LOG_ERROR					( "path or file descriptor is NULL" );
				break;
			}
			case CELL_FS_EPERM: {
				LOG_ERROR					( "Permission is invalid" );
				break;
			}
			case CELL_FS_EACCES: {
				LOG_ERROR					( "Permission is invalid (Only CFS, FAT and HOSTFS)" );
				break;
			}
			case CELL_FS_EEXIST: {
				LOG_ERROR					( "When the specified file exists, CELL_FS_O_CREATand CELL_FS_O_EXCLare specified" );
				break;
			}
			case CELL_FS_ENOSPC: {
				LOG_ERROR					( "When CELL_FS_O_CREATis specified in flagsand the file does not exist, there is no area to create a new file" );
				break;
			}
			case CELL_FS_EROFS: {
				LOG_ERROR					( "When CELL_FS_O_CREATis specified in flagsand the file does not exist, the directory in which it is to be created does not permit writing (Only CFS (SYS_DEV_HDD1) and FAT)" );
				break;
			}
			case CELL_FS_ENOTMSELF: {
				LOG_ERROR					( "A file other than the MSELF file is specified in path, and either CELL_FS_O_MSELFor CELL_FS_O_RDONLY|CELL_FS_O_MSELFis specified in flags" );
				break;
			}
			default:
				NODEFAULT					( );
		}
	}

	LOGI_DEBUG								("resources:device_manager", "async %s opening %s %s", 
											m_query->is_load_type() ? "read" : "write",
											error != CELL_FS_SUCCEEDED ? "failed" : "successfull",
											detail::make_query_logging_string (m_query).c_str());

	if ( m_last_file_name != native_path )
	{
		m_sector_data_last_file_pos		=	(file_size_type)-1;
		m_last_file_name				=	native_path;
	}
	
	if ( error != CELL_FS_SUCCEEDED )
	{
		m_query->set_error_type				(query_result::error_type_cannot_open_file);
		return								false;
	}

	return									true;
}

bool   device_manager::do_async_operation (mutable_buffer out_data, u64 const file_pos, bool sector_aligned)
{
	u64 bytes_processed;
	CellFsErrno const error				=	m_query->is_load_type() ? 
		cellFsRead(*pointer_cast<int*>(&m_file_handle), out_data.c_ptr(), out_data.size(), &bytes_processed) :
		cellFsWrite(*pointer_cast<int*>(&m_file_handle), out_data.c_ptr(), out_data.size(), &bytes_processed);

	if ( error != CELL_FS_SUCCEEDED ) {
		switch ( error ) {
			case CELL_FS_EBADF: {
				LOG_ERROR					( "file descriptor is invalid" );
				break;
			}
			case CELL_FS_EBUSY: {
				LOG_ERROR					( "File is locked by stream support API" );
				break;
			}
			case CELL_FS_ENOTSUP: {
				LOG_ERROR					( "This function is not supported" );
				break;
			}
			case CELL_FS_EIO: {
				LOG_ERROR					( "I/O error has occurred" );
				break;
			}
			case CELL_FS_ENOMEM: {
				LOG_ERROR					( "Memory is insufficient" );
				break;
			}
			case CELL_FS_EFSSPECIFIC: {
				LOG_ERROR					( "File system specific internal error has occurred" );
				break;
			}
			case CELL_FS_EPERM: {
				LOG_ERROR					( "Permission is invalid" );
				break;
			}
			case CELL_FS_ENOSPC: {
				LOG_ERROR					( "No area to write into " );
				break;
			}
			case CELL_FS_EROFS: {
				LOG_ERROR					( "When CELL_FS_O_CREATis specified in flagsand the file does not exist, the directory in which it is to be created does not permit writing (Only CFS (SYS_DEV_HDD1) and FAT)" );
				break;
			}
			case CELL_FS_EFAULT: {
				LOG_ERROR					( "path or file descriptor is NULL" );
				break;
			}
			default:
				NODEFAULT					( );
		}
	}

	if ( error == CELL_FS_SUCCEEDED )
		return								true;

	R_ASSERT								(false, "file operation failed, see log for details");

	m_query->set_error_type					(m_query->is_load_type() ? 
											query_result::error_type_cannot_read_file :
											query_result::error_type_cannot_write_file);

	return									false;
}

void   device_manager::close_file ()
{
	CellFsErrno	const error				=	cellFsClose( *pointer_cast<int*>(&m_file_handle) );

	if ( error != CELL_FS_SUCCEEDED ) {
		switch ( error ) {
			case CELL_FS_EBADF: {
				LOG_ERROR					( "file descriptor is invalid" );
				break;
			}
			case CELL_FS_EBUSY: {
				LOG_ERROR					( "File is locked by stream support API" );
				break;
			}
			case CELL_FS_EFSSPECIFIC: {
				LOG_ERROR					( "File system specific internal error has occurred" );
				break;
			}
			default:
				NODEFAULT					( );
		}
	}
}

} // namespace resources
} // namespace xray
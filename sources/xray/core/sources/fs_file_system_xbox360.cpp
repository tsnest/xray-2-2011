////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system.h"
#include "fs_helper.h"
#include <xray/fs_utils.h>
#include "xray/os_include.h"
#include <xbox.h>

namespace xray {
namespace fs   {

XRAY_CORE_API bool	make_dir(pcstr portable_path);

bool initialize						( pcstr const replication_folder )
{
	long const result				= XMountUtilityDrive( false, 64*1024, 64*1024 );
	R_ASSERT_U						( result == ERROR_SUCCESS );

	if ( replication_folder )
 		make_dir					( replication_folder );

	XRAY_CONSTRUCT_REFERENCE		( g_fat, file_system );
	if ( replication_folder )
		g_fat->set_replication_dir	( replication_folder );

	return							true;
}

void finalize						( )
{
	XRAY_DESTROY_REFERENCE			( g_fat );
}

void flush_replications	( )
{
	XFlushUtilityDrive				( );
}

} // namespace fs
} // namespace xray
////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_file_system.h"
#include "fs_helper.h"
#include "fs_nodes.h"

namespace xray {
namespace fs   {

XRAY_CORE_API bool	make_dir(pcstr portable_path);

bool initialize						( pcstr const replication_folder )
{
	if ( replication_folder )
 		make_dir					( replication_folder );

	XRAY_CONSTRUCT_REFERENCE		( g_fat, file_system );
	if ( replication_folder )
		g_fat->set_replication_dir	( replication_folder );
	
	return							( true );
}

void finalize						( )
{
	XRAY_DESTROY_REFERENCE			( g_fat );
}

void flush_replications	( )
{
}

} // namespace fs
} // namespace xray
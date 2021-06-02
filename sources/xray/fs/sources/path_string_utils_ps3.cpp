////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/native_path_string.h>
#include <xray/os_include.h>
#include <sys/paths.h>

namespace xray {
namespace fs_new {

native_path_string const &			get_current_directory ();

static native_path_string 					s_current_directory;
static bool	s_initialized				=	false;

native_path_string const &			get_current_directory ()
{
	if ( s_initialized )
		return								s_current_directory;
	
	s_current_directory.assign_with_conversion	(SYS_APP_HOME);
	s_initialized						=	true;
	return									s_current_directory;
}

} // namespace fs_new
} // namespace xray
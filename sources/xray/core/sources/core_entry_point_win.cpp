////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core_entry_point.h>
#include <xray/os_include.h>
#include <xray/platform_extensions_win.h>
#include <xray/fs/path_string_utils.h>

namespace xray {
namespace core {

static string512	s_application		=	"";

void platform::preinitialize( )
{
}

void   set_application_name ( pcstr application_name )
{
	strings::copy							(s_application, application_name);
}

pcstr   application_name ( )
{
	if ( ! * s_application )
	{
		string512	path;
		if ( GetModuleFileName( 0, path, sizeof(path) ) )
		{
			pstr const last_back_slash	=	strrchr ( path, '\\' );
			if ( last_back_slash )
				strings::copy				(s_application, last_back_slash + 1);
			else
				strings::copy				(s_application, path);
		}
	}

	return									s_application;
}

pcstr   user_name ( )
{
	static string512	s_user			=	"";
	static bool			s_initialized	=	false;

	if ( ! s_initialized )
	{
		DWORD buffer_size				=	sizeof(s_user);
		GetUserName							( s_user, & buffer_size );
		R_ASSERT							( xray::platform::unload_delay_loaded_library("advapi32.dll") );
		s_initialized					=	true;
	}

	return									s_user;
}

pcstr user_data_directory	( )
{
	static fs_new::native_path_string		s_user_data_directory;
	static bool		s_initialized		=	false;

	if ( !s_initialized )
	{
		s_user_data_directory			=	fs_new::get_current_directory();
		bool const append_result		=	fs_new::append_relative_path	(
												& s_user_data_directory, 
												fs_new::native_path_string::convert("../../user_data")
											);

		XRAY_UNREFERENCED_PARAMETER			(append_result);
		s_initialized					=	true;
	}

	return									s_user_data_directory.c_str();
}

} // namespace core
} // namespace xray
////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core_entry_point.h>
#include "resources_device_manager.h"

namespace xray {
namespace core {

static string512	s_application		=	"";
static string512	s_user				=	"";
static fs_new::native_path_string	s_user_data_directory = "";
static bool			s_user_data_initialized		=	false;

void   set_application_name ( pcstr application_name )
{
	strings::copy		(s_application, application_name);
}

pcstr   application_name ( )
{
	return				s_application;
}

pcstr   user_name ( )
{
	return				s_user;
}

pcstr   user_data_directory ( )
{
	if ( !s_user_data_initialized ) {
		s_user_data_directory			=	fs_new::get_current_directory();
		
		bool const append_result		=	fs_new::append_relative_path	(
												& s_user_data_directory, 
												fs_new::native_path_string::convert("user_data")
											);
		XRAY_UNREFERENCED_PARAMETER			(append_result);
		s_user_data_initialized					=	true;
	}
	
	return				s_user_data_directory.c_str();
}

} // namespace core
} // namespace xray
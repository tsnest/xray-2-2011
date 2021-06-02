////////////////////////////////////////////////////////////////////////////
//	Created		: 01.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs_path.h>						// for fs::convert_to_native
#include <xray/os_preinclude.h>
#	undef			NOUSER            			// All USER undefs and routines
#	undef			NOMSG             			// typedef MSG and associated routines
#include <xray/os_include.h>

#include <shlwapi.h>
#pragma comment	(lib, "shlwapi.lib")

static pcstr get_current_directory	( );

pcstr const s_current_directory		= get_current_directory( );

static pcstr get_current_directory	( )
{
	static string_path				s_current_directory;
	static bool	s_initialized		= false;
	if ( s_initialized )
		return						s_current_directory;

	GetCurrentDirectory				( sizeof(s_current_directory), s_current_directory );
	xray::fs::convert_to_native_in_place( s_current_directory );
	s_initialized					= true;
	return							s_current_directory;
}

namespace xray {
namespace fs {

pcstr   get_default_user_data_folder	( )
{
	return	"../../user_data";
}

path_string   convert_to_relative_path (pcstr physical_path)
{
	path_string						out_relative_path;

	path_string	physical_path_native	=	physical_path;
	convert_to_native_in_place		(physical_path_native.c_str());
	
	pcstr const cur_dir			=	s_current_directory;
	bool const result			=	!!PathRelativePathTo(out_relative_path.c_str(), 
													     cur_dir, 
														 FILE_ATTRIBUTE_DIRECTORY,
														 physical_path_native.c_str(), 
														 FILE_ATTRIBUTE_NORMAL);
	if ( !result )
		return						physical_path;
	
	u32 const out_relative_path_length	=	out_relative_path.find((char)0);
	out_relative_path.set_length	(out_relative_path_length);
	convert_to_portable_in_place	(out_relative_path.c_str());
	
	if ( out_relative_path.length() > 2 &&
		 out_relative_path[0] == '.' && out_relative_path[1] == '/' )
	{
		path_string	const temp	=	out_relative_path.substr(2, u32(-1));
		out_relative_path		=	temp;
	}

	return							out_relative_path;
}

bool   is_absolute_path (pcstr path)
{
	return							path[0] && path[1] == ':';
}

signalling_bool   convert_to_absolute_native_path (buffer_string * out_result, 
												   pcstr relative_portable_path)
{
	verify_path_is_portable			(relative_portable_path);
	
	path_string const relative_native	=	convert_to_native(relative_portable_path);
	if ( is_absolute_path(relative_native.c_str()) )
	{
		* out_result			=	relative_native;
		return						true;
	}

	path_string	out_absolute	=	s_current_directory;
#pragma message(XRAY_TODO("Lain - revisit this after merge with Andy: get_current_directory() should not end with /"))
	out_absolute.rtrim				('\\');
	out_absolute.rtrim				('/');

	convert_to_portable_in_place	(out_absolute.c_str());
	if ( !append_relative_path(& out_absolute, relative_portable_path) )
		return						false;

	convert_to_native_in_place		(out_absolute.c_str());
	* out_result				=	out_absolute;
	return							true;
}


} // namespace fs
} // namespace xray
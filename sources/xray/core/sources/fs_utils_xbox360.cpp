////////////////////////////////////////////////////////////////////////////
//	Created		: 01.10.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

namespace xray {
namespace fs {

pcstr get_default_user_data_folder				( )
{
	return							"game:/user_data";
}

path_string convert_to_relative_path			( pcstr const physical_path )
{
	XRAY_UNREFERENCED_PARAMETER		( physical_path );
	NOT_IMPLEMENTED					( return path_string() );
}

signalling_bool convert_to_absolute_native_path (
		buffer_string * const out_result, 
		pcstr const relative_portable_path
	)
{
	XRAY_UNREFERENCED_PARAMETERS	( out_result, relative_portable_path );
	NOT_IMPLEMENTED					( return false );
}

} // namespace fs
} // namespace xray
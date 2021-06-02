////////////////////////////////////////////////////////////////////////////
//	Created		: 08.02.2011
//	Author		: Evgeniy Obertyukh
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_helper.h"
#include "shellapi.h"


namespace xray{
namespace editor_base{

void		fs_helper::remove_to_recycle_bin			( String^ dir_path )
{
	SHFILEOPSTRUCT					data;
	memory::fill8					( &data, sizeof(data), 0, sizeof(data) );
	data.wFunc						= FO_DELETE;

	u32 const string_length			= dir_path->Length;
	u32 const buffer_size			= (string_length + 2) * sizeof(char);
	pstr const folder_name_buffer	= static_cast<pstr>( ALLOCA( buffer_size ) );
	unmanaged_string full_path		= unmanaged_string( dir_path );
	pcstr const folder_name			= full_path.c_str( );
	memory::copy					( folder_name_buffer, buffer_size, folder_name, string_length );
	folder_name_buffer[string_length]		= 0;
	folder_name_buffer[string_length + 1]	= 0;
	data.pFrom						= folder_name_buffer;
	data.fFlags						= FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_ALLOWUNDO;
	/*int const operation_result	= */SHFileOperation	( &data );
}

} // namespace editor_base
} // namespace xray
////////////////////////////////////////////////////////////////////////////
//	Created		: 06.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/core_entry_point.h>
#include <sys/paths.h>
#include <xray/fs/device_file_system_interface.h>

namespace xray {

namespace command_line {
	extern fixed_string4096	g_command_line;
} // namespace command_line

namespace core {

void platform::preinitialize( )
{
}

fs_new::device_file_system_interface *	get_core_device_file_system	()
{
	NOT_IMPLEMENTED		( return 0 );
}

pcstr   get_command_line ( )
{
	pcstr const result	= command_line::g_command_line.c_str( );
	return				result ? result : "";
}

} // namespace core
} // namespace xray
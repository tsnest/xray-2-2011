////////////////////////////////////////////////////////////////////////////
//	Created 	: 25.09.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <stdarg.h>						// for va_list
#include <xray/os_preinclude.h>
#undef NOUSER
#undef NOMB
#include <xray/os_include.h>

namespace xray {
namespace logging {

static bool   s_console_initialized			=	false;
static bool   s_tried_to_initialize_console	=	false;
static HANDLE s_stdout_handle				=	NULL;
//static HANDLE s_console_handle				=	NULL;

void   write_to_stdout (pcstr format, ...)
{
	va_list					mark;
	va_start				( mark, format );

	xray::fixed_string4096	message;
	message.appendf_va_list	( format, mark );

#if 0
 	WriteFile				( s_console_handle, message.c_str(), message.size(), &num_written, NULL );
#endif // #if 0

	DWORD num_written	=	0;
 	WriteFile				( s_stdout_handle, message.c_str(), message.length(), &num_written, NULL );
	va_end					( mark );

//	printf					("%s", message.c_str());
}

void   initialize_stdout_if_needed ()
{
	if ( s_stdout_handle )
		return;

	s_stdout_handle				=	GetStdHandle(STD_OUTPUT_HANDLE);
}

bool   initialize_console ()
{
	if ( s_tried_to_initialize_console )
		return						s_console_initialized;

	s_tried_to_initialize_console	=	true;
	s_stdout_handle				=	GetStdHandle(STD_OUTPUT_HANDLE);

 	if ( !GetConsoleWindow() && !AttachConsole(ATTACH_PARENT_PROCESS) )
 	{
 		if ( !AllocConsole( ) )
 		{
 			LOG_WARNING				("cannot neither attach parent console, nor create new");
			s_console_initialized	=	false;
 			return					false;
 		}

		s_stdout_handle			=	GetStdHandle(STD_OUTPUT_HANDLE);
 	}
 
// 	s_console_handle			=	GetStdHandle(STD_OUTPUT_HANDLE);
	s_console_initialized		=	true;

	return							true;
}

void   finalize_console ()
{
	if ( s_console_initialized )
	{
		fflush						(stdout);
//		CloseHandle					(s_console_handle);
		CloseHandle					(s_stdout_handle);
		FreeConsole					();
	}
}

} // namespace xray
} // namespace logging
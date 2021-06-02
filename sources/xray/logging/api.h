////////////////////////////////////////////////////////////////////////////
//	Created		: 15.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_API_H_INCLUDED
#define XRAY_LOGGING_API_H_INCLUDED

#ifndef XRAY_LOGGING_API
#	ifdef XRAY_STATIC_LIBRARIES
#		define XRAY_LOGGING_API
#	else // #ifdef XRAY_STATIC_LIBRARIES
#		ifdef XRAY_LOGGING_BUILDING
#			define XRAY_LOGGING_API		XRAY_DLL_EXPORT
#		else // #ifdef XRAY_LOGGING_BUILDING
#			ifndef XRAY_ENGINE_BUILDING
#				define XRAY_LOGGING_API	XRAY_DLL_IMPORT
#			else // #ifndef XRAY_ENGINE_BUILDING
#				define XRAY_LOGGING_API	XRAY_DLL_EXPORT
#			endif // #ifndef XRAY_ENGINE_BUILDING
#		endif // #ifdef XRAY_LOGGING_BUILDING
#	endif // #ifdef XRAY_STATIC_LIBRARIES
#endif // #ifndef XRAY_LOGGING_API

#include <xray/fs/device_file_system_proxy.h>

namespace xray {
namespace logging {

enum log_file_usage;

enum verbosity {
	invalid		=	 0,	//	do not use it !!
	silent		=	 1,	//	1 << 0,
	error		=	 2,	//	1 << 1,
	warning		=	 3,	//	1 << 2,
	info		=	 4,	//	1 << 3,
	debug		=	 5,	//	1 << 4,
	trace		=	 6,	//	1 << 5,
	unset		=	 1 << 31,
}; // enum verbosity

XRAY_LOGGING_API	void				preinitialize			( );
XRAY_LOGGING_API	void				initialize				( fs_new::device_file_system_proxy	device, 
																  log_file_usage					log_file_usage );
XRAY_LOGGING_API	void				finalize				( );
XRAY_LOGGING_API	pcstr				verbosity_to_string		( int verbosity );
XRAY_LOGGING_API	verbosity			string_to_verbosity 	( pcstr in_verbosity );
XRAY_LOGGING_API	bool				use_console_for_logging ( );

enum stdstream_enum { stdstream_out, stdstream_error };
XRAY_LOGGING_API	void				write_to_stdstream		( stdstream_enum stream, pcstr format, ... );

} // namespace logging
} // namespace xray

#endif // #ifndef XRAY_LOGGING_API_H_INCLUDED
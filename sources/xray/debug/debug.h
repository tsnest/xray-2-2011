////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_DEBUG_H_INCLUDED
#define XRAY_DEBUG_H_INCLUDED

#include <xray/debug/api.h>

struct _exception;
struct _EXCEPTION_POINTERS;

namespace xray {

enum assert_enum;
enum process_error_enum;

namespace logging {
	enum verbosity;
} // namespace logging

namespace debug {

struct engine;

enum error_mode {
	error_mode_silent,
	error_mode_verbose,
}; // enum error_mode

enum bugtrap_usage {
	no_bugtrap,
	native_bugtrap,
	managed_bugtrap,
}; // enum error_mode

XRAY_DEBUG_API	void			preinitialize		( );
XRAY_DEBUG_API	void			initialize			( debug::engine * engine );
XRAY_DEBUG_API	void			finalize			( );

XRAY_DEBUG_API	void __cdecl	on_error			( bool* do_debug_break, process_error_enum process_error, bool* ignore_always, assert_enum assert_type, pcstr reason, pcstr expression, pcstr file, pcstr function, u32 line, pcstr format, ... );
XRAY_DEBUG_API	void			on_error			( bool* do_debug_break, process_error_enum process_error, bool* ignore_always, assert_enum assert_type, pcstr reason, pcstr expression, pcstr file, pcstr function, u32 line );

XRAY_DEBUG_API	void __cdecl	on_error			( bool* do_debug_break, process_error_enum process_error, bool* ignore_always, int error_code, pcstr expression, pcstr file, pcstr function, u32 line, pcstr format, ... );
XRAY_DEBUG_API	void			on_error			( bool* do_debug_break, process_error_enum process_error, bool* ignore_always, int error_code, pcstr expression, pcstr file, pcstr function, u32 line );

XRAY_DEBUG_API	void __cdecl	terminate			( pcstr format, ... );
XRAY_DEBUG_API	void __cdecl	terminate			( u32 exit_code, pcstr format, ... );

XRAY_DEBUG_API	void			output				( pcstr message );
XRAY_DEBUG_API	void __cdecl	printf				( pcstr format, ... );

XRAY_DEBUG_API	void			change_bugtrap_usage( debug::error_mode, debug::bugtrap_usage );

XRAY_DEBUG_API	void			dump_call_stack		( pcstr						initiator, 
													  bool						use_error_verbosity, 
													  u32 						num_first_to_ignore, 
													  u32 						num_last_to_ignore, 
													  _EXCEPTION_POINTERS*		pointers, 
													  void*						call_stack[] = 0);

XRAY_DEBUG_API	void			log_call_stack		( pcstr header );

typedef void protected_function_type				( pvoid );
XRAY_DEBUG_API	void			protected_call		( protected_function_type* function_to_call, pvoid arguments );

XRAY_DEBUG_API	int				on_math_error		( struct ::_exception* exception );

XRAY_DEBUG_API	bool			is_debugger_present	( );

XRAY_DEBUG_API	int		unhandled_exception_filter	( int const exception_code, 
													 _EXCEPTION_POINTERS* const exception_information );

XRAY_DEBUG_API	void			debug_message_box	( pcstr message );

#ifdef DEBUG
XRAY_DEBUG_API	bool			is_fpe_enabled		( );
XRAY_DEBUG_API	void			enable_fpe			( bool value );
#endif // #ifdef DEBUG

} // namespace debug
} // namespace xray

#endif // #ifndef XRAY_DEBUG_H_INCLUDED
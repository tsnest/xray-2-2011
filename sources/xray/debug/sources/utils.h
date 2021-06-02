////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_INTERNAL_H_INCLUDED
#define DEBUG_INTERNAL_H_INCLUDED

struct _SYSTEMTIME;

#include <xray/debug/engine.h>

namespace xray {
namespace debug {

void set_thread_stack_guarantee				( );
void postinitialize							( );
xray::debug::engine *	debug_engine		( );
void notify_xbox_debugger					( pcstr message );
void on_error								( pcstr message );

namespace platform {

pstr fill_format_message					( int const error_code );
void free_format_message					( pstr const buffer );

} // namespace platform

namespace bugtrap {
	void setup_unhandled_exception_handler	( );
} // namespace bugtrap

#if XRAY_PLATFORM_WINDOWS

#	define OUTPUT_DEBUG_STRING				OutputDebugString

#elif XRAY_PLATFORM_XBOX_360				// #if XRAY_PLATFORM_WINDOWS

#	ifdef DEBUG
#		pragma warning ( push )
#		pragma warning ( disable : 4005 )		// because of the DELETE macro
#		include			<xtl.h>
#		pragma warning ( pop )

#		include			<xbdm.h>
#		pragma comment	( lib, "xbdm.lib" )
#		define	OUTPUT_DEBUG_STRING			OutputDebugString
#	else // #ifdef DEBUG
#		define	OUTPUT_DEBUG_STRING( a )
#	endif // #ifdef DEBUG

#elif XRAY_PLATFORM_PS3						// #if XRAY_PLATFORM_PS3
#	define OUTPUT_DEBUG_STRING( a )			::printf("%s",(a));
#else // #elif XRAY_PLATFORM_PS3

#	error please define your platform

#endif // #elif XRAY_PLATFORM_PS3

inline pstr strings_copy			( pstr destination, u32 destination_size, pcstr source )
{
#ifdef _MSC_VER
	errno_t const error	= strcpy_s( destination, destination_size, source );
	XRAY_UNREFERENCED_PARAMETER	( error );
#elif defined(__GCC__) // #ifdef _MSC_VER
	strlcpy			( destination, source, destination_size );
#elif defined(__SNC__) // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER	( destination_size );
	strcpy			( destination, source );
#else // #elif defined(__SNC__)
#	error define strings::copy for your platform here
#endif // #ifdef _MSC_VER
	return				( destination );
}

inline pstr strings_concat			( pstr destination, u32 destination_size, pcstr source )
{
#ifdef _MSC_VER
	errno_t const error	= strcat_s( destination, destination_size, source );
	XRAY_UNREFERENCED_PARAMETER	( error );
#elif defined(__GCC__) // #ifdef _MSC_VER
	strlcat			( destination, source, destination_size );
#elif defined(__SNC__) // #ifdef _MSC_VER
	XRAY_UNREFERENCED_PARAMETER	( destination_size );
	strcat			( destination, source );
#else // #elif defined(__SNC__)
#	error define strings::copy for your platform here
#endif // #ifdef _MSC_VER
	return				( destination );
}

template < int count >
inline pstr strings_copy	( char (&destination)[count], pcstr source )
{
	return				( strings_copy(&destination[0], count, source) );
}

template < int count >
inline pstr strings_concat	( char (&destination)[count], pcstr source )
{
	return				( strings_concat(&destination[0], count, source) );
}

#ifdef _MSC_VER
typedef	long	atomic32;
#else // #ifdef _MSC_VER
typedef	u32		atomic32;
#endif // #ifdef _MSC_VER

atomic32 interlocked_increment			( atomic32 & target );
atomic32 interlocked_decrement			( atomic32 & target );
atomic32 interlocked_exchange			( atomic32 & target, atomic32 value );
atomic32 interlocked_compare_exchange	( atomic32 & target, atomic32 value, atomic32 comparand );
u32	 current_thread_id					( );
void yield								( u32 ms );

} // namespace debug
} // namespace xray

#endif // #ifndef DEBUG_INTERNAL_H_INCLUDED
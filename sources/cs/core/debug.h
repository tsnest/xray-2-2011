////////////////////////////////////////////////////////////////////////////
//	Module 		: debug.h
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : debug functionality
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_DEBUG_H_INCLUDED
#define CS_CORE_DEBUG_H_INCLUDED

#ifdef _MSC_VER
#	pragma warning(push)
#	pragma warning(disable:4995)
#	include <malloc.h>
#	pragma warning(pop)
#endif // #ifdef _MSC_VER

namespace debug {
				void			initialize				( );
				void			uninitialize			( );
#ifndef _MANAGED
	template <typename T>
	inline		T				identity		( T const& value) { return value; }
#endif // #ifndef _MANAGED

	CS_CORE_API void			use_dot_net				( bool const value );
	CS_CORE_API void			show_error				( pcstr reason, pcstr expression, pcstr file, pcstr function, u32 const line );
	CS_CORE_API void			show_error				( int const error_code, pcstr expression, pcstr file, pcstr function, u32 const line );
	CS_CORE_API void			throw_error				( pcstr reason, pcstr expression, pcstr file, pcstr function, u32 const line );
	CS_CORE_API int				sprintf_s				( pstr buffer, size_t const& buffer_size );
	CS_CORE_API int	 __cdecl	sprintf_s				( pstr buffer, size_t const& buffer_size, pcstr format, ... );
}

#if CS_PLATFORM_WINDOWS
#	define DEBUG_BREAK			__debugbreak
#elif CS_PLATFORM_XBOX_360		// #if CS_PLATFORM_WINDOWS
#	define DEBUG_BREAK			__debugbreak
#elif CS_PLATFORM_PS3			// #elif CS_PLATFORM_XBOX_360
#	define DEBUG_BREAK			__builtin_trap
#endif // #if XRAY_PLATFORM_WINDOWS


#ifndef _MANAGED
#	define CS_IDENTITY(parameter) ::debug::identity( parameter )
#else // #ifndef _MANAGED
#	define CS_IDENTITY(parameter) ( parameter )
#endif // #ifndef _MANAGED

#define FATAL(...) \
	if ( CS_IDENTITY(true) ) { \
		pstr					buffer = (pstr)ALLOCA(4096*sizeof(char)); \
		::debug::sprintf_s		( buffer, 4096, ##__VA_ARGS__ ); \
		::debug::show_error		( buffer,"FATAL error occured!",__FILE__, __FUNCTION__, __LINE__ ); \
		DEBUG_BREAK				( ); \
	} \
	else \
		(void)0

#define R_ASSERT(expression,...) \
	if ( CS_IDENTITY(true) ) { \
		if ( !CS_IDENTITY(expression) ) { \
			pstr				buffer = (pstr)ALLOCA(4096*sizeof(char)); \
			::debug::sprintf_s	( buffer, 4096, ##__VA_ARGS__ ); \
			::debug::show_error	( *buffer ? buffer : "assertion failed!",#expression,__FILE__, __FUNCTION__, __LINE__ ); \
			DEBUG_BREAK			( ); \
		} \
	} \
	else \
		(void)0

#define R_CAPI(expr) \
	if ( CS_IDENTITY(true) ) { \
		HRESULT					hr = expr; \
		if ( FAILED(hr) ) \
			::debug::show_error	( hr, #expr, __FILE__, __FUNCTION__, __LINE__ ); \
	} \
	else \
		(void)0

#define CHECK_AND_CONTINUE(expression, action, initiator, format, ...) \
	if ( !CS_IDENTITY(expression) ) { \
	msg						( initiator, cs::core::message_type_error, format, ##__VA_ARGS__ ); \
		action; \
	} \
	else \
		(void)0

#if CS_DEBUG_LIBRARIES
#	define ASSERT				R_ASSERT
#	define NODEFAULT \
		if ( CS_IDENTITY(true) ) { \
			::debug::show_error	( "assertion failed!","NODEFAULT reached",__FILE__, __FUNCTION__, __LINE__ ); \
			DEBUG_BREAK			( ); \
		} \
		else \
			(void)0

#else // #if CS_DEBUG_LIBRARIES
#	define ASSERT(...)
#	define NODEFAULT			__assume( 0 )
#endif // #if CS_DEBUG_LIBRARIES

#endif // #ifndef CS_CORE_DEBUG_H_INCLUDED
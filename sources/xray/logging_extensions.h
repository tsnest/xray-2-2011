////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_EXTENSIONS_H_INCLUDED
#define XRAY_LOGGING_EXTENSIONS_H_INCLUDED

#if XRAY_CORE_BUILDING
#	define XRAY_LOG_MODULE_INITIATOR			"core"
#endif // #if XRAY_CORE_BUILDING

#ifndef XRAY_LOG_MODULE_INITIATOR
#	error you should define XRAY_LOG_MODULE_INITIATOR macro before including this header
#endif // #ifndef XRAY_LOG_MODULE_INITIATOR

#include <fastdelegate/fastdelegate.h>

namespace xray {
namespace logging {

enum verbosity {
	silent	= 1 << 0,
	error	= 1 << 1,
	warning	= 1 << 2,
	info	= 1 << 3,
	debug	= 1 << 4,
	trace	= 1 << 5,
	unset	= 1 << 31,
}; // enum verbosity

} // namespace logging
} // namespace xray

#include <xray/logging_helper.h>
#include <xray/logging_settings.h>

namespace xray {
namespace logging {

enum callback_flag {
	first	= 1 << 0,
	last	= 1 << 1,
}; // enum callback_flag

typedef	fastdelegate::FastDelegate<
	void (
		pcstr,					// file
		pcstr,					// function signature
		int	 ,					// line
		pcstr,					// initiator
		int	 ,					// verbosity
		pcstr,					// log string
		callback_flag   		// first/last string
	)
>				log_callback;

XRAY_CORE_API	void			flush				( pcstr file_name = 0 );

XRAY_CORE_API	void			set_log_callback	( log_callback callback );
XRAY_CORE_API	log_callback	get_log_callback	( );

XRAY_CORE_API	void			push_rule			( pcstr initiator, int verbosity );
XRAY_CORE_API	void			pop_rule			( );
XRAY_CORE_API	void			on_thread_spawn		( );

} // namespace logging
} // namespace xray

#ifdef LOG_ERROR
#	error do not define LOG_ERROR macro
#endif // #ifdef LOG_ERROR
#ifdef LOG_WARNING
#	error do not define LOG_WARNING macro
#endif // #ifdef LOG_WARNING
#ifdef LOG_INFO
#	error do not define LOG_INFO macro
#endif // #ifdef LOG_INFO

#define LOG_ERROR( ... )				::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::error	) ( ##__VA_ARGS__ )
#define LOG_WARNING( ... )				::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::warning	) ( ##__VA_ARGS__ )
#define LOG_INFO( ... )					::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::info		) ( ##__VA_ARGS__ )

#ifdef DEBUG
#	define LOG_TRACE( ... )				::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::trace	) ( ##__VA_ARGS__ )
#	define LOGI_TRACE( initiator, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::trace) ( ##__VA_ARGS__ )
#else // #ifdef DEBUG
#	define LOG_TRACE( ... )
#	define LOGI_TRACE( ... )
#endif // #ifdef DEBUG

#ifndef XRAY_MASTER_GOLD
#	define LOG_DEBUG( ... )				::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::debug	) ( ##__VA_ARGS__ )
#	define LOGI_DEBUG( initiator, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::debug) ( ##__VA_ARGS__ )
#else // #ifndef XRAY_MASTER_GOLD
#	define LOG_DEBUG( ... )
#	define LOGI_DEBUG( ... )
#endif // #ifndef XRAY_MASTER_GOLD

#define LOGI_ERROR( initiator, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::error)	( ##__VA_ARGS__ )
#define LOGI_WARNING( initiator, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::warning) ( ##__VA_ARGS__ )
#define LOGI_INFO( initiator, ... )		::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::info) ( ##__VA_ARGS__ )
#define LOGI( initiator, type, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", type) ( ##__VA_ARGS__ )

#endif // #ifndef XRAY_LOGGING_EXTENSIONS_H_INCLUDED
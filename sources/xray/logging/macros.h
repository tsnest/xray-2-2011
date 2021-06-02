////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_LOGGING_MACROS_H_INCLUDED
#define XRAY_LOGGING_LOGGING_MACROS_H_INCLUDED

#ifdef LOG_ERROR
#	error do not define LOG_ERROR macro
#endif // #ifdef LOG_ERROR
#ifdef LOG_WARNING
#	error do not define LOG_WARNING macro
#endif // #ifdef LOG_WARNING
#ifdef LOG_INFO
#	error do not define LOG_INFO macro
#endif // #ifdef LOG_INFO

#define LOG_ERROR( ... )				::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" , ::xray::logging::error) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::error	) ( ##__VA_ARGS__ ) : 0

#define LOG_WARNING( ... )				::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" , ::xray::logging::warning) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::warning	) ( ##__VA_ARGS__ ) : 0

#define LOG_INFO( ... )					::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" , ::xray::logging::info) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::info		) ( ##__VA_ARGS__ ) : 0

#ifdef DEBUG
#	define LOG_TRACE( ... )				::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" , ::xray::logging::trace) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::trace	) ( ##__VA_ARGS__ ) : 0
#	define LOGI_TRACE( initiator, ... )	::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::trace) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::trace) ( ##__VA_ARGS__ ) : 0
#else // #ifdef DEBUG
#	define LOG_TRACE( ... )
#	define LOGI_TRACE( ... )
#endif // #ifdef DEBUG

#ifndef XRAY_MASTER_GOLD
#	define LOG_DEBUG( ... )				::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" , ::xray::logging::debug) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", ::xray::logging::debug	) ( ##__VA_ARGS__ ) : 0

#	define LOGI_DEBUG( initiator, ... )	::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::debug) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::debug) ( ##__VA_ARGS__ ) : 0
#else // #ifndef XRAY_MASTER_GOLD
#	define LOG_DEBUG( ... )
#	define LOGI_DEBUG( ... )
#endif // #ifndef XRAY_MASTER_GOLD

#define LOGI_ERROR( initiator, ... )	::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::error) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::error)	( ##__VA_ARGS__ ) : 0

#define LOGI_WARNING( initiator, ... )	::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::warning) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::warning) ( ##__VA_ARGS__ ) : 0

#define LOGI_INFO( initiator, ... )		::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::info) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", ::xray::logging::info) ( ##__VA_ARGS__ ) : 0

#define LOGI( initiator, type, ... )	::xray::logging::check_verbosity(XRAY_LOG_MODULE_INITIATOR ":" initiator ":", type) ? \
										::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", type) ( ##__VA_ARGS__ ) : 0

#define LOGI_FORCED( initiator, type, ... )	::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":" initiator ":", type) ( ##__VA_ARGS__ )

#define LOG_FORCED( type, ... )				::xray::logging::helper	( __FILE__, __FUNCSIG__, __LINE__, XRAY_LOG_MODULE_INITIATOR ":", type) ( ##__VA_ARGS__ )


#endif // #ifndef XRAY_LOGGING_LOGGING_MACROS_H_INCLUDED
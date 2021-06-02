////////////////////////////////////////////////////////////////////////////
//	Module 		: core.cpp
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : core functionality
////////////////////////////////////////////////////////////////////////////

#include "pch_script.h"
#include <cs/core/os_include.h>

#ifndef CS_STATIC_LIBRARIES
#	include "delayimp.h"
#else // #ifndef CS_STATIC_LIBRARIES
#	if CS_PLATFORM_WINDOWS
#		pragma comment(lib, "advapi32.lib")
#		pragma comment(lib, "user32.lib")
#	endif // #elif CS_PLATFORM_WINDOWS
#endif // #ifndef CS_STATIC_LIBRARIES

namespace cs {
	namespace core {
		CS_CORE_API cs_core core;
	} // namespace core
} // namespace cs

static u32	user_counter	= 0;
static bool s_use_debug_engine	= true;
static bool s_use_logging		= true;

using cs::core::logging::user_callback_type;

void dump_memory_leaks								();
user_callback_type const& cs_core_logging_callback	();
pvoid cs_core_logging_callback_user_data			();

void cs_core_cleanup				()
{
	if (!user_counter)
		return;

	msg								(
		cs::message_initiator_core,
		cs::core::message_type_warning,
		"leaked %s core uninitializers",
		user_counter
	);

	for (u32 i=0; i<user_counter; ++i)
		cs::core::core.uninitialize	();
}

cs_core::cs_core					() :
	m_application_name		(0),
	m_user_name				(0)
{
}

void cs_core::initialize			( pcstr const library_user, bool const use_debug_engine, bool const use_logging )
{
	if (!user_counter)
		initialize_impl		(library_user, use_debug_engine, use_logging);

	++user_counter;
	msg						(cs::message_initiator_core, cs::core::message_type_information,"%s intialized cs::core library",library_user);
}

void cs_core::uninitialize			()
{
	ASSERT					(user_counter, "core is uninitialized more times than it was initialized");

	--user_counter;
	if (user_counter)
		return;

	user_callback_type		callback = cs_core_logging_callback();
	pvoid					user_data = cs_core_logging_callback_user_data();
	cs_core_logging_callback( 0, 0 );

	msg						(cs::message_initiator_core, cs::core::message_type_information,"cs::core library is uninitialized");
	if ( s_use_logging )
		logging::uninitialize	();
	::string::uninitialize	();

	cs_free					(m_application_name);
	cs_free					(m_user_name);

	memory.uninitialize		();

	if ( s_use_debug_engine )
		debug::uninitialize	();

#if CS_DEBUG_LIBRARIES
	size_t					stats = cs::core::memory_stats();
	string256				temp;
	cs::sprintf				(temp, "memory left: %d byte(s)\r\n", stats);
	OUTPUT_DEBUG_STRING		(temp);
	dump_memory_leaks		();
#endif // #if CS_DEBUG_LIBRARIES

	cs_core_logging_callback( callback, user_data );
}

void cs_core::initialize_impl		(pcstr const library_user, bool const use_debug_engine, bool const use_logging )
{
	memory.initialize		();

	s_use_debug_engine		= use_debug_engine;
	if ( s_use_debug_engine )
		debug::initialize	();

	m_application_name		= sz_dup(library_user);

	string256				user_name;
#if CS_PLATFORM_WINDOWS
	DWORD					size = sizeof(user_name);
	GetUserName				(user_name,&size);
//	__FUnloadDelayLoadedDLL2("advapi32.dll");
#else // #if CS_PLATFORM_WINDOWS
	sz_cpy					(user_name, "");
#endif // #if CS_PLATFORM_WINDOWS
	m_user_name				= sz_dup(user_name);

	s_use_logging			= use_logging;
	if ( s_use_logging )
		logging::initialize	();

	::string::initialize	();
	build::initialize		();
}

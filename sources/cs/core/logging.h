////////////////////////////////////////////////////////////////////////////
//	Module 		: log_message_types.h
//	Created 	: 24.09.2006
//  Modified 	: 24.09.2006
//	Author		: Dmitriy Iassenev
//	Description : log message types
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_LOGGING_H_INCLUDED
#define CS_CORE_LOGGING_H_INCLUDED

#include <fastdelegate/fastdelegate.h>

#ifndef CS_CORE_API
#	define CS_CORE_API							CS_API
#endif // #ifndef CS_CORE_API

#ifndef CS_CORE_CALL
#	define CS_CORE_CALL							CS_CALL
#endif // #ifndef CS_CORE_CALL

namespace cs {

enum message_initiator {
	message_initiator_core = (unsigned int)(0),
	message_initiator_lua_studio_backend,
	message_initiator_script,
	message_initiator_render_debug,
	message_initiator_decision,
	message_initiator_navigation,
	message_initiator_brain_unit,
	message_initiator_ai,
	message_initiator_lua_studio,
	message_initiator_behaviour_studio,
	message_initiator_render,

	message_initiator_count,
}; // enum message_initiator

namespace core {
	enum message_type {
		message_type_error = (unsigned int)(0),
		message_type_warning,
		message_type_output,
		message_type_information,
		message_type_count,
	}; // enum message_type

	namespace logging {
		typedef void (CS_CORE_CALL* user_callback_type) 		(cs::message_initiator const&, cs::core::message_type const&, char const* const&, void* user_data);
	} // namespace logging
} // namespace core

} // namespace cs

extern "C" {
	CS_CORE_API	void CS_CORE_CALL	cs_core_logging_callback	(cs::core::logging::user_callback_type const&, void* const& user_data);
}

#endif // #ifndef CS_CORE_LOGGING_H_INCLUDED
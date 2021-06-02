////////////////////////////////////////////////////////////////////////////
//	Module 		: log.cpp
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : log
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/log_extensions.h>
#include <cs/core/os_include.h>		// OUTPUT_DEBUG_STRING
#include <stdio.h>
#include <stdarg.h>
#include "log_file.h"

static logging::callback_type	s_internal_log_callback;
static log_file*				s_log_file = 0;

static cs::core::logging::user_callback_type	s_user_callback = 0;
static pvoid									s_user_data = 0;

void CS_CORE_CALL cs_core_logging_callback	(cs::core::logging::user_callback_type const& callback, pvoid const& user_data)
{
	s_user_callback				= callback;
	s_user_data					= user_data;
}

cs::core::logging::user_callback_type const& cs_core_logging_callback	()
{
	return						s_user_callback;
}

pvoid cs_core_logging_callback_user_data	()
{
	return						s_user_data;
}

void logging::initialize		()
{
	ASSERT						(!s_log_file, "log file has been already initialized");
	s_log_file					= cs_new<log_file>();
}

void logging::uninitialize		()
{
	ASSERT						(s_log_file, "log file hasn't been initialized");
	cs_delete					(s_log_file);
}

void logging::flush				()
{
	if (s_log_file)
		s_log_file->flush		();
}

void logging::callback			(callback_type const& callback)
{
	s_internal_log_callback		= callback;
}

logging::callback_type const& logging::callback	()
{
	return						s_internal_log_callback;
}

static void output_string		(cs::message_initiator const& initiator, cs::core::message_type const& message_type, pcstr line, bool show_debug_string)
{
	if (s_log_file)
		s_log_file->add_line	(line);
	
	if ( IS_DEBUGGER_PRESENT() ) {
		pcstr					message_initiator = 0;
		switch (initiator) {
			case cs::message_initiator_core : {
				message_initiator	= "[core]";
				break;
			}
			case cs::message_initiator_lua_studio_backend : {
				message_initiator	= "[lua_studio_backend]";
				break;
			}
			case cs::message_initiator_script : {
				message_initiator	= "[script]";
				break;
			}
			case cs::message_initiator_render_debug : {
				message_initiator	= "[render_debug]";
				break;
			}
			case cs::message_initiator_decision : {
				message_initiator	= "[decision]";
				break;
			}
			case cs::message_initiator_navigation : {
				message_initiator	= "[navigation]";
				break;
			}
			case cs::message_initiator_brain_unit : {
				message_initiator	= "[brain_unit]";
				break;
			}
			case cs::message_initiator_ai : {
				message_initiator	= "[ai]";
				break;
			}
			case cs::message_initiator_lua_studio : {
				message_initiator	= "[lua_studio]";
				break;
			}
			case cs::message_initiator_behaviour_studio : {
				message_initiator	= "[behaviour_studio]";
				break;
			}
			case cs::message_initiator_render : {
				message_initiator	= "[render]";
				break;
			}
			default				: NODEFAULT;
		}
		pcstr					message_type_string = 0;
		switch (message_type) {
			case cs::core::message_type_error : {
				message_type_string	= "[error]";
				show_debug_string	= true;
				break;
			}
			case cs::core::message_type_warning : {
				message_type_string	= "[warning]";
				break;
			}
			case cs::core::message_type_output : {
				message_type_string	= "[output]";
				break;
			}
			case cs::core::message_type_information : {
				message_type_string	= "[info]";
				break;
			}
			default : NODEFAULT;
		}

		string4096				temp;
		const int				max_header_length = 29; // sz_len("[lua_studio_backend][warning]")
		string256				temp0;
		int						size = cs::sprintf(temp0, "%s%s", message_type_string, message_initiator);
		memset					(temp0 + size,' ',(max_header_length - size)*sizeof(char));
		temp0[max_header_length]= 0;
		cs::sprintf				(temp, "%s : %s\r\n", temp0, line);
		OUTPUT_DEBUG_STRING		(temp);
	}

	if (s_internal_log_callback)
		s_internal_log_callback	(initiator, message_type, line);

	if (s_user_callback)
		s_user_callback			(initiator, message_type, line, s_user_data);
}

void logging::log				(
		cs::message_initiator const& initiator,
		cs::core::message_type const& message_type,
		pcstr message
	)
{
	bool						show_debug_string = true;//false;
	string4096					temp;
	*temp						= 0;
	pstr						temp_begin = temp;
	pstr						j = temp_begin;
	for (pcstr i = message; *i; ++i) {
		if ((*i != '\n') && (*i != '\r')) {
			*j++				= *i;
			continue;
		}

		*j						= 0;
		if (*temp_begin)
			output_string		(initiator, message_type, temp, show_debug_string);
		j						= temp_begin;
	}
	
	*j							= 0;
	if (*temp_begin)
		output_string			(initiator, message_type, temp, show_debug_string);
}

int	vsmsg						(
		cs::message_initiator const& initiator,
		cs::core::message_type const& message_type,
		pcstr format,
		va_list mark
	)
{
	string4096					buffer;
	int							result = cs::vsnprintf(buffer, sizeof(buffer), sizeof(buffer) - 1, format, mark);
	logging::log				(initiator, message_type, buffer);
	return						result;
}

int msg							(
		cs::message_initiator const& initiator,
		cs::core::message_type const& message_type,
		pcstr format,
		...
	)
{
	va_list						args;
	va_start					(args, format);
	return						vsmsg(initiator, message_type, format, args);
}

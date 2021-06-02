////////////////////////////////////////////////////////////////////////////
//	Module 		: debug.cpp
//	Created 	: 24.08.2006
//  Modified 	: 24.08.2006
//	Author		: Dmitriy Iassenev
//	Description : debug functionality
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/debug.h>
#include <stdio.h>
#include <stdarg.h>
#include <cs/core/os_include.h> // FormatMessage and the others

namespace debug {
	void	on_error	(pcstr message);
} // namespace debug

#if CS_PLATFORM_WINDOWS
std::string api_error_code		(int const error_code)
{
	pstr				message_buffer = 0;
	FormatMessage		(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,0,error_code,0,(LPSTR)&message_buffer,0,0);
	std::string			result = message_buffer;
	LocalFree			(message_buffer);
	return				result;
}
#endif // #if CS_PLATFORM_WINDOWS

static void printf_all			(string4096& buffer, pcstr format, ...)
{
	string4096			temp;
	va_list				arguments;
	va_start			(arguments,format);
	cs::vsprintf		(temp, sizeof(temp), format, arguments);
	msg					(cs::message_initiator_core, cs::core::message_type_error, "%s", temp);
	sz_concat			(buffer, temp, "\r\n");
}

void debug::show_error			(pcstr reason, pcstr expression, pcstr file, pcstr function, u32 const line)
{
	string4096			message;
	sz_cpy				(message, "");

	printf_all			(message, "\r\n");
	printf_all			(message, "Error occured : %s",reason);
	printf_all			(message, "Expression    : %s",expression);
	printf_all			(message, "File          : %s",file);
	printf_all			(message, "Line          : %d",line);
	printf_all			(message, "Function      : %s\r\n",function);

	logging::flush		();

	debug::on_error		(message);
}

void debug::show_error			(int const error_code, pcstr expression, pcstr file, pcstr function, u32 const line)
{
	string4096			message;
	sz_cpy				(message, "");

	printf_all			(message, "\r\n");
#if CS_PLATFORM_WINDOWS
	printf_all			(message, "API failure   : %s(%s)",api_error_code(error_code).c_str());
#endif // #if CS_PLATFORM_WINDOWS
	printf_all			(message, "Expression    : %s",expression);
	printf_all			(message, "File          : %s",file);
	printf_all			(message, "Line          : %d",line);
	printf_all			(message, "Function      : %s\r\n",function);

	logging::flush		();

	debug::on_error		(message);
}

void debug::throw_error			(pcstr reason, pcstr expression, pcstr file, pcstr function, u32 const line)
{
	string4096			message;
	sz_cpy				(message, "");

	printf_all			(message, "\r\n");
	printf_all			(message, "Error occured : %s(%s)",reason);
	printf_all			(message, "Expression    : %s",expression);
	printf_all			(message, "File          : %s",file);
	printf_all			(message, "Line          : %d",line);
	printf_all			(message, "Function      : %s\r\n",function);

	logging::flush		();

	debug::on_error		(message);
}

int debug::sprintf_s				(pstr buffer, size_t const& buffer_size)
{
	CS_UNREFERENCED_PARAMETER	( buffer_size );
	ASSERT				(buffer_size > 0);
	*buffer				= 0;
	return				0;
}

int	debug::sprintf_s				(pstr buffer, size_t const& buffer_size, pcstr format, ...)
{
	va_list				arguments;
	va_start			(arguments,format);
	return				cs::vsprintf(buffer,buffer_size,format,arguments);
}

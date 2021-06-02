////////////////////////////////////////////////////////////////////////////
//	Created		: 19.10.2011
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CALL_STACK_H_INCLUDED
#define CALL_STACK_H_INCLUDED

#include <xray/stdlib_extensions.h>

#if defined(_MSC_VER)
#	include <malloc.h>
#	define ALLOCA						_alloca
#elif defined(__GNUC__) // #if defined(_MSC_VER)
#	include <alloca.h>
#	define ALLOCA						alloca
#else // #elif defined(__GNUC__)
#	error please define macros for your compiler here
#endif // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360

namespace xray {
namespace debug {

struct call_stack_size_calculator {
	call_stack_size_calculator	() :	m_size(0) { }

	bool	predicate	(u32 call_stack_id, u32	num_call_stack_lines, pcstr module_name, 
						 pcstr file_name, int line_number, pcstr function, size_t address)
	{
		XRAY_UNREFERENCED_PARAMETERS			( num_call_stack_lines, call_stack_id );
		static pcstr s_full_call_stack_line_format_local	= "%s(%d) : %s : %s : 0x%08x";
		static pcstr s_call_stack_line_format_local			= "%s       : %s : 0x%08x";
		char								buffer[8192];
		if ( line_number > 0 )
			xray::sprintf					(buffer, sizeof(buffer), s_full_call_stack_line_format_local, file_name, 
											 line_number, function, module_name, address);
		else
			xray::sprintf					(buffer, sizeof(buffer), s_call_stack_line_format_local, module_name, function, address);
		m_size							+=	strlen(buffer) + 2;
		return								true;
	}

	u32		needed_size					() const { return m_size; }
private:
	u32										m_size;
}; // struct call_stack_size_calculator

} // namespace debug
} // namespace xray

#endif // #ifndef CALL_STACK_H_INCLUDED
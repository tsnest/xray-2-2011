#include "pch.h"
#include <xray/debug_macros.h>
#include <xray/fixed_string.h>

namespace xray {
namespace debug {
namespace detail {

u32	  string_helper::size () const
{
	return					strings::length(m_buffer);
}
	
void   string_helper::appendf (pcstr format, ...)
{
	va_list					argptr;
	va_start 				(argptr, format);
	appendf_va_list			(format, argptr);
	va_end	 				(argptr);
}

void   string_helper::appendf_va_list (pcstr const format, va_list argptr)
{
	buffer_string string	(m_buffer, array_size(m_buffer), strings::length(m_buffer));
	string.appendf_va_list	(format, argptr);
}

} // namespace detail
} // namespace debug
} // namespace xray

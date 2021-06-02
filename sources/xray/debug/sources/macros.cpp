#include "pch.h"
#include <string.h>
#include <xray/debug/extensions.h>
#include <xray/stdlib_extensions.h>

namespace xray {
namespace debug {
namespace detail {

u32	  string_helper::size () const
{
	return					(u32)strlen(m_buffer);
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
	u32 const old_size	=	(u32)strlen(m_buffer);
	vsprintf				(m_buffer + old_size, sizeof(m_buffer) - old_size, format, argptr);
}

} // namespace detail
} // namespace debug
} // namespace xray

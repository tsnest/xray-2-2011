///////////////////////////////////////////////////////////////////////////
//	Created		: 18.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
///////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/fs/native_path_string.h>
#include <xray/fs/portable_path_string.h>
#include <xray/fs/virtual_path_string.h>

namespace xray {
namespace fs_new {

#if XRAY_PLATFORM_PS3
const char native_path_string::separator;
const char portable_path_string::separator;
#endif // #if XRAY_PLATFORM_PS3

native_path_string::native_path_string (portable_path_string const & other) : path_string_impl(separator)
{
	assign_with_conversion					(other.c_str());
}

portable_path_string::portable_path_string (native_path_string const & other) : path_string_impl(separator)
{
	assign_with_conversion					(other.c_str());
}

void   path_string_impl::convert (iterator begin, iterator end)
{
	char const other_separator			=	(m_separator == '/') ? '\\' : '/';
	for ( iterator it = begin, it_end = end; it != it_end; ++it )
		if ( * it == other_separator )
			* it						=	m_separator;
}

} // namespace fs_new
} // namespace xray
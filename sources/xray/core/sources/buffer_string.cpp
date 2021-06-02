////////////////////////////////////////////////////////////////////////////
//	Created 	: 11.11.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray/buffer_string.h"
#include <stdarg.h>

namespace xray {

//-----------------------------------------------------------------------------------
// operations
//-----------------------------------------------------------------------------------

buffer_string const&   buffer_string::operator += (value_type const * s)
{
	if ( !s )
		return						* this;

	ASSERT(m_end + strings::length(s) < m_max_end, "tried to add '%s'\n to '%s'", s, c_str());

	while ( * s && m_end < m_max_end )
	{
		* m_end = * s;
		++m_end;
		++s;
	}

	* m_end						=	NULL;
	return							* this;
}

buffer_string &   buffer_string::assignf (pcstr const format, ...)
{
	clear							();
	va_list							argptr;
	va_start 						(argptr, format);
	appendf_va_list					(format, argptr);
	va_end	 						(argptr);
	return							* this;
}

buffer_string &   buffer_string::appendf (pcstr const format, ...)
{
	va_list							argptr;
	va_start 						(argptr, format);
	appendf_va_list					(format, argptr);
	va_end	 						(argptr);
	return							* this;
}

buffer_string &   buffer_string::appendf_va_list (pcstr const format, va_list argptr)
{
	char	 						temp_buffer[1024*4 - 8];
	int const char_count		=	vsnprintf(temp_buffer, sizeof(temp_buffer), format, argptr);
	ASSERT	 						(char_count != -1);
	return							append((char*)temp_buffer, (char*)temp_buffer + char_count);
}

void   buffer_string::substr (size_type const pos, size_type count, buffer_string * out_dest) const
{
	ASSERT							(out_dest);
	out_dest->clear					();

	if ( count == npos )
		count					=	length() - pos;
	else
	{
		size_type	src_max_count	=	length() > pos ? length() - pos : 0;
		if ( count > src_max_count )
			count	=	src_max_count;

		size_type	dest_max_count	=	out_dest->max_length();
		if ( count > dest_max_count )
			count				=	dest_max_count;
	}

	out_dest->append				(m_begin + pos, m_begin + pos + count);
}

buffer_string &   buffer_string::make_uppercase ()
{
	if ( length() )
		strupr						(m_begin, length() + 1);
	return							* this;
}

buffer_string &   buffer_string::make_lowercase ()
{
	if ( length() )
		strlwr						(m_begin, length() + 1);
	return							* this;
}

bool   buffer_string::is_lowercase () const
{
	return							strings::is_lower_case(c_str());
}

buffer_string &	  buffer_string::replace (pcstr what, pcstr with)
{
	u32 const what_length		=	xray::strings::length(what);
	u32 const with_length		=	xray::strings::length(with);
	CURE_ASSERT						(what_length, return * this);
	u32 const max_result_buffer	=	math::max(length(), (length() / what_length + 1) * with_length) + 1;
	buffer_string					temp((pstr)ALLOCA(max_result_buffer), max_result_buffer);

	u32 pos						=	npos;
	while ( (pos = find(what)) != npos )
	{
		temp.assign					(begin(), begin() + pos);
		temp.append					(with);
		temp.append					(begin() + pos + what_length, end());
		* this					=	temp;
	}

	return							* this;
}

buffer_string &   buffer_string::assign_replace (pcstr source, pcstr what, pcstr with)
{
	* this						=	source;
	replace							(what, with);
	return							* this;
}

void   get_common_prefix (buffer_string * out_string, pcstr first, pcstr second)
{
	u32 const common_prefix_length	=	strings::common_prefix_length(first, second);
	out_string->assign				(first, first + common_prefix_length);
}

} // namespace xray
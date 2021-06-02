////////////////////////////////////////////////////////////////////////////
//	Created		: 31.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_FORMAT_H_INCLUDED
#define XRAY_LOGGING_FORMAT_H_INCLUDED

#include <xray/logging/format_specifier.h>

namespace xray {
namespace logging {

struct log_format
{
	format_string_type						string;
	format_enabled_container				enabled;
	format_index_container					indexes;

	void	set							(format_specifier const & format_expression);
};

} // namespace logging
} // namespace xray

#endif // #ifndef XRAY_LOGGING_FORMAT_H_INCLUDED
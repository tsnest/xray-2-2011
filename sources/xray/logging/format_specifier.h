////////////////////////////////////////////////////////////////////////////
//	Created		: 25.08.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_FORMAT_SPECIFIER_H_INCLUDED
#define XRAY_LOGGING_FORMAT_SPECIFIER_H_INCLUDED

#include <xray/logging/noncopyable.h>
#include <xray/fixed_string.h>
#include <xray/containers_extensions.h>

namespace xray {
namespace logging {

enum format_specifier_enum
{
	format_specifier_unset,
	format_specifier_thread_id,
	format_specifier_initiator,
	format_specifier_time,
	format_specifier_verbosity,
	format_specifier_message,
	format_specifier_separator,
	format_specifier_count,	
};

typedef	fixed_vector<format_specifier_enum, format_specifier_count>		format_specifier_list;
typedef	int		format_index_container[format_specifier_count];
typedef	bool	format_enabled_container[format_specifier_count];
typedef fixed_string<512>												format_string_type;
// this is node of a tree that represents an ordered list of format specifiers enums
// that are used to format logging
class format_specifier : private noncopyable
{
public:
	format_specifier					(format_specifier_enum specifier);
	format_specifier					(format_specifier const & left, format_specifier const & right);

	void	fill_specifier_list			(format_specifier_list & list, format_string_type * out_format_string) const;

private:
	format_specifier const *			m_left;
	format_specifier const *			m_right;

	format_specifier_enum 				m_specifier;
};

struct format_separator : format_specifier
{
	format_separator					(pcstr separator) 
		: format_specifier(format_specifier_separator), separator(separator) {}
	
	fixed_string<128>					separator;
};

inline
format_specifier	operator +			(format_specifier const & left, format_specifier const & right)
{
	return									format_specifier(left, right);
}

} // namespace logging
} // namespace xray

#endif // #ifndef XRAY_LOGGING_FORMAT_SPECIFIER_H_INCLUDED
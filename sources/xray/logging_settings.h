////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef LOGGING_SETTINGS_H_INCLUDED
#define LOGGING_SETTINGS_H_INCLUDED

namespace xray {
namespace logging {

class XRAY_CORE_API settings : private detail::noncopyable {
public:
	enum			flags_enum { flags_log_only_user_string	=	1, 
								 flags_log_to_console		=	2, };
	flags_enum		flags;

	explicit		settings			( pcstr initiator_prefix, u32 flags = 0 );
	inline	pcstr	get_initiator_prefix() const { return m_initiator_prefix; }

private:
					settings			();
	string256		m_initiator_prefix;
}; // class settings

} // namespace logging
} // namespace xray

#endif // #ifndef LOGGING_SETTINGS_H_INCLUDED
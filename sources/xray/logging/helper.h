////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_LOGGING_HELPER_H_INCLUDED
#define XRAY_LOGGING_HELPER_H_INCLUDED

#include <xray/logging/api.h>
#include <xray/logging/format.h>

namespace xray {
namespace logging {

struct XRAY_LOGGING_API helper_data : private core::noncopyable {
	pcstr			m_initiator;
	pcstr			m_file;
	pcstr			m_function_signature;
	int				m_line;
	verbosity		m_verbosity;

					helper_data	(
						pcstr 		file,
						pcstr 		function_signature,
						int   		line,
						pcstr 		major_initiator,
						verbosity	verbosity
					);
}; // struct helper_data

enum	log_flags_enum;

bool XRAY_LOGGING_API check_verbosity (pcstr initiator, verbosity verbosity);

class XRAY_LOGGING_API helper : private helper_data {
public:
					helper		(
						pcstr 		file,
						pcstr 		function_signature,
						int   		line,
						pcstr 		major_initiator,
						verbosity	verbosity
					);
	void __cdecl	operator( )	( log_flags_enum const log_flags, pcstr format, ... );
	void __cdecl	operator( )	( format_specifier const & log_format, pcstr format, ... );
	void __cdecl	operator( )	( format_specifier const & log_format, int const log_flags, pcstr format, ... );
	void __cdecl	operator( )	( log_format *  format_list, int const log_flags, pcstr format, ... );
	void __cdecl	operator( )	( pcstr format, ... );
}; // class helper

} // namespace logging
} // namespace xray

#endif // #ifndef XRAY_LOGGING_HELPER_H_INCLUDED
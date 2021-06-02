////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "path_parts.h"
#include <xray/logging/api.h>
#include <xray/logging/helper.h>
#include <xray/logging/extensions.h>
#include <xray/logging/log_file.h>
#include <xray/buffer_string.h>
#include <stdarg.h>						// for va_list
#include <xray/fs/native_path_string.h>
#include <xray/fs/device_utils.h>
#include <xray/debug/log_callback.h>

#include "logging.h"
#include "globals.h"

using xray::logging::helper;
using xray::logging::helper_data;
using xray::logging::path_parts;
using xray::logging::log_callback;
using xray::logging::log_file;

static log_callback				s_log_callback	= 0;

xray::logging::log_file_usage	 get_log_file_usage ();

namespace xray {
namespace logging {

void fill_local_time( xray::buffer_string& dest );

void set_log_callback			(log_callback callback)
{
	ASSERT						(callback);
	s_log_callback				= callback;
}

log_callback get_log_callback	()
{
	return						( s_log_callback );
}

int get_tree_verbosity			( path_parts* const path );
void log_thread_unsafe			( logging::log_flags_enum const log_flags, pcstr string, u32 string_length, logging::verbosity verbosity );

} // namespace logging
} // namespace xray

helper_data::helper_data		(
		pcstr const file,
		pcstr const function_signature,
		int const line,
		pcstr const initiator,
		xray::logging::verbosity const verbosity
	) :
	m_file					( file ),
	m_function_signature	( function_signature ),
	m_line					( line ),
	m_verbosity				( verbosity ),
	m_initiator				( initiator )
{
}

helper::helper					(
		pcstr const file,
		pcstr const function_signature,
		int const line,
		pcstr const initiator,
		xray::logging::verbosity const verbosity
	) :
	helper_data				(
		file,
		function_signature,
		line,
		initiator,
		verbosity
	)
{
}

static void fill_log_string				( xray::buffer_string &					dest,
										  pcstr const							message_start, 
										  pcstr const							message_end, 
										  xray::logging::path_parts &			path, 
										  int const								verbosity,
										  xray::logging::log_format const &		format )
{
	using namespace xray;
	using namespace xray::logging;

	format_string_type	strings_storage		[format_specifier_count];
	buffer_string *		strings				[format_specifier_count];
	for ( int i=0; i<format_specifier_count; ++i )
		strings[i]						=	& strings_storage[i];

	u32 const message_length			=	message_end - message_start;
	buffer_string	message_string			((pstr)message_start, message_length + 1, message_length);
	char const saved_end_char			=	* message_end;
	* (pstr)message_end					=	NULL;

	strings[format_specifier_message]	=	& message_string;
	
	if ( format.enabled[format_specifier_initiator] )
	{
		path.concat2buffer					(* strings[format_specifier_initiator]);

		for ( pstr	i	=	(* strings[format_specifier_initiator]).begin(),
					end	=	(* strings[format_specifier_initiator]).end();
					i != end; ++i )
		{
			if ( *i == '/' )	
				*i						=	':';
		}
	}
	if ( format.enabled[format_specifier_thread_id] )
	{
		(* strings[format_specifier_thread_id]).assignf	("%-8s", xray::threading::current_thread_logging_name());
	}
	if ( format.enabled[format_specifier_time] )
	{
		fill_local_time						(* strings[format_specifier_time]);
	}
	if ( format.enabled[format_specifier_verbosity] )
	{
		* strings[format_specifier_verbosity]	=	verbosity_to_string(verbosity);
	}

	COMPILE_ASSERT							(format_specifier_count == 7, OMG_FIX_BELOW_THEN);
	dest.assignf							(format.string.c_str(),
 												(* strings[format.indexes[0]]).c_str(), 
												(* strings[format.indexes[1]]).c_str(),
 												(* strings[format.indexes[2]]).c_str(), 
												(* strings[format.indexes[3]]).c_str(),
 												(* strings[format.indexes[4]]).c_str(), 
												(* strings[format.indexes[5]]).c_str());

	* (pstr)message_end					=	saved_end_char;
}

struct predicate : private xray::logging::noncopyable
{
	pcstr											m_file;
	pcstr											m_function_signature;
	int												m_line;
	xray::logging::verbosity						m_verbosity;
	xray::logging::log_flags_enum const				m_log_flags;
	xray::logging::log_format const &				m_format;
	xray::logging::path_parts &						m_path;

	inline		predicate		(
			pcstr const 									file,
			pcstr const 									function_signature,
			int const										line,
			xray::logging::verbosity const					verbosity,
			xray::logging::log_flags_enum const				log_flags,
			xray::logging::path_parts &						path,
			xray::logging::log_format const &				format
		) :
		m_file					( file ),
		m_function_signature	( function_signature ),
		m_line					( line ),
		m_verbosity				( verbosity ),
		m_log_flags				( log_flags ),
		m_path					( path ),
		m_format				( format )
	{
	}

	inline bool	operator ( )	( u32 const index, pcstr string, u32 const length, bool const is_last ) const
	{
		char const	end_line[]			=	"\r\n";
		u32 const	end_line_length		=	xray::array_size(end_line) - 1; // counts trailing zero, so need to -1
		u32 const	final_length		=	128 + length + end_line_length + 1;

		xray::buffer_string	final_string	((pstr)ALLOCA(final_length*sizeof(char)), final_length);

		fill_log_string						(final_string, string, string+length, 
											 m_path, m_verbosity, m_format);

		xray::threading::mutex_raii	raii	(xray::logging::globals->log_mutex);

		final_string.append					( "\n" );
		xray::debug::output					( final_string.c_str() );
		xray::buffer_string::iterator const end	= final_string.begin() + final_string.length();
		final_string.erase					( end - 1, end );
		final_string.append					(end_line, end_line+end_line_length);
		xray::logging::log_thread_unsafe	(m_log_flags, final_string.c_str(), final_string.length(), m_verbosity);

		if ( s_log_callback ) {
			s_log_callback		(
				m_file,
				m_function_signature,
				m_line,
				m_verbosity, 
				string,
				!index ? xray::logging::first : ( is_last ? xray::logging::last : xray::logging::callback_flag( 0 ) )
			);
		}

		return								( true );
	}
}; // struct predicate

void	xray::logging::log_format::set	(xray::logging::format_specifier const & format_expression)
{
	format_specifier_list					specifiers;
	format_expression.fill_specifier_list	(specifiers, & string);
	for ( u32 i=0; i<format_specifier_count; ++i )
	{
		indexes[ i ]					=	0;
		enabled[ i ]					=	false;
	}

	for ( u32 i=0; i<specifiers.size(); ++i )
	{
		indexes[ i ]					=	(int)specifiers[i];
		enabled[ specifiers[i] ]		=	true;
	}
}

static void process				(
		helper_data &									helper,
		xray::logging::format_specifier const *			log_format_specifier, 
		xray::logging::log_format const *				log_format, 
		xray::logging::log_flags_enum const				log_flags,
		pcstr const										format, 
		va_list const									args
	)
{
	using namespace xray::logging;

	debug_log_disable_raii					debug_log_disable;

	path_parts								path(helper.m_initiator);
	
	xray::logging::log_format				local_format;
	if ( log_format_specifier )
		local_format.set					(* log_format_specifier);

	xray::logging::log_format const * format_to_use	=	log_format_specifier ? & local_format : & globals->format;
	if ( log_format )
		format_to_use					=	log_format;

	string4096								message_buffer;
	xray::vsnprintf							( message_buffer, sizeof( message_buffer ) - 1, format, args );

	xray::strings::iterate_items	(
		message_buffer,
		predicate (
			helper.m_file,
			helper.m_function_signature,
			helper.m_line,
			helper.m_verbosity,
			log_flags,
			path,
			* format_to_use
		),
		'\n'
	);
}

void helper::operator ( )	( pcstr const format, ... )
{
	va_list	 				mark;
	va_start 				( mark, format );
	process	 				( *this, NULL, NULL, (log_flags_enum)0, format, mark );
	va_end	 				( mark );
}

void helper::operator ( )	( xray::logging::log_flags_enum const log_flags, pcstr const format, ... )
{
	va_list	 				mark;
	va_start 				( mark, format );
	process	 				( *this, NULL, NULL, log_flags, format, mark );
	va_end	 				( mark );
}

void helper::operator ( )	( xray::logging::format_specifier const & log_format, pcstr format, ... )
{
	va_list	 				mark;
	va_start 				( mark, format );
	process	 				( *this, & log_format, NULL, (xray::logging::log_flags_enum)0, format, mark );
	va_end	 				( mark );
}

void helper::operator ( )	( xray::logging::format_specifier const &	log_format, 
							  int const	log_flags, 
							  pcstr format, 
							  ... )
{
	va_list	 				mark;
	va_start 				( mark, format );
	process	 				( *this, & log_format, NULL, (xray::logging::log_flags_enum)log_flags, format, mark );
	va_end	 				( mark );
}

void helper::operator( )	( xray::logging::log_format * const format_struct, int const log_flags, pcstr format, ... )
{
	va_list	 				mark;
	va_start 				( mark, format );
	process	 				( *this, NULL, format_struct, (xray::logging::log_flags_enum)log_flags, format, mark );
	va_end	 				( mark );
}

bool xray::logging::check_verbosity (pcstr initiator, verbosity verbosity)
{
	debug_log_disable_raii					debug_log_disable;

	path_parts								path(initiator);
	
	int const allowed_verbosity			=	get_tree_verbosity( &path );
	if ( verbosity > allowed_verbosity )
		return								false;

	return									true;
}
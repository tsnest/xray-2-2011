////////////////////////////////////////////////////////////////////////////
//	Created 	: 29.08.2008
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "logging.h"
#include "logging_rule_tree_node.h"
#include <xray/logging_log_file.h>
#include "testing_impl.h"
#include "core_entry_point.h"
#include "testing_impl.h"
#include <xray/logging_settings.h>
#include <xray/fs_path.h>
#include <xray/fs_utils.h>

static xray::command_line::key	s_use_console("console", "", "logging", "turns on console output");
static xray::command_line::key	s_log_to_stdout("log_to_stdout", "", "logging", "turns on writing to stdout");

xray::memory::doug_lea_allocator_type xray::logging::g_allocator;

typedef std::pair<char*, int>					initiator_rule;
typedef xray::logging::vector<initiator_rule>	rules_stack;

using xray::logging::rule_tree::node;
using xray::logging::path_parts;
using xray::logging::log_file;
using xray::threading::mutex;

static node*		s_initiator_tree		= 0;
static rules_stack*	s_rules_stack			= 0;
static log_file*	s_log_file				= 0;
static u32			s_ready_for_use_tls_key	= xray::threading::tls_get_invalid_key( );

static xray::uninitialized_reference<mutex>	s_log_mutex;

namespace xray {
namespace logging {

mutex&	get_log_mutex							( )
{
	return					*s_log_mutex;
}

void	try_lock_log_mutex						( )
{
	if ( s_log_mutex.initialized() )
		s_log_mutex->lock	( );
}

void	try_unlock_log_mutex					( )
{
	if ( s_log_mutex.initialized() )
		s_log_mutex->unlock	( );
}

} // namespace logging
} // namespace xray

static bool get_ready_for_use					( )
{
	if ( !xray::threading::tls_is_valid_key( s_ready_for_use_tls_key ) )
		return					false;

	pvoid const value			= xray::threading::tls_get_value( s_ready_for_use_tls_key );
	return						!!*xray::pointer_cast< u32 const* >( &value );
}

static void set_ready_for_use					( bool const value )
{
	if ( !xray::threading::tls_is_valid_key( s_ready_for_use_tls_key ) )
		return;

	u32 const new_value			= value ? 1 : 0;
	xray::threading::tls_set_value( s_ready_for_use_tls_key, *xray::pointer_cast< pvoid const* >(&new_value) );
	R_ASSERT					( get_ready_for_use( ) == value );
}

static inline void check_not_ready_for_use		( )
{
#ifdef DEBUG
	bool const backup		= get_ready_for_use( );
	set_ready_for_use		( false );
	ASSERT					( !backup );
	set_ready_for_use		( backup );
#endif // #ifdef DEBUG
}

struct ready_for_use_guard {
	inline	ready_for_use_guard					( )
	{
		ASSERT					( get_ready_for_use( )  );
		set_ready_for_use		( false );
	}

	inline	~ready_for_use_guard				( )
	{
		check_not_ready_for_use	( );
		set_ready_for_use		( true );
	}
}; // struct ready_for_use_guard

void xray::logging::preinitialize				( )
{
	s_ready_for_use_tls_key		= threading::tls_create_key();
	set_ready_for_use			( false );
}

void xray::logging::on_thread_spawn				( )
{
	set_ready_for_use			( true );
}

void   generate_log_file_name (xray::fs::path_string * out_result, pcstr extension)
{
	ASSERT						(extension);
	ASSERT						(out_result);
	* out_result				=  xray::core::user_data_directory();
	* out_result				+= xray::core::application_name();
	xray::fs::path_string user_name	=	xray::core::user_name();
	if ( user_name.length() )
		out_result->appendf		("_%s", user_name.c_str());

	out_result->appendf			(".%s", extension);
	* out_result				= xray::fs::convert_to_portable(out_result->c_str());
}

void xray::logging::initialize					( core::log_file_usage log_file_usage, int const rule4root )
{
	check_not_ready_for_use		( );

	XRAY_CONSTRUCT_REFERENCE	( s_log_mutex, mutex );

	ASSERT						( !s_log_file );
	if ( log_file_usage != core::no_log ) {
		fs::path_string			file_name;
		generate_log_file_name	( & file_name, "log" );
		s_log_file				= LOG_NEW( log_file ) ( log_file_usage, file_name.c_str() );
	}

	ASSERT						( !s_rules_stack );
	s_rules_stack				= LOG_NEW( rules_stack );

	ASSERT						( !s_initiator_tree );
	s_initiator_tree			= LOG_NEW( node ) ( rule4root );

	set_ready_for_use			( true );

	push_rule					( "", rule4root );
}

void xray::logging::finalize				( )
{
	while ( !s_rules_stack->empty() )
		pop_rule				( );

	set_ready_for_use			( false );

	LOG_DELETE					( s_initiator_tree );
	LOG_DELETE					( s_rules_stack );
	LOG_DELETE					( s_log_file );
	XRAY_DESTROY_REFERENCE		( s_log_mutex );

	threading::tls_delete_key	( s_ready_for_use_tls_key );
	
	finalize_console			( );
}

xray::logging::verbosity xray::logging::string_to_verbosity ( pcstr in_verbosity )
{
	logging::verbosity	verbosities[]	= {	silent, error, warning,	info, debug, trace };
	for ( u32 i=0; i<array_size(verbosities); ++i )
		if ( strings::equal(verbosity_to_string(verbosities[i]), in_verbosity) )
			return						verbosities[i];
	
	return						unset;
}

pcstr xray::logging::verbosity_to_string	( int const verbosity )
{
	switch ( verbosity )
	{
		case silent	 :			return ( "silent" );
		case info    :			return ( "info" );
		case warning :			return ( "warning" );
		case error   :			return ( "error" );
		case debug	 :			return ( "debug" );
		case trace   :			return ( "trace" );
		default		 :			NODEFAULT(return 0);
	}
}

////////////////////////////////////////////////////////////////////////////
// rules
////////////////////////////////////////////////////////////////////////////

static void build_tree							( )
{
	s_initiator_tree->clean		( );

	rules_stack::iterator		i = s_rules_stack->begin( );
	rules_stack::iterator		e = s_rules_stack->end( );
	for ( ; i != e; ++i )
		s_initiator_tree->set	( ( *i ).first, ( *i ).second );
}

void xray::logging::push_rule			( pcstr initiator, int const verbosity )
{
	ready_for_use_guard			guard;

	ASSERT						( s_rules_stack );
	if ( !initiator )
		initiator				= "";

	initiator_rule				rule;
	int const initiator_len		= strings::length( initiator );

	get_log_mutex().lock		( );
	g_allocator.user_current_thread_id( );
	rule.first					= LOG_ALLOC( char, initiator_len + 1 );
	get_log_mutex().unlock		( );

	strings::copy					( rule.first, initiator_len + 1, initiator );

	rule.second					= verbosity;

	get_log_mutex().lock		( );

	s_rules_stack->push_back	( rule );	
	build_tree					();

	get_log_mutex().unlock		( );
}

void xray::logging::pop_rule				( )
{
	threading::mutex_raii raii	( get_log_mutex() );
	if ( s_rules_stack->empty( ) ) {
		return;
	}

	ready_for_use_guard			guard;

	initiator_rule rule			= s_rules_stack->back( );
	s_rules_stack->pop_back		( );

	g_allocator.user_current_thread_id( );
	LOG_FREE					( rule.first );

	build_tree					( );
}


namespace xray {
namespace logging {

int get_tree_verbosity							( path_parts* const path )
{
	ASSERT						( s_initiator_tree );
	return						( s_initiator_tree->get_verbosity( path ) );
}

bool use_console_for_logging ( )
{
	static bool s_use_console_for_logging	=	testing::run_tests_command_line() || s_use_console;
	return						s_use_console_for_logging;
}

void log_thread_unsafe							( logging::settings const * settings, pcstr message, u32 const string_length, logging::verbosity verbosity )
{
	check_not_ready_for_use		( );
	
	if ( s_log_file && s_log_file->initialized() )
		s_log_file->append		( message, string_length );

	bool const log_to_console_settings	=	settings && (settings->flags & logging::settings::flags_log_to_console);
	bool const use_console_for_testing	=	use_console_for_logging() && verbosity <= warning;

	bool logged_to_stdout				=	false;

	if ( log_to_console_settings || use_console_for_testing )
	{
		static bool s_initialized_console			=	false;
		static bool s_tried_to_initialize_console	=	false;
		if ( !s_tried_to_initialize_console )
		{
			s_initialized_console					=	initialize_console		();
			s_tried_to_initialize_console			=	true;
		}

		if ( s_initialized_console )
		{
			write_to_stdout								( "%s", message );
			logged_to_stdout						=	true;
		}
	}

	if ( s_log_to_stdout && !logged_to_stdout )
	{
		initialize_stdout_if_needed						();
		write_to_stdout									( "%s", message );
		logged_to_stdout							=	true;
	}
}

} // namespace logging
} // namespace xray

bool xray::logging::ready_for_use		( )
{
	return						s_initiator_tree && ( get_ready_for_use( ) );
}

void xray::logging::flush				( pcstr file_name )
{
	if ( !s_log_file || !get_ready_for_use( ) )
		return;

	if ( file_name )
		fs::make_dir_r			( file_name, false );

	threading::mutex_raii raii	( get_log_mutex() );

	ASSERT						( get_ready_for_use( ) );
	set_ready_for_use			( false );

	if ( s_log_file->initialized() )
		s_log_file->flush		( file_name );

	set_ready_for_use			( true );
}

void xray::logging::ready_for_use		( bool const value )
{
	bool const backup			= get_ready_for_use( );
	
	set_ready_for_use			( false );
	ASSERT_U					( !backup || ( backup != value ), "previous[%s] current[%s]", backup ? "true" : "false", value ? "true" : "false");
	set_ready_for_use			( value );
}

pcstr xray::logging::log_file_name	( )
{
	if ( !get_ready_for_use( ) )
		return					( "" );

	ASSERT						( get_ready_for_use( ) );
	set_ready_for_use			( false );

	ASSERT						( s_log_file );

	set_ready_for_use			( true );

	return						( s_log_file->file_name( ) );
}

xray::logging::log_file* xray::logging::get_log_file( )
{
	return						( s_log_file );
}

void   xray::logging::write_exit_code_file ( int exit_code )
{
	if ( !get_log_file() )
		return;

	fs::path_string				file_name;
	generate_log_file_name		(& file_name, "exit_code");

	FILE * file				=	NULL;
	if ( fs::open_file(& file, fs::open_file_create | fs::open_file_truncate | fs::open_file_write, file_name.c_str()) )
	{
		fprintf						(file, "%d", exit_code);
		fclose						(file);
	}
}
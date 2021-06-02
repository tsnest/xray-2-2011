////////////////////////////////////////////////////////////////////////////
//	Created 	: 07.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/command_line_extensions.h>
#include <xray/intrusive_list.h>
#include <xray/construction.h>
#include "build_extensions.h"
#include <xray/core_entry_point.h>
#include <xray/core/engine.h>
#include "build_extensions.h"

namespace xray {
namespace debug {
	void			preinitialize		( );
} // namespace debug

namespace command_line {

	typedef intrusive_list<key, key *, & key::m_next_key, threading::mutex_tasks_unaware>	command_line_keys_list;
static char					s_command_line_keys_buffer			[sizeof(command_line_keys_list)];
static command_line_keys_list* s_command_line_keys			=	NULL;
static threading::atomic32_type	s_command_line_keys_creation	=	0;
static bool					s_command_line_initialized		=	false;
static u32					s_command_line_keys_count		=	0;
static bool					s_command_line_ready			=	false;
static core::engine *		s_engine						=	NULL;
fixed_string4096			g_command_line;
command_line::key			s_show_help							("help", "h", "", "shows this help");
fixed_string4096			s_command_line_error;

static string512			s_finger_print	=	"";

static
bool   is_delimiter	(char const value, pcstr delimiters)
{
	return					!!strchr(delimiters, value);
}

static
bool   is_whitespace (char const value) { return is_delimiter(value, " \t"); }

void   protected_key_construct (pvoid keyptr_)
{
	key * const keyptr	=	(key *)keyptr_;
	keyptr->protected_construct ();
}

key::key (pcstr full_name, pcstr short_name, pcstr category, pcstr description, pcstr argument_description) : 
																	m_full_name(full_name), 
																	m_short_name(short_name), 
																	m_category(category),
																	m_type(type_uninitialized), 
																	m_number_value(0.f), 
																	m_description(description),
																	m_argument_description(argument_description)
{
#ifndef XRAY_STATIC_LIBRARIES
	debug::preinitialize						( );
	logging::preinitialize						( );
#endif // #ifndef XRAY_STATIC_LIBRARIES
	debug::protected_call						( protected_key_construct, this );
}

void   key::protected_construct ()
{
	R_ASSERT									(m_full_name);
	R_ASSERT									(m_short_name);

	pcstr i	= NULL;
	for ( i = m_short_name; * i; ++i )
		if ( is_delimiter(* i, " \t=-") )
			break;

#ifndef MASTER_GOLD
	pcstr const wrong_key_assert_message	=	"command_line_key with delimiter characters specified (they are not supported): \"%s\"";
	pcstr const empty_key_assert_message	=	"command_line_key with empty shortname specified";
#endif // #ifndef MASTER_GOLD

	R_ASSERT	(!* i, wrong_key_assert_message, m_short_name);

	for ( i = m_full_name; * i; ++i )
		if ( is_delimiter(* i, " \t=-") )
			break;

	R_ASSERT	(!* i, wrong_key_assert_message, m_full_name);
	R_ASSERT	(strings::length(m_short_name) + strings::length(m_full_name) > 0, empty_key_assert_message);

	if ( !s_command_line_keys )
	{
		bind_pointer_to_buffer_mt_safe	(	s_command_line_keys, 
											s_command_line_keys_buffer, 
											s_command_line_keys_creation,
											bind_pointer_to_buffer_mt_safe_placement_new_predicate() );
	}

	s_command_line_keys->push_back			(this);
	++s_command_line_keys_count;
}

bool   key::is_set () 
{
	if ( m_type == type_uninitialized )
		initialize				();

	return						m_type != type_unset;
}

bool   key::is_set_as_string			(buffer_string * out_value)
{
	if ( m_type == type_uninitialized )
		initialize							();

	if ( m_type == type_unset )
		return								false;

	if ( out_value )
		* out_value						=	m_string_value;
	return									true;
}

bool   key::is_set_as_number			(float * out_value)
{
	if ( m_type == type_uninitialized )
		initialize				();

	if ( m_type == type_unset )
		return					false;

	CURE_ASSERT					(m_type == type_number, return 0.f, "given value is not convertible to number: %s", m_string_value.c_str());

	if ( out_value )
		* out_value			=	m_number_value;
	return						true;
}

void   key::initialize (pcstr value)
{
	if ( !value || ! * value )
	{
		m_type				=	type_void;
		return;
	}

	m_string_value			=	value;
	if ( strings::convert_string_to_number(value, & m_number_value) )
		m_type				=	type_number;
	else
		m_type				=	type_string;
}

struct command_line_key_finder
{
	command_line_key_finder (pcstr key_name) : key_name(key_name), result(NULL) {}
	void operator () (key * const command_line_key)
	{
		if ( command_line_key->full_name() == key_name || command_line_key->short_name() == key_name )
			result					=	command_line_key;
	}

	fixed_string512						key_name;
	key *			result;
};

static
key *   find_key	(pcstr key_name)
{
	command_line_key_finder					finder(key_name);
	s_command_line_keys->for_each			(finder);
	return									finder.result;
}

template <class Predicate>
void   iterate_keys (Predicate predicate)
{
	CURE_ASSERT								(s_command_line_ready, return, "please run initialize first");
	pcstr command_line					=	g_command_line.c_str();
	for ( pcstr current = command_line; * current; ++ current )
	{
		if ( is_whitespace(* current) )
			continue;

		if ( * current != '-' )
		{
			if ( !s_command_line_error.length() )
				s_command_line_error.assignf	("expected '-' symbol and not %c at %s(%d)", 
												 * current, command_line, current - command_line);
			continue;
		}

		++ current;

		if ( ! * current )
		{
			if ( !s_command_line_error.length() )
				s_command_line_error.assignf	("last command line key is empty: %s", command_line);
			break;
		}

//		bool fullname_specified			=	false;
		if ( * current == '-' )
		{
//			fullname_specified			=	true;
			++ current;

			if ( ! * current )
			{
				if ( !s_command_line_error.length() )
					s_command_line_error.assignf	("last command line key is empty: %s", command_line);
				break;
			}
		}

		pcstr	key_name_start			=	current;
		for ( ; * current; ++ current )
		{
			if ( is_whitespace(* current) || * current == '=' )
				break;
		}

		fixed_string512		key_name		(key_name_start, current);

		if ( * current != '=' )
		{
			predicate						(find_key(key_name.c_str()), key_name.c_str(), NULL);
			continue;
		}
		++ current;

		if ( ! * current )
		{
			if ( !s_command_line_error.length() )
				s_command_line_error.assignf	("key value is empty: %s", key_name_start);
			break;
		}

		pcstr key_value_start			=	current;
		bool search_quote				=	false;
		if ( * current == '"' )
		{
			search_quote				=	true;				
			++ current;
			key_value_start				=	current;
		}

		for ( ; * current; ++ current )
		{
			if ( search_quote )
			{
				if ( * current == '"' && * (current - 1) != '\\' )
					break;
			}
			else if ( is_whitespace(* current) )
				break;
		}

		fixed_string512	key_value			(key_value_start, current);
		if ( search_quote )
			++ current;

		predicate							(find_key(key_name.c_str()), key_name.c_str(), key_value.c_str());
	}
}

struct initializer
{
	void operator () (key * const command_line_key, pcstr key_name, pcstr key_value)
	{
		XRAY_UNREFERENCED_PARAMETER		(key_name);
		if ( command_line_key )
			command_line_key->initialize(key_value);
	}
};

void   initialize_keys ()
{
	iterate_keys							(initializer());
}

struct key_initializator
{
	void operator () (key * const command_line_key, pcstr key_name, pcstr key_value)
	{
		if ( !command_line_key )
			return;

		if ( strings::equal(command_line_key->full_name(), key_name) ||
			 strings::equal(command_line_key->short_name(), key_name) )
		{
			command_line_key->initialize	(key_value);
		}
	}
};

void   key::initialize ()
{
	m_type								=	type_unset;

	iterate_keys							(key_initializator());
}

struct checker
{
	void operator () (key * const command_line_key, pcstr key_name, pcstr key_value)
	{
		XRAY_UNREFERENCED_PARAMETER		(key_value);

		if ( !command_line_key && !(key_name && * key_name == '.') )
		{
			LOG_INFO					(logging::format_message, logging::log_to_console,
										"\nkey with name '%s' is not registered, use -help to see list of available commands", key_name);

			if ( debug::is_debugger_present() )
				DEBUG_BREAK				();

			if ( s_engine )
				s_engine->exit			(0);
		}
	}
};

void   check_keys ()
{
	if ( s_command_line_error.length() )
	{
		LOG_INFO							(logging::format_message, logging::log_to_console,
											 "%s", s_command_line_error.c_str());
		if ( debug::is_debugger_present() )
			DEBUG_BREAK						();

		if ( s_engine )
			s_engine->exit					(0);
	}

	iterate_keys							(checker());
}

bool   initialized ()
{
	return							s_command_line_initialized;
}

void   initialize (core::engine * engine, pcstr const command_line, command_line::contains_application_bool const contains_application)
{
	s_engine			=	engine;
	typedef	xray::fixed_string4096	string_type;
	string_type	string	=	command_line;
	//string.rtrim			(' ');
	//if ( string.length() >= 2 && string[0] == '"' && string.last() == '"' )
	//	string					=	string.substr(1, string.length()-2);

	if ( contains_application == command_line::contains_application_true )
	{
		R_ASSERT					(string.length() > 0, "empty command line specified together with contains_application = true");
		if ( string[0] == '"' )
		{
			u32 const index		=	string.find('"', 1);
			R_ASSERT				(index != string.npos, "command line passed with quote that is not enclosed");
			string				=	string.substr(index + 1, string.npos);
		}
		else
		{
			u32 index		=	0;
			for ( u32 n = string.length(); index < n; ++ index )
				if ( string[index] == ' ' || string[index] == '\t' )
					break;

			string				=	string.substr(index, string.npos);
		}
	}

	s_command_line_ready		=	true;
	g_command_line				=	string;
	initialize_keys					();
	s_command_line_initialized	=	true;
}

void   handle_help_key ( )
{
	if ( s_show_help )
		show_help_and_exit		( );
}

typedef buffer_vector<key *>	keys_array;

struct command_line_key_adder
{
	command_line_key_adder (keys_array * keys) : keys_(keys), longest_short_key_name(0), longest_full_key_name(0) {}
	command_line_key_adder () {}
	void operator () (key * const command_line_key)
	{
		longest_short_key_name	=	math::max(longest_short_key_name, strings::length(command_line_key->short_name()));
		longest_full_key_name	=	math::max(longest_full_key_name, strings::length(command_line_key->full_name()));
		keys_->push_back			(command_line_key);
	}

	keys_array *					keys_;
	u32								longest_short_key_name;
	u32								longest_full_key_name;
};

struct key_compare_predicate
{
	bool operator () (key const * const left, key const * const right)
	{
		int const category_compare	=	strings::compare(left->category(), right->category());
		if ( category_compare == 0 )
		{
			pcstr const left_name	=	* left->short_name() ? left->short_name() : left->full_name();
			pcstr const right_name	=	* right->short_name() ? right->short_name() : right->full_name();
			return						strings::compare(left_name, right_name) < 0;
		}

		return							category_compare < 0;
	}
};

void   show_help_and_exit ( )
{
	keys_array						keys(ALLOCA(s_command_line_keys_count * sizeof(key *)), s_command_line_keys_count);

	command_line_key_adder			adder_predicate(& keys);
	s_command_line_keys->for_each	(adder_predicate);

	std::sort							(keys.begin(), keys.end(), key_compare_predicate());

	fixed_string512						format_string;
	format_string.assignf				("    %%-%ds  %%s %%s", 
										 adder_predicate.longest_short_key_name + 
										 adder_predicate.longest_full_key_name + 5);

	LOG_INFO							(logging::format_message, logging::log_to_console,
										"               " XRAY_ENGINE_ID ", build %d, %s\n"
										"                  Copyright(C) GSC Game World - 2009\n"
										"      Finger print info: %s",
										build::calculate_build_id(build::build_date()), build::build_date(),s_finger_print);

	pcstr previous_category			=	"";

	for ( keys_array::const_iterator	it	=	keys.begin(); 
										it	!=	keys.end();
										++it )
	{
		key const * const key		=	* it;
		if ( it == keys.begin() || !strings::equal(key->category(), previous_category) )
		{
			fixed_string512	category	=	* key->category() ? key->category() : "global";
			fixed_string512	first_char;
			category.substr				(0, 1, & first_char);
			first_char.make_uppercase	();
			category[0]				=	first_char[0];

			LOG_INFO					(logging::format_message, logging::log_to_console,
									     "\n%s options: ", category.c_str() );
			previous_category		=	key->category();
		}

		fixed_string512 key_name;
		if ( * key->short_name() && * key->full_name() )
			key_name.assignf			("-%s [-%s]", key->full_name(), key->short_name());
		else 
			key_name.assignf			("-%s", * key->short_name() ? key->short_name() : key->full_name());

		if ( * key->argument_description() )
			key_name.appendf			(" = <%s>", key->argument_description());

		LOG_INFO						(logging::format_message, logging::log_to_console,
										 format_string.c_str(), 
										 key_name.c_str(), 
										 * key->description() ? ":" : "", 
										 key->description());
	}

	if ( s_engine )
		s_engine->exit					(0);
	else
		debug::terminate				("");
}

bool	show_help	( )
{
	return								s_show_help;
}

void	set_finger_print ( pcstr finger_print )
{
	strings::copy			(s_finger_print, finger_print);
}

static bool is_whitespace_or_assign_symbol	( char const value )
{
	return					( is_delimiter( value, " \t=" ) );
}

bool key_is_set_impl		( pcstr command_line, pcstr key_raw )
{
	if ( !*key_raw ) {
		LOGI_WARNING		( "command_line_key", "empty key specified to ");
		return				( false );
	}

#ifdef DEBUG
	for ( pcstr i = key_raw; *i; ++i )
		if ( is_whitespace_or_assign_symbol( *i ) ) {
			LOGI_WARNING	( "command_line_key", "key with delimietr characters specified (they are not supported): \"%s\"", key_raw );
			return			( false );
		}
#endif // #ifdef DEBUG

	pstr					key = 0;
	STR_JOINA				( key, "-", key_raw );

	for ( pcstr i = command_line; *i; ++i ) {
		pcstr j				= i;
		pcstr k				= key;
		for ( ; *j && *k && ( *j == *k ); ++j, ++k);

		if ( !*k )
			return			( !*j || is_whitespace_or_assign_symbol( *j ) );
	}

	return					( false );
}

bool key_is_set				( pcstr key_raw )
{
	return					( key_is_set_impl( core::get_command_line(), key_raw ) );
}

bool key_is_set_impl		( pcstr const command_line, pcstr const key_raw, pstr const buffer, u32 const buffer_size )
{	
	ASSERT					( buffer );
	ASSERT					( buffer_size );

	*buffer					= 0;

	if ( !*key_raw ) {
		LOGI_WARNING		( "command_line_key", "empty key specified to ");
		return				( false );
	}

	for ( pcstr i = key_raw; *i; ++i )
		if ( is_whitespace_or_assign_symbol( *i ) ) {
			LOGI_WARNING	( "command_line_key", "key with delimiter characters specified (they are not supported): \"%s\"", key_raw );
			return			( false );
		}

	pstr					key = 0;
	STR_JOINA				( key, "-", key_raw );

	for ( pcstr i = command_line; *i; ++i ) {
		pcstr j				= i;
		pcstr k				= key;
		for ( ; *j && *k && ( *j == *k ); ++j, ++k);

		if ( !*j )
			return			( !*k );

		if ( *k )
			continue;

		if ( *j != '=' )
			continue;

		++j;
		pcstr m				= j;
		for ( ; *m && !is_delimiter( *m, " \t" ); ++m );

		u32 const count		= u32( m - j );
		if ( buffer_size < count + 1 ) {
			LOGI_WARNING	( "command_line_key", "not enough storage for value of the key \"%s\" specified", key );
			return			( false );
		}

		memory::copy		( buffer, buffer_size, j, count*sizeof( char ) );
		buffer[count]		= 0;
		return				( true );
	}

	return					( false );
}

bool key_is_set				( pcstr const key_raw, pstr const buffer, u32 const buffer_size )
{
	return					( key_is_set_impl( core::get_command_line(), key_raw, buffer, buffer_size ) );
}

} // namespace command_line

namespace build {

XRAY_CORE_API	u32	build_station_build_id	( );
	
u32	build_station_build_id	( )
{
	static bool initialized				= false;
	static u32  build_station_build_id	= u32(-1);
	if ( initialized )
		return							build_station_build_id;

	initialized							= true;
	if ( XRAY_SSCANF(xray::command_line::s_finger_print, "Build %d", &build_station_build_id) != 1 ) {
		build_station_build_id			= xray::build::calculate_build_id( xray::build::build_date() );
	}

	return								build_station_build_id;
}

} // namespace build

} // namespace xray
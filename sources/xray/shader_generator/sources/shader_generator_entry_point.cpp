////////////////////////////////////////////////////////////////////////////
//	Created		: 18.09.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <boost/regex.hpp>
#include <boost/noncopyable.hpp>
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>

static char const* const defined_string_constant = "defined";

template <typename Predicate>
static bool search_patterns	( char const* begin, char const* const end, boost::regex const& expression, Predicate const& predicate )
{
	boost::match_results<char const*>	what;
	boost::match_flag_type flags		= boost::match_default; 

	bool result							= false;
	while ( boost::regex_search(begin, end, what, expression) ) {
		result							= true;
		predicate						( what );
		begin							= what[0].second; 
		flags							|= boost::match_prev_avail; 
		flags							|= boost::match_not_bob;
	}

	return								result;
}

template < int index >
class definition_parser : private boost::noncopyable {
public:
	inline		definition_parser	( unique_definitions_type& definitions ) :
		m_definitions			( definitions )
	{
	}

	inline void	operator( )			( boost::match_results<char const*> const& what ) const
	{
		std::string				result(std::string(what[index].first, what[index].second));
		m_definitions.insert	( result );
	}

private:
	unique_definitions_type&	m_definitions;
}; //struct definition_parser

template < int index0, int index1 >
class definition_recursive_parser : private boost::noncopyable {
public:
	inline		definition_recursive_parser	( unique_definitions_type& definitions ) :
		m_definitions			( definitions )
	{
	}
	
	inline void	operator( )					( boost::match_results<char const*> const& what ) const
	{
		char const* i			= what[index0].first;
		char const* const e		= what[index0].second;
		char const* k			= e;
		for (bool found = false; i != e; ++i ) {
			if ( *i == '/' ) {
				if ( found ) {
					k			= i;
					break;
				}
				found			= true;
				continue;
			}
			else
				found			= false;
		}

		static boost::regex const identifier(
			"[a-zA-Z][a-zA-Z0-9_$]*"
		);
		search_patterns			( what[index0].first, k, identifier, definition_parser<index1>(m_definitions) );
	}

private:
	unique_definitions_type&	m_definitions;
}; // class definition_recursive_parser

static void process_includes(
		char const* shader_path,
		char const* const buffer,
		unsigned int const buffer_size,
		total_info_type& total_info,
		include_ids_type& include_ids
	);

static void parse_shader	(
		char const* base_path,
		char const* file_name,
		char const* const buffer,
		unsigned int const buffer_size,
		total_info_type& total_info
	)
{
	string_path	drive, directory, name, extension;
	errno_t error					= _splitpath_s( file_name, drive, directory, name, extension );
	if ( error ) {
		printf						( "ERROR: cannot split path \"%s\"\n", file_name );
		return;
	}

	error							= strcat_s( drive, directory );
	if ( error ) {
		printf						( "ERROR: cannot concatenate strings \"%s\" and \"%s\"\n", drive, directory );
		return;
	}

	if ( drive[strlen(drive)-1] != '\\' ) {
		error						= strcat_s( drive, "\\" );
		if ( error ) {
			printf					( "ERROR: cannot concatenate strings \"%s\" and \"%s\"\n", drive, directory );
			return;
		}
	}

	file_info_type& file_info		= total_info[file_name];
	file_info.body.assign			( buffer, buffer + buffer_size );

	process_includes				( base_path, buffer, buffer_size, total_info, file_info.include_ids );

	// !because defines could under ifdef!
	//static boost::regex const preprocessor_define(
	//	"(^[ \\t]*#[ \\t]*define[ \\t]+)([a-zA-Z_$][a-zA-Z0-9_$]*)"
	//);
	//search_patterns					( buffer, buffer + buffer_size, preprocessor_define,		definition_parser<2>(file_info.internal_definitions) );

	static boost::regex const preprocessor_ifdef_ifndef(
		"(^[ \\t]*#[ \\t]*ifn{0,1}def[ \\t]+)([a-zA-Z_$][a-zA-Z0-9_$]*)"
	);
	search_patterns					( buffer, buffer + buffer_size, preprocessor_ifdef_ifndef,	definition_parser<2>(file_info.external_definitions) );

	static boost::regex const preprocessor_if(
		"(^[ \\t]*#[ \\t]*if[ \\t]+)([a-zA-Z0-9_$\\t !|\\&\\+\\-\\*\\/\\(\\)\\>\\<\\=]*$)"
	);
	search_patterns					( buffer, buffer + buffer_size, preprocessor_if,			definition_recursive_parser<2,0>(file_info.external_definitions) );

	// remove keyword "defined", which could be presented in the #if conditional clauses
	file_info.internal_definitions.insert	( defined_string_constant );

	definitions_type& personal		= file_info.personal_unique_definitions;
	personal.resize					( file_info.external_definitions.size() );
	personal.erase					(
		std::set_difference			(
			file_info.external_definitions.begin(),
			file_info.external_definitions.end(),
			file_info.internal_definitions.begin(),
			file_info.internal_definitions.end(),
			personal.begin()
		),
		personal.end()
	);

	include_ids_type::const_iterator i			= file_info.include_ids.begin();
	include_ids_type::const_iterator const e	= file_info.include_ids.end();
	for ( ; i != e; ++i ) {
		file_info_type& include_info = total_info[*i];
		file_info.external_definitions.insert	( include_info.unique_definitions.begin(), include_info.unique_definitions.end() );
	}

	definitions_type& result		= file_info.unique_definitions;
	result.resize					( file_info.external_definitions.size() );
	result.erase					(
		std::set_difference			(
			file_info.external_definitions.begin(),
			file_info.external_definitions.end(),
			file_info.internal_definitions.begin(),
			file_info.internal_definitions.end(),
			result.begin()
		),
		result.end()
	);

	file_info.internal_definitions.erase	( file_info.internal_definitions.find(defined_string_constant) );
}

static void process_shader	( char const* const base_path, char const* const file_name, total_info_type& total_info )
{
	if ( total_info.find(file_name) != total_info.end() )
		return;

	int	const file_open_flags		=	_O_BINARY | _O_SEQUENTIAL | _O_RDONLY;
	int const permission_flags		=	_S_IREAD;
	int const sh_permission_flags	=	_SH_DENYWR;

	int	file_handle					=	0;
	errno_t error_code				=	_sopen_s(& file_handle, file_name, file_open_flags, 
											 sh_permission_flags, permission_flags);

	if ( error_code ) 
	{
		Sleep						(1);
		error_code					=	_sopen_s( &file_handle, file_name, file_open_flags,		
											 sh_permission_flags, permission_flags);
		if ( error_code ) {
			printf					( "ERROR: cannot open file \"%s\"\n", file_name );
			return;
		}
	}

	FILE* const file				=	_fdopen(file_handle, "rb");
	unsigned int const file_size	= 	_filelength ( _fileno(file) );
	bool use_stack					=	file_size < 768*1024;
	char* buffer					=	0;
	if ( use_stack )
		buffer						= (char*)_alloca(file_size*sizeof(char));
	else
		buffer						= (char*)malloc(file_size*sizeof(char));

	size_t const read_bytes			= fread( buffer, 1, file_size, file );
	fclose							( file );

	if ( read_bytes != file_size ) {
		printf						( "ERROR: cannot read from file \"%s\"\n", file_name );
		if ( !use_stack )
			free					( buffer );

		return;
	}

	parse_shader					( base_path, file_name, buffer, file_size, total_info );

	if ( !use_stack )
		free						( buffer );
}

class include_parser : private boost::noncopyable {
public:
	inline		include_parser		( char const* const path, total_info_type& total_info, include_ids_type& include_ids ) :
		m_path					( path ),
		m_total_info			( total_info ),
		m_include_ids			( include_ids )
	{
	}

	inline void	operator( )			( boost::match_results<char const*> const& what ) const
	{
		std::string				result( m_path + std::string(what[2].first, what[2].second) );
		m_include_ids.insert	( result );
		process_shader			( m_path, result.c_str(), m_total_info );
	}

private:
	total_info_type&			m_total_info;
	include_ids_type&			m_include_ids;
	char const* const			m_path;
}; //struct include_parser

static void process_includes(
		char const* shader_path,
		char const* const buffer,
		unsigned int const buffer_size,
		total_info_type& total_info,
		include_ids_type& include_ids
	)
{
	static boost::regex const preprocessor_include(
		"(^[ \\t]*#[ \\t]*include[ \\t]+\\\")([^\\*\\?\r\n]+)(\\\")"
	);
	search_patterns				( buffer, buffer + buffer_size, preprocessor_include, include_parser(shader_path,total_info,include_ids) );
}

static void process_shaders	( total_info_type& total_info, char const* const base_path, char const* const full_path, char const* const search_mask )
{
	_finddata32i64_t		file_desc;
	string_path				file_spec;
	strcpy_s				( file_spec, full_path );
	strcat_s				( file_spec, search_mask );
	long handle				= (long)_findfirst32i64( file_spec, &file_desc );

	if ( handle	==	-1 ) {
		strcpy_s			( file_spec, full_path );
		strcat_s			( file_spec, "*.*" );
		long handle			= (long)_findfirst32i64( file_spec, &file_desc );
		do {
			if ( !strcmp(".", file_desc.name) )
				continue;

			if ( !strcmp("..", file_desc.name) )
				continue;

			if ( (file_desc.attrib & _A_SUBDIR) == _A_SUBDIR ) {
				if ( (file_desc.attrib & _A_HIDDEN) == _A_HIDDEN )
					continue;

				string_path		temp;
				strcpy_s		( temp, full_path );
				strcat_s		( temp, file_desc.name );
				strcat_s		( temp, "\\" );
				process_shaders	( total_info, base_path, temp, search_mask );
				continue;
			}

			continue;
		} while ( _findnext32i64 (handle, &file_desc) != -1 );

		return;
	}

	do {
		if ( !strcmp(".", file_desc.name) )
			continue;

		if ( !strcmp("..", file_desc.name) )
			continue;

		if ( (file_desc.attrib & _A_SUBDIR) == _A_SUBDIR ) {
			if ( (file_desc.attrib & _A_HIDDEN) == _A_HIDDEN )
				continue;

			string_path		temp;
			strcpy_s		( temp, full_path );
			strcat_s		( temp, file_desc.name );
			strcat_s		( temp, "\\" );
			process_shaders	( total_info, base_path, temp, search_mask );
			continue;
		}

		string_path			file_name;
		strcpy_s			( file_name, full_path );
		strcat_s			( file_name, file_desc.name );
		process_shader		( base_path, file_name, total_info );
	} while ( _findnext32i64 (handle, &file_desc) != -1 );
}

static void print_dependencies	( total_info_type& total_info, std::string const& file_name, std::string const& definition, size_t const full_path_length, include_ids_type& include_ids )
{
	{
		file_info_type& include_info = total_info[file_name];
		definitions_type::const_iterator found	= std::lower_bound( include_info.personal_unique_definitions.begin(), include_info.personal_unique_definitions.end(), definition );
		if ( (found != include_info.personal_unique_definitions.end()) && (*found == definition) )
			include_ids.insert	( file_name.c_str() + full_path_length );
	}

	file_info_type const& file_info				= total_info[file_name];
	include_ids_type::const_iterator i			= file_info.include_ids.begin();
	include_ids_type::const_iterator const e	= file_info.include_ids.end();
	for ( ; i != e; ++i ) {
		file_info_type& include_info = total_info[*i];
		definitions_type::const_iterator found = std::lower_bound( include_info.unique_definitions.begin(), include_info.unique_definitions.end(), definition );
		if ( found == include_info.unique_definitions.end() )
			continue;

		if ( *found != definition )
			continue;

		print_dependencies	( total_info, *i, definition, full_path_length, include_ids );
	}
}

static void print_dependencies	( total_info_type& total_info, std::string const& file_name, std::string const& definition, size_t const full_path_length )
{
	include_ids_type	include_ids;

	print_dependencies	( total_info, file_name, definition, full_path_length, include_ids );

	include_ids_type::const_iterator b			= include_ids.begin();
	include_ids_type::const_iterator const e	= include_ids.end();
	for (include_ids_type::const_iterator i = b; i != e; ++i ) {
		if ( i == b )
			printf		( " %s", (*i).c_str() );
		else
			printf		( ", %s", (*i).c_str() );
	}
}

//static void process_heuristics	( total_info_type& total_info )
//{
//	// heuristics:
//	// remove definitions being defined by some of the shaders
//	{
//		unique_definitions_type	total_internal;
//		total_internal.insert	( defined_string_constant );
//		{
//			total_info_type::const_iterator	i		= total_info.begin();
//			total_info_type::const_iterator	const e = total_info.end();
//			for ( ; i != e; ++i )
//				total_internal.insert	( (*i).second.internal_definitions.begin(), (*i).second.internal_definitions.end() );
//		}
//		{
//			total_info_type::iterator i			= total_info.begin();
//			total_info_type::iterator const e	= total_info.end();
//			for ( ; i != e; ++i ) {
//				(*i).second.unique_definitions.resize	( (*i).second.external_definitions.size() );
//				(*i).second.unique_definitions.erase	(
//					std::set_difference		(
//						(*i).second.external_definitions.begin(),
//						(*i).second.external_definitions.end(),
//						total_internal.begin(),
//						total_internal.end(),
//						(*i).second.unique_definitions.begin()
//					),
//					(*i).second.unique_definitions.end()
//				);
//			}
//		}
//	}
//}
//
static void print_results	( total_info_type& total_info, char const* const full_path )
{
	size_t const full_path_length	= strlen(full_path);
	unique_definitions_type	unique_definitions;

	printf					( "Definitions per file (%d):\n", total_info.size() );
	{
		total_info_type::const_iterator	i		= total_info.begin();
		total_info_type::const_iterator	const e = total_info.end();
		for (int count = 1; i != e; ++i, ++count ) {
			unique_definitions.insert			( (*i).second.unique_definitions.begin(), (*i).second.unique_definitions.end() );

			printf			( "%3d. %s (%d): \n", count, (*i).first.c_str(), (*i).second.unique_definitions.size() );

			definitions_type::const_iterator ii			= (*i).second.unique_definitions.begin();
			definitions_type::const_iterator const ee	= (*i).second.unique_definitions.end();
			for ( ; ii != ee; ++ii ) {
				printf				( "\t%-32s (", (*ii).c_str() );
				print_dependencies	( total_info, (*i).first, *ii, full_path_length );
				printf				( " )\n" );
			}
		}
	}

	printf					( "\n\nAll the definitions found (%d):\n", unique_definitions.size() );
	{
		unique_definitions_type::const_iterator	i		= unique_definitions.begin();
		unique_definitions_type::const_iterator	const e = unique_definitions.end();
		for ( ; i != e; ++i )
			printf			( "\t%s\n", (*i).c_str() );
	}
}

void main					( char argc, char* argv[] )
{
	static bool debug_stop = true;
// 	while( debug_stop)
// 	{
// 		Sleep(100);
// 	}

	(void)argc; (void)argv;

	string_path				full_path;
	if ( argc > 1 ) {
		strcpy_s			( full_path, sizeof(full_path), argv[1] );
	}

	_finddata32i64_t		file_desc;
	long handle				= (long)_findfirst32i64( full_path, &file_desc );

	if( handle == -1 )
	{
		_getcwd					( full_path, sizeof(full_path) );
		strcat_s				( full_path, "\\" );

		if ( argc > 1 ) {
			strcat_s			( full_path, sizeof(full_path), argv[1] );
		}

		_finddata32i64_t		file_desc;
		handle				= (long)_findfirst32i64( full_path, &file_desc );

		if( handle == -1 )
		{
			printf( " The specified path \"%s\" not found", full_path );
			return;
		}
	}


	total_info_type			total_info;

	if ( (file_desc.attrib & _A_SUBDIR) == _A_SUBDIR )
	{
		strcat_s				( full_path, "\\" );

		process_shaders			( total_info, full_path, full_path, "*.ps" );
		process_shaders			( total_info, full_path, full_path, "*.vs" );
		process_shaders			( total_info, full_path, full_path, "*.gs" );
		process_shaders			( total_info, full_path, full_path, "*.cs" );
		process_shaders			( total_info, full_path, full_path, "*.ds" );
		process_shaders			( total_info, full_path, full_path, "*.hs" );
	}
	else if ( (file_desc.attrib & _A_NORMAL) == _A_NORMAL )
	{
		string_path base_path;
		string_path base_path_tmp;
		string_path file_name;
		string_path file_name_ext;

		_splitpath_s( full_path, base_path, sizeof(base_path), base_path_tmp, sizeof(base_path_tmp), file_name, sizeof(file_name), file_name_ext, sizeof(file_name_ext) );
		
		strcat_s( base_path, sizeof(base_path), base_path_tmp);
		strcat_s( file_name, sizeof(file_name), file_name_ext);

		process_shader	( base_path, full_path, total_info );
		strcpy_s( full_path, sizeof(full_path), base_path );
	}

//	process_heuristics		( total_info );

	if ( (argc > 1) && (!strcmp("-verbose", argv[1])) ) {
		print_results		( total_info, full_path );
		printf				( "\n\n" );
	}

	void compile_shaders( total_info_type const& total_info, char const* const full_path );
	compile_shaders			( total_info, full_path );
}
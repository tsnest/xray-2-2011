////////////////////////////////////////////////////////////////////////////
//	Created		: 06.03.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "script_engine_wrapper.h"
#include <xray/resources_fs.h>
#include <xray/core_entry_point.h>

static char const _g_lua_body[] = 
	"----------------------------------------------------------------------------			\r\n"
	"--	Module 		: _G.lua																\r\n"
	"--	Created 	: 02.05.2005															\r\n"
	"--  Modified 	: 20.07.2005															\r\n"
	"--	Author		: Dmitriy Iassenev														\r\n"
	"--	Description : functions to store into the global (_G) table							\r\n"
	"----------------------------------------------------------------------------			\r\n"
	"																						\r\n"
	"local super_tables_ids_table_id	= '__super_tables'									\r\n"
	"local super_tables_table_id		= '__super_tables__'								\r\n"
	"																						\r\n"
	"function printf 			( fmt, ... )												\r\n"
	"	log						( string.format(fmt, ...) )									\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function empty				( container )												\r\n"
	"	if ( type(container) == 'function' ) then											\r\n"
	"		for i in container do															\r\n"
	"			return			false														\r\n"
	"		end																				\r\n"
	"																						\r\n"
	"		return				true														\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	for i,j in pairs( container ) do													\r\n"
	"		return				false														\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	return					true														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"local						unique_table_field_counter 	= '__counter__'					\r\n"
	"																						\r\n"
	"function add_property		( storage, id )												\r\n"
	"	if ( storage[unique_table_field_counter] == nil ) then								\r\n"
	"		storage[unique_table_field_counter]	= 0											\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	assert					( storage[id] == nil )										\r\n"
	"																						\r\n"
	"	local					index = storage[unique_table_field_counter]					\r\n"
	"	storage[ id ]			= index														\r\n"
	"	storage[ unique_table_field_counter ]	= index + 1									\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function add_property_pair	( storage, id )												\r\n"
	"	add_property			( storage, id )												\r\n"
	"	storage[ storage[id] ]	= id														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"local function read_only_function		( table, key, value )							\r\n"
	"	error					( 'attempt to update a read-only configuration table', 2 )	\r\n"
	"end																					\r\n"
	"																						\r\n"
	"local function access_function			( table, key )									\r\n"
	"	for i,j in pairs( table[super_tables_table_id] ) do									\r\n"
	"		local				temp = j[ key ]												\r\n"
	"		if ( temp ~= nil ) then															\r\n"
	"			return			temp														\r\n"
	"		end																				\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	return					nil															\r\n"
	"end																					\r\n"
	"																						\r\n"
	"local function select_access_function	( child )										\r\n"
	"	if ( child[super_tables_table_id] == nil ) then										\r\n"
	"		return				nil															\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	return					access_function												\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function inherit_table		( child, super_tables, read_only )							\r\n"
	"	local function select_read_only_function ( )										\r\n"
	"		if (read_only == true) then														\r\n"
	"			return			read_only_function											\r\n"
	"		end																				\r\n"
	"																						\r\n"
	"		return				nil															\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	child[ super_tables_table_id ] = super_tables										\r\n"
	"																						\r\n"
	"	setmetatable			(															\r\n"
	"		child,																			\r\n"
	"		{																				\r\n"
	"			__newindex		= select_read_only_function( ),								\r\n"
	"			__index 		= select_access_function(child),							\r\n"
	"		}																				\r\n"
	"	)																					\r\n"
	"																						\r\n"
	"	return					child														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function create_property_table		( child, environment )								\r\n"
	"	local function object_from_string	( description )									\r\n"
	"		local chunk			= loadstring('return ' .. description)						\r\n"
	"		if ( chunk == nil ) then														\r\n"
	"			return			nil															\r\n"
	"		end																				\r\n"
	"		assert				( chunk )													\r\n"
	"																						\r\n"
	"		local new_environment = { this = environment, }									\r\n"
	"		setmetatable		(															\r\n"
	"			new_environment,															\r\n"
	"			{																			\r\n"
	"				__new_index = read_only_function,										\r\n"
	"				__index 	= environment,												\r\n"
	"			}																			\r\n"
	"		)																				\r\n"
	"																						\r\n"
	"		setfenv				( chunk, new_environment )									\r\n"
	"																						\r\n"
	"		local success, result	=  pcall( chunk )										\r\n"
	"		if ( success ~= true ) then														\r\n"
	"			return			nil															\r\n"
	"		end																				\r\n"
	"																						\r\n"
	"		return				result														\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	local function setup_super_tables	( )												\r\n"
	"		child[ super_tables_table_id ] = { }											\r\n"
	"																						\r\n"
	"		if ( child[super_tables_ids_table_id] == nil ) then								\r\n"
	"			return																		\r\n"
	"		end																				\r\n"
	"																						\r\n"
	"		for i,j in pairs( child[super_tables_ids_table_id] ) do							\r\n"
	"			if ( j == nil ) then														\r\n"
	"				return																	\r\n"
	"			end																			\r\n"
	"																						\r\n"
	"			local			super_table = object_from_string( j )						\r\n"
  	"			--assert			( super_table ~= nil, j )								\r\n"
   	"			if ( super_table ~= nil ) then												\r\n"
	"				table.insert( child[super_tables_table_id], super_table )				\r\n"
   	"			else																		\r\n"
   	"				printf		( 'ERROR: cannot load object from string \"' .. j .. '\"' )	\r\n"
   	"			end																			\r\n"
	"		end																				\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	local function select_function_proxy	( table, key )								\r\n"
	"		setmetatable		(															\r\n"
	"			child,																		\r\n"
	"			{																			\r\n"
	"				__newindex	= nil,														\r\n"
	"				__index 	= nil,														\r\n"
	"			}																			\r\n"
	"		)																				\r\n"
	"																						\r\n"
	"		setup_super_tables	( )															\r\n"
	"																						\r\n"
	"		setmetatable		(															\r\n"
	"			child,																		\r\n"
	"			{																			\r\n"
	"				__newindex	= read_only_function,										\r\n"
	"				__index 	= select_access_function(child),							\r\n"
	"			}																			\r\n"
	"		)																				\r\n"
	"																						\r\n"
	"		if ( key ~= super_tables_table_id ) then										\r\n"
	"			return			access_function( table, key )								\r\n"
	"		end																				\r\n"
	"																						\r\n"
	"		return				child[ super_tables_table_id ]								\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	setmetatable			(															\r\n"
	"		child,																			\r\n"
	"		{																				\r\n"
	"			__newindex		= read_only_function,										\r\n"
	"			__index 		= select_function_proxy,									\r\n"
	"		}																				\r\n"
	"	)																					\r\n"
	"																						\r\n"
	"	return					child														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function merge				( ... )														\r\n"
	"	local					result = { }												\r\n"
	"																						\r\n"
	"	local					args = {...}												\r\n"
	"	local					n = #args													\r\n"
	"	for i=1,n do																		\r\n"
	"		local				j = args[ i ]												\r\n"
	"		if ( j ~= nil ) then															\r\n"
	"			for _i,_j in pairs( j ) do													\r\n"
	"				if ( _j ~= nil ) then													\r\n"
	"					table.insert( result, _j )											\r\n"
	"				end																		\r\n"
	"			end																			\r\n"
	"		end																				\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	return					result														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function merge_pairs		( ... )														\r\n"
	"	local					result = { }												\r\n"
	"																						\r\n"
	"	local					args = { ... }												\r\n"
	"	local					n = #args													\r\n"
	"	for i=1,n do																		\r\n"
	"		local				j = args[ i ]												\r\n"
	"		if ( j ~= nil ) then															\r\n"
	"			for _i,_j in pairs( j ) do													\r\n"
	"				if ( _j ~= nil ) then													\r\n"
	"					result[ _i ]	= _j												\r\n"
	"				end																		\r\n"
	"			end																			\r\n"
	"		end																				\r\n"
	"	end																					\r\n"
	"																						\r\n"
	"	return					result														\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function create_reference	( table_id, environment )									\r\n"
	"	local result			= { }														\r\n"
	"	result[ super_tables_ids_table_id ]	= { table_id }									\r\n"
	"	return					create_property_table( result, environment )				\r\n"
	"end																					\r\n"
	"																						\r\n"
	"function time_global		( )															\r\n"
	"	return					cs.ai.world.m_current_time_ms								\r\n"
	"end																					\r\n"
;

using xray::core::script_engine_wrapper;
using cs::script::file_handle;
using cs::script::file_buffer;

script_engine_wrapper::script_engine_wrapper			( )
{
}

void script_engine_wrapper::log							( cs::message_initiator const &message_initiator, cs::core::message_type const &message_type, const char *string )
{
	XRAY_UNREFERENCED_PARAMETER	(message_initiator);

	logging::verbosity verbosity	= logging::trace;
	switch ( message_type ) {
		case cs::core::message_type_error : {
			verbosity		= logging::error;
			break;
		}
		case cs::core::message_type_warning : {
			verbosity		= logging::warning;
			break;
		}
		case cs::core::message_type_information : {
			verbosity		= logging::info;
			break;
		}
		case cs::core::message_type_output : {
			verbosity		= logging::debug;
			break;
		}
		default : NODEFAULT();
	}

	LOGI					( "script", verbosity, "%s", string );
}

pcstr script_engine_wrapper::get_file_name	( int const file_type, pcstr file_name, pstr const result, u32 const result_size, bool add_extension )
{
	fs_new::native_path_string			path;

	pcstr					extension = "";
	pcstr					folder = "";
	switch (file_type) {
		case (cs::script::engine::file_type_script) : {
			if ( add_extension )
				extension	= ".lua";

#if 1
			pstr temp	= 0;
			STR_JOINA( temp, "resources/", file_name );
			if ( resources::convert_virtual_to_physical_path( &path , temp, resources::sources_mount ) )
				file_name	= path.c_str();
#else // #if 0
			if ( resources::convert_virtual_to_physical_path( &path , file_name ) )
				file_name	= path.c_str();
#endif // #if 0
			break;
		}
		case (cs::script::engine::file_type_script_chunk) : {
			if ( add_extension )
				extension	= ".lua_chunk";

#if 1
			pstr temp		= 0;
			STR_JOINA( temp, "resources/", file_name );
			if ( resources::convert_virtual_to_physical_path( &path , temp, resources::sources_mount ) )
				file_name	= path.c_str();
#else // #if 0
			if ( resources::convert_virtual_to_physical_path( &path , temp ) )
				file_name	= path.c_str();
#endif // #if 0

			break;
		}
		case (cs::script::engine::file_type_config) : {
			UNREACHABLE_CODE();
			break;
		}
		default :			return false;
	}

	R_ASSERT				( (!strchr(file_name, ':') && !strings::starts_with(file_name, "../")) || !*folder );

	xray::buffer_string dest(result, result_size);
	dest					= folder;
	dest					+= file_name;
	dest					+= extension;

	return					(result);
}

bool script_engine_wrapper::file_exists					( int file_type, const char *file_name )
{
	string_path				real_file_name;

	if ( (file_name[0] == '_') && (file_name[1] == 'G') && (file_name[2] == 0) )
		return				true;

	fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

	if ( !device->file_exists(get_file_name( file_type, file_name, real_file_name, sizeof(real_file_name), false)) )
		if ( !device->file_exists(get_file_name( file_type, file_name, real_file_name, sizeof(real_file_name), true)) )
			return			false;

	return					true;
}

file_handle script_engine_wrapper::open_file_buffer		( int file_type, const char *file_name, cs::script::file_buffer &file_buffer, u32 &buffer_size )
{
	string_path				real_file_name;

	using namespace	fs_new;
	fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

	fs_new::file_type *	file;
	if ( (file_name[0] == '_') && (file_name[1] == 'G') && (file_name[2] == 0) ) {
		file_buffer			= static_cast< pcvoid >( _g_lua_body );
		buffer_size			= (array_size(_g_lua_body) - 1)*sizeof(_g_lua_body[0]);
		return				_g_lua_body;
	}

	if ( !device->open( & file, 
						get_file_name( file_type, file_name, real_file_name, sizeof(real_file_name), false ),
						file_mode::open_existing, 
						file_access::read,
						assert_on_fail_false) ) 
	{
		if ( !device->open( & file, 
							get_file_name( file_type, file_name, real_file_name, sizeof(real_file_name), true ), 
							file_mode::open_existing, 
							file_access::read,
							assert_on_fail_false) ) 
		{
			FATAL			( "cannot open file %s", real_file_name );
			return			0;
		}
	}

	device->seek			( file, 0, seek_file_end );
	buffer_size				= (u32)device->tell(file);
	device->seek			( file, 0, seek_file_begin );
	pvoid const buffer		= DEBUG_MALLOC( (u32)buffer_size, "script" );
	device->read			( file, buffer, buffer_size );
	device->close			( file );

	file_buffer				= buffer;
	return					( (cs::script::file_handle)file_buffer );
}

void script_engine_wrapper::close_file_buffer			( cs::script::file_handle file_handle )
{
	ASSERT					( file_handle );

	pvoid buffer			= (void*)file_handle;
	if ( buffer != _g_lua_body )
		DEBUG_FREE			( buffer );
}

bool script_engine_wrapper::create_file					( int file_type, const char *file_name, const cs::script::file_buffer &file_buffer, const u32 &buffer_size )
{
	XRAY_UNREFERENCED_PARAMETERS(file_type, file_name, file_buffer, buffer_size);
	UNREACHABLE_CODE		(return false);
}

void script_engine_wrapper::lua_studio_backend_file_path( int file_type, const char *file_name, char *path, const u32 &max_size )
{
	if ( (file_name[0] == '_') && (file_name[1] == 'G') && (file_name[2] == 0) ) {
		strings::copy		( path, max_size, "../../resources/sources/scripts/_g.lua" );
		return;
	}

	fs_new::synchronous_device_interface const &	device	=	core::get_core_synchronous_device( );

	if ( !device->file_exists(get_file_name( file_type, file_name, path, max_size, false)) && 
		 !device->file_exists(get_file_name( file_type, file_name, path, max_size, true)) )
	{
		UNREACHABLE_CODE( );
	}
}
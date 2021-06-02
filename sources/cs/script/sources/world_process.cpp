////////////////////////////////////////////////////////////////////////////
//	Module 		: world_process.cpp
//	Created 	: 02.07.2005
//  Modified 	: 02.07.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class : process filefunctionality
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"

#ifdef _MSC_VER
#	define TRY_STACK_FIRST
#endif // #ifdef _MSC_VER

using cs::script::detail::world;

#ifdef TRY_STACK_FIRST
#	include <cs/core/os_include.h>		// GetExceptionCode
#endif

pcstr script_header = "\
	local __FILE__ = \"%s\"\
	local this = {}\
	%s this %s\
	setmetatable(this, {__index = _G})\
	setfenv(1, this)\
";

#ifdef TRY_STACK_FIRST
#	define MAX_STACK_SIZE	256*1024
#endif // #ifdef TRY_STACK_FIRST

bool world::generate_this_string	(lua_State* state, pcstr table_name, pstr left_part, u32 left_size, pstr right_part, u32 right_size)
{
	sz_cpy							(left_part,left_size,"");
	sz_cpy							(right_part,right_size,"");
	u32								table_name_length = sz_len(table_name) + 1;
	pstr							S2 = (pstr)ALLOCA(table_name_length*sizeof(char));
	sz_cpy							(S2,table_name_length*sizeof(char),table_name);
	pstr							S = S2;
	pstr							S1 = S;
	for (int i=0;;++i) {
		if (!sz_len(S)) {
			lua_world(state).msg	(cs::core::message_type_error,"the namespace name %s is incorrect!",table_name);
			return					false;
		}

		u32 quote_nesting			= 0;
		pstr c;
		for ( c = S1; *c; ++c ) {
			if ( (*c == '\'') || (*c == '\"') ) {
				++quote_nesting;
				continue;
			}

			if ( quote_nesting )
				continue;

			if ( *c == '.' ) {
				S1					= c;
				break;
			}
		}

		if ( !*c )
			S1						= 0;

		if (i)
			sz_cat					(left_part,left_size,"{} end ");
		
		if (S1)
			sz_cat					(left_part,left_size,"if ");

		sz_cat						(left_part,left_size,S);

		if (S1) {
			sz_cat					(left_part,left_size," == nil then ");
			sz_cat					(left_part,left_size,S);
		}

		sz_cat						(left_part,left_size," = ");

		if (S1) {
			*S1						= '.';
			++S1;
		}
		else
			break;
	}

	return							true;
}

struct chunk_loader_data {
	pcstr							m_buffer;
	u32							m_size;
};

char const* world::load_chunk		(lua_State* L, void* ud, size_t* size)
{
	CS_UNREFERENCED_PARAMETER		( L );

	chunk_loader_data&				data = *(chunk_loader_data*)ud;
	if (!data.m_size)
		return						0;

	*size							= data.m_size;
	data.m_size						= 0;
	return							data.m_buffer;
}

bool world::process_buffer			(lua_State* state, pcstr buffer, u32 buffer_size, pcstr file_name)
{
	int								error_code;
	chunk_loader_data				data;
	data.m_buffer					= buffer;
	data.m_size						= buffer_size;
	error_code						= lua_load(state,load_chunk,&data,file_name);

	if (!error_code)
		return						true;

	process_error					(state,file_name,error_code);

	return							false;
}

bool world::process_buffer			(lua_State* state, pcstr buffer, u32 buffer_size, pcstr file_name, pcstr table_name, bool chunk)
{
	if (!table_name || !sz_cmp("_G",table_name) || chunk)
		return						process_buffer(state,buffer,buffer_size,file_name);

	string4096						header, left_part, right_part;

	if (!generate_this_string(state, table_name, left_part, sizeof(left_part), right_part, sizeof(right_part)))
		return						false;

	cs::sprintf						(header, script_header, table_name, left_part, right_part);

	u32								header_length = sz_len(header);
	u32								allocation_count = header_length + buffer_size + 1;
	pstr							script_body;

#ifdef TRY_STACK_FIRST
	bool							dynamic_allocation = false;

	__try {
		if (allocation_count*sizeof(char) < MAX_STACK_SIZE)
			script_body				= (pstr)ALLOCA(allocation_count*sizeof(char));
		else {
			script_body				= cs_alloc<char>(allocation_count, "lua script file");
			dynamic_allocation		= true;
		}
	}
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        int							errcode = _resetstkoflw();
        R_ASSERT					(errcode, "Could not reset the stack after \"Stack overflow\" exception!");
		script_body					= cs_alloc<char>(allocation_count, "lua script file (after exception)");
		dynamic_allocation			= true;
    };
#else // TRY_STACK_FIRST
	script_body						= cs_alloc<char>(allocation_count, "lua script file");
#endif // #ifdef TRY_STACK_FIRST

	sz_cpy							(script_body,header_length + buffer_size + 1,header);
	mem_copy						(script_body + header_length,buffer,u32(buffer_size));
	
	bool							result = process_buffer(state,script_body,buffer_size + header_length,file_name);
	
#ifdef TRY_STACK_FIRST
	if (dynamic_allocation)
		cs_free						(script_body);
#else // TRY_STACK_FIRST
	cs_free							(script_body);
#endif // #ifdef TRY_STACK_FIRST

	return							result;
}

struct chunk_saver_data {
	pstr							m_buffer;
	u32							m_size;
	u32							m_allocated_size;
};

int world::save_chunk				(lua_State* state, void const* p, u32 size, void* u)
{
	CS_UNREFERENCED_PARAMETER		( state );

	chunk_saver_data&				data = *(chunk_saver_data*)u;

	if (data.m_allocated_size < data.m_size + size) {
		while (data.m_allocated_size < data.m_size + size)
			data.m_allocated_size	*= 2;
		data.m_buffer				= (pstr)cs_realloc(data.m_buffer, data.m_allocated_size, "Lua chunk");
	}
	mem_copy						(data.m_buffer + data.m_size,p,(u32)size);
	data.m_size						+= size;
	return							1;
}

bool world::execute_file_in_table	(pcstr file_name, pcstr table_name)
{
	int								start = lua_gettop(state());
	
	string256						file_name_description;
	cs::script::file_buffer			buffer;
	u32								buffer_size;
	bool							chunk = false;
	cs::script::file_handle			file_handle = 0;
	if (engine().file_exists(cs::script::engine::file_type_script,file_name))
		file_handle					= engine().open_file_buffer(cs::script::engine::file_type_script,file_name,buffer,buffer_size);
	else {
		chunk						= true;

		if (engine().file_exists(cs::script::engine::file_type_script_chunk,file_name))
			file_handle				= engine().open_file_buffer(cs::script::engine::file_type_script_chunk,file_name,buffer,buffer_size);
		else  {
			msg						(cs::core::message_type_error,"Cannot open script file \"%s\"",file_name);
			return					false;
		}
	}
	
	{
		string512					temp;
		engine().lua_studio_backend_file_path	(cs::script::engine::file_type_script,file_name,temp,sizeof(temp));
		sz_lwr						(temp, sizeof(temp));
		sz_concat					(file_name_description,sizeof(file_name_description),"@",temp);
	}
	
	if (!process_buffer(state(),(pcstr)buffer,buffer_size,file_name_description,table_name,chunk)) {
		engine().close_file_buffer	(file_handle);
		lua_settop					(state(),start);
		return						false;
	}
	
	engine().close_file_buffer		(file_handle);

#if 0
	if (!chunk) {
		chunk_saver_data			temp;
		temp.m_allocated_size		= buffer_size > 1024 ? buffer_size : 1024;
		temp.m_buffer				= (pstr)cs_malloc(temp.m_allocated_size, "Lua chunk");
		temp.m_size					= 0;
		lua_dump					(state(),save_chunk,&temp);
		engine().create_file		(script::engine::file_type_script_chunk,file_name,(script::file_buffer const& )temp.m_buffer,temp.m_size);
		cs_free						(temp.m_buffer);
	}
#endif // #if 0

	int								error_code = luabind::detail::pcall(state(),0,0);
	
	if (!error_code)
		return						true;

	process_error					(state(),file_name,error_code);

	lua_settop						(state(),start);
	return							false;
}

bool world::execute_file			(pcstr file_name)
{
	pcstr const table_name_first	= strrchr(file_name,'/');
	pcstr const table_name_second	= strrchr(file_name,'\\');
	pcstr const table_name_last		= std::max(table_name_first,table_name_second);
	pcstr const table_name			= table_name_last ? (table_name_last + 1) : file_name;
	if (!sz_len(table_name) || !sz_cmp(table_name,"_g") || !sz_cmp(table_name,"_G"))
		return						execute_file_in_table(file_name,"_G");
	else {
		string_path					temp;
		sz_cpy						(temp, "modules['");
		sz_cat						(temp, file_name);
		sz_cat						(temp, "']");
		return						execute_file_in_table(file_name,temp);
	}
}

void world::process_file_internal	(pcstr file_name, bool assert_if_not_exist)
{
	bool							global_script_loaded = (!*file_name || !sz_cmp(file_name,"_G"));
	if (global_script_loaded || m_state_reload || !table_loaded(file_name)) {
		if	(
				!assert_if_not_exist &&
				(
					!previous_not_exist_query(file_name) ||
					(
						!engine().file_exists(cs::script::engine::file_type_script,file_name) &&
						!engine().file_exists(cs::script::engine::file_type_script_chunk,file_name)
					)
				)
			)
		{
			on_file_no_exist		(file_name);
			return;
		}

		m_state_reload				= false;
		msg							(cs::core::message_type_information,"loading script \"%s\"",file_name);
		execute_file				(file_name);
	}
}

void world::process_file			(pcstr file_name, bool state_reload)
{
	m_state_reload					= state_reload;
	process_file_internal			(file_name,true);
	m_state_reload					= false;
}

bool world::load_string				(pcstr string_to_do, pcstr id)
{
	int								error_code = 
		luaL_loadbuffer(
			state(),
			string_to_do,
			sz_len(string_to_do),
			id
		);

	if (error_code) {
		process_error				(state(), id, error_code);
		return						false;
	}

	return							true;
}

bool world::do_string				(lua_State* state, pcstr string_to_do, pcstr id)
{
	if (!load_string(string_to_do, id))
		return						false;

	int								error_code = luabind::detail::pcall(state,0,0);
	if (error_code) {
		process_error				(state,id,error_code);
		return						false;
	}

	return							true;
}

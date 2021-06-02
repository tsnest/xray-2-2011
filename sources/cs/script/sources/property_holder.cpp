////////////////////////////////////////////////////////////////////////////
//	Module 		: property_holder.cpp
//	Created 	: 12.11.2005
//  Modified 	: 12.11.2005
//	Author		: Dmitriy Iassenev
//	Description : property holder class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "property_holder.h"
#include "world.h"
#include <cs/core/string_stream.h>

using cs::script::detail::property_holder;
using cs::script::detail::world;

#ifndef lua_Integer
#	define	lua_Integer		int
#	define	lua_tointeger	(lua_Integer)lua_tonumber
#endif // lua_Integer

static bool valid_identifier									(str_shared const& string)
{
	if ((**string >= '0') && (**string <= '9'))
		return						false;

	pcstr							I = *string;
	pcstr							E = *string + string.size();
	for ( ; I != E; ++I) {
		if ((*I >= 'a') && (*I <= 'z'))
			continue;

		if ((*I >= 'A') && (*I <= 'Z'))
			continue;

		if ((*I == '_'))
			continue;

		if ((*I >= '0') && (*I <= '9'))
			continue;

		return						false;
	}

	return							true;
}

static void process_string										(string_stream& stream, pcstr value, u32 const length)
{
	string4096						temp;

	mem_copy						(temp,value,length*sizeof(char));
	temp[length]					= 0;
	pcstr							i = value;
	pcstr							e = value + length;
	pstr							j = temp;

	if ((*i == '"') && (length > 1) && (*(e - 1) == '"')) {
		++i;					
		--e;
	}

	for ( ; i != e; ++i, ++j) {
		*j							= *i;
		if (*i != '\\') {
			if (*i != '"') {
				if (*i >= 32)
					continue;

				if (!*i)
					continue;

				*j					= '\\';
				if (*i<10) {
					*++j			= *i + '0';
					continue;
				}
				*++j				= (*i)/10 + '0';
				*++j				= (*i)%10 + '0';
				continue;
			}
			else
				*j					= '\\';
		}

		*++j						= *i;
	}
	*j								= 0;

	if (temp[0] != '"') {
		stream.msg					("\"%s\"",temp);
		return;
	}

	if (*(j - 1) != '"') {
		stream.msg					("\"%s\"",temp + 1);
		return;
	}

	stream.msg						("%s",temp);
}

property_holder::property_holder							(world* world, pcstr file_name, u8 const flags) :
	m_world						(*world)
{
	construct					(file_name, flags);
}

void property_holder::construct								(pcstr file_name, u8 const flags)
{
	m_id						= file_name;
	m_flags						= flags;

	if	(
			((flags&  load_on_construct) == load_on_construct) ||
			m_world.engine().file_exists(cs::script::engine::file_type_script,file_name) || 
			m_world.engine().file_exists(cs::script::engine::file_type_script_chunk,file_name)
		)
	{
		m_world.execute_script	(file_name);
	}
	else {
		if (!m_world.luabind_object(file_name,m_object,LUA_TTABLE)) {
			m_object			= luabind::globals(m_world.virtual_machine());
			m_object			= create_section_object(file_name);
		}
	}

	pcstr modules				= "modules.";
	u32 const modules_length	= sz_len(file_name);
	u32 const file_name_length	= sz_len(file_name);
	u32 const buffer_length		= ( modules_length + file_name_length + 1 ) *sizeof( char );
	pstr const string			= (pstr)ALLOCA( buffer_length );
	sz_concat					( string, buffer_length, modules, file_name );

	bool						success = m_world.luabind_object( string, m_object, LUA_TTABLE );
	R_ASSERT					(success,"Failed to load configuration file \"%s\"",file_name);
}

bool property_holder::table									(object_type const& section) const
{
	return						luabind::type(section) == LUA_TTABLE;
}

struct section_object_predicate {
	lua_State*					m_state;

	inline		section_object_predicate		(lua_State* state)
	{
		ASSERT					(state, "null lua state passed");
		m_state					= state;
	}

	inline bool operator()						(int const index, pcstr string, u32 const length, bool const last) const
	{
		CS_UNREFERENCED_PARAMETER	( index );
		CS_UNREFERENCED_PARAMETER	( length );
		CS_UNREFERENCED_PARAMETER	( last );

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-2);
		if (!lua_isnil(m_state,-1)) {
			lua_replace			(m_state,-2);
			return				true;
		}

		FATAL					("cannot find section '%s'",string);
		return					false;
	}
};

property_holder::object_type property_holder::section_object(pcstr section) const
{
	lua_State*					state = object().interpreter();
	object().push				(state);
	iterate_items				(section,section_object_predicate(state),'.');
	return						luabind::object( luabind::handle( state, -1 ) );
}

struct create_section_object_predicate {
	lua_State*					m_state;

	inline		create_section_object_predicate	(lua_State* state)
	{
		ASSERT					(state, "null lua state passed");
		m_state					= state;
	}

	inline bool operator()						(int const index, pcstr string, u32 const length, bool const last) const
	{
		CS_UNREFERENCED_PARAMETER	( index );
		CS_UNREFERENCED_PARAMETER	( length );
		CS_UNREFERENCED_PARAMETER	( last );

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-2);
		if (!lua_isnil(m_state,-1)) {
			lua_replace			(m_state,-2);
			return				true;
		}

		lua_pop					(m_state,1);
		lua_pushstring			(m_state,string);
		lua_newtable			(m_state);
		lua_rawset				(m_state,-3);

		lua_pushstring			(m_state,"create_section");
		lua_gettable			(m_state,LUA_GLOBALSINDEX);

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-3);

		lua_call				(m_state,1,1);
		
		lua_replace				(m_state,-2);
		return					true;
	}
};

property_holder::object_type property_holder::create_section_object	(pcstr section) const
{
	lua_State*					state = object().interpreter();
	object().push				(state);
	iterate_items				(section,create_section_object_predicate(state),'.');
	lua_pop						(state,1);
	return						section_object(section);
}

struct section_exist_predicate {
	lua_State*					m_state;

	inline		section_exist_predicate	(lua_State* state)
	{
		ASSERT					(state, "null lua state passed");
		m_state					= state;
	}

	inline bool	operator()				(int const index, pcstr string, u32 const length, bool const last) const
	{
		CS_UNREFERENCED_PARAMETER	( index );
		CS_UNREFERENCED_PARAMETER	( length );
		CS_UNREFERENCED_PARAMETER	( last );

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-2);
		if (!lua_isnil(m_state,-1)) {
			lua_replace			(m_state,-2);
			return				true;
		}

		return					false;
	}
};

bool property_holder::section_exist							(pcstr section) const
{
	lua_State*					state = object().interpreter();
	object().push				(state);
	
	if (!iterate_items(section,section_exist_predicate(state),'.'))
		return					false;

	bool						result = lua_istable(state,-1);
	lua_pop						(state,1);
	return						result;
}

bool property_holder::line_exist							(pcstr section, pcstr line) const
{
	if (!section_exist(section))
		return					false;

	object_type					tag = section_object(section);
	return						table(tag) && (luabind::type(tag[line]) != LUA_TNIL);
}

bool property_holder::is_section							(object_type object) const
{
	if (!table(object))
		return					false;

	object.push					(object.interpreter());
	bool						result = !!lua_getmetatable(object.interpreter(),-1);
	lua_pop						(object.interpreter(),1);
	return						result;
}

u32	property_holder::line_count								(pcstr section) const
{
	object_type					tag = section_object(section);

	if (!table(tag))
		return					0;

	u32							i = 0;
	iterator					I(tag);
	iterator					E;
	for ( ; I != E; ++I) {
		if (!is_section(*I))
			++i;
	}
	return						i;
}

bool property_holder::r_line								(pcstr section, int line_id, pcstr& id, pcstr& value) const
{
	object_type					tag = r_section(section);
	iterator					I(tag);
	iterator					E;
	for (int i=0; I != E; ++I) {
		if (is_section(*I))
			continue;
		
		if (i == line_id) {
			id					= cast<pcstr>(I.key());
			value				= cast<pcstr>(*I);
			return				true;
		}

		++i;
	}
	return						false;
}

property_holder::object_type property_holder::r_section		(pcstr section) const
{
	object_type					tag = section_object(section);
	if (table(tag))
		return					tag;
	
	FATAL						("Can't open section '%s'",section);
	return						tag;
}

inline property_holder::object_type property_holder::value	(pcstr section, pcstr line) const
{
	object_type					tag = r_section(section)[line];
	if (luabind::type(tag) != LUA_TNIL)
		return					tag;

	FATAL						("Can't find variable %s in [%s]",line,section);
	return						object_type();
}

template <typename _result_type>
inline _result_type property_holder::value					(pcstr section, pcstr line) const
{
	return						luabind::object_cast<_result_type>(value(section,line));
}

pcstr property_holder::r_code								(pcstr section, pcstr line) const
{
	string256					temp;
	sz_concat					(temp, section, ".", line);
	return						value<pcstr>(temp,"__description__");
}

pcstr property_holder::r_string								(pcstr section, pcstr line) const
{
	return						value<pcstr>(section,line);
}

property_holder::object_type property_holder::r_object		(pcstr section, pcstr line) const
{
	return						value(section,line);
}

bool property_holder::r_bool								(pcstr section, pcstr line) const
{
	object_type					tag = value(section,line);
	switch (luabind::type(tag)) {
		case LUA_TBOOLEAN :		return	luabind::object_cast<bool>(tag);
		case LUA_TNUMBER  :		return	!!luabind::object_cast<int>(tag);
		case LUA_TSTRING  :		{
			pcstr				string = luabind::object_cast<pcstr>(tag);

			string16			temp;
			sz_ncpy				(temp,string,4);
			sz_lwr				(temp, sizeof(temp));
			
			u32					temp_n = *(u32*)temp;

			pcstr				yes = "yes";
			if ((*((u32*)(yes))) == temp_n)
				return			true;

			pcstr				on = "on";
			if (((*((u32*)(on))) & 0x00ffffff) == temp_n)
				return			true;

			return				false;
		}
	}
	return						false;
}

u8 property_holder::r_u8									(pcstr section, pcstr line) const
{
	return						value<u8>(section,line);
}

u16 property_holder::r_u16									(pcstr section, pcstr line) const
{
	return						value<u16>(section,line);
}

u32 property_holder::r_u32									(pcstr section, pcstr line) const
{
	return						value<u32>(section,line);
}

s8 property_holder::r_s8									(pcstr section, pcstr line) const
{
	return						value<s8>(section,line);
}

s16 property_holder::r_s16									(pcstr section, pcstr line) const
{
	return						value<s16>(section,line);
}

s32 property_holder::r_s32									(pcstr section, pcstr line) const
{
	return						value<s32>(section,line);
}

float property_holder::r_float								(pcstr section, pcstr line) const
{
	return						value<float>(section,line);
}

#if 0
vec2f property_holder::r_vec2f							(pcstr section, pcstr line) const
{
	object_type					tag = value(section,line);
	if (tag.type() != LUA_TSTRING) {
		if (tag.type() == LUA_TTABLE)
			return				vec2f().set(cast<float>(tag[1]),cast<float>(tag[2]));
		return					luabind::object_cast<vec2f>(tag);
	}

	pcstr						string = luabind::object_cast<pcstr>(tag);
	vec2f						result;
	sscanf_s					(string,"%f,%f",&result[0],&result[1]);
	return						result;
}

vec3f property_holder::r_vec3f							(pcstr section, pcstr line) const
{
	object_type					tag = value(section,line);
	if (tag.type() != LUA_TSTRING) {
		if (tag.type() == LUA_TTABLE)
			return				vec3f().set(cast<float>(tag[1]),cast<float>(tag[2]),cast<float>(tag[3]));
		return					luabind::object_cast<vec3f>(tag);
	}

	pcstr						string = luabind::object_cast<pcstr>(tag);
	vec3f						result;
	sscanf_s					(string,"%f,%f,%f",&result[0],&result[1],&result[2]);
	return						result;
}

vec4f property_holder::r_vec4f							(pcstr section, pcstr line) const
{
	object_type					tag = value(section,line);
	if (tag.type() != LUA_TSTRING) {
		if (tag.type() == LUA_TTABLE)
			return				vec4f().set(cast<float>(tag[1]),cast<float>(tag[2]),cast<float>(tag[3]),cast<float>(tag[4]));
		return					luabind::object_cast<vec4f>(tag);
	}

	pcstr						string = luabind::object_cast<pcstr>(tag);
	vec4f						result;
	sscanf_s					(string,"%f,%f,%f,%f",&result[0],&result[1],&result[2],&result[3]);
	return						result;
}
#endif // #if 0

template <typename T, typename P>
void property_holder::w									(pcstr S, pcstr L, T V, pcstr comment, P const& predicate)
{
	CS_UNREFERENCED_PARAMETER	( comment );

	lua_State*					state = object().interpreter();
	create_section_object(S).push	(state);
	lua_pushstring				(state,L);
	predicate					(state,V);
	lua_rawset					(state,-3);
	lua_pop						(state,1);
}

void property_holder::w_string		(pcstr S, pcstr L, pcstr V, pcstr comment)
{
	w							(S,L,V,comment,&lua_pushstring);
}

void property_holder::w_u8			(pcstr S, pcstr L, u8 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_u16			(pcstr S, pcstr L, u16 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_u32			(pcstr S, pcstr L, u32 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_s8			(pcstr S, pcstr L, s8 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_s16			(pcstr S, pcstr L, s16 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_s32			(pcstr S, pcstr L, s32 V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_float		(pcstr S, pcstr L, float V, pcstr comment)
{
	w							(S,L,(lua_Number)V,comment,&lua_pushnumber);
}

void property_holder::w_bool		(pcstr S, pcstr L, bool V, pcstr comment)
{
	w							(S,L,V,comment,&lua_pushboolean);
}

#if 0
template <typename type>
void property_holder::w_vector2		(pcstr S, pcstr L, type const& V, pcstr comment)
{
	lua_State*					state = object().lua_state();
	create_section_object(S).push	(state);
	lua_pushstring				(state,L);
	
	lua_newtable				(state);

	lua_pushnumber				(state,1);
	lua_pushnumber				(state,(lua_Number)V.x);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,2);
	lua_pushnumber				(state,(lua_Number)V.y);
	lua_rawset					(state,-3);

	lua_rawset					(state,-3);
	lua_pop						(state,1);
}

template <typename type>
void property_holder::w_vector3		(pcstr S, pcstr L, type const& V, pcstr comment)
{
	lua_State*					state = object().lua_state();
	create_section_object(S).push	(state);
	lua_pushstring				(state,L);
	
	lua_newtable				(state);

	lua_pushnumber				(state,1);
	lua_pushnumber				(state,(lua_Number)V.x);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,2);
	lua_pushnumber				(state,(lua_Number)V.y);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,3);
	lua_pushnumber				(state,(lua_Number)V.z);
	lua_rawset					(state,-3);

	lua_rawset					(state,-3);
	lua_pop						(state,1);
}

template <typename type>
void property_holder::w_vector4		(pcstr S, pcstr L, type const& V, pcstr comment)
{
	lua_State*					state = object().lua_state();
	create_section_object(S).push	(state);
	lua_pushstring				(state,L);
	
	lua_newtable				(state);

	lua_pushnumber				(state,1);
	lua_pushnumber				(state,(lua_Number)V.x);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,2);
	lua_pushnumber				(state,(lua_Number)V.y);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,3);
	lua_pushnumber				(state,(lua_Number)V.z);
	lua_rawset					(state,-3);

	lua_pushnumber				(state,4);
	lua_pushnumber				(state,(lua_Number)V.w);
	lua_rawset					(state,-3);

	lua_rawset					(state,-3);
	lua_pop						(state,1);
}
#endif // #if 0

struct predicate : private boost::noncopyable {
	world&						m_world;

	inline		predicate		(world& world) :
		m_world					(world)
	{
	}
	
	inline void	operator()		(lua_State* state, pcstr body) const
	{
		ASSERT					(m_world.state() == state);
		lua_newtable			(state);
		lua_pushstring			(state, "__description__");
		lua_pushstring			(state, body);
		lua_rawset				(state, -3);

		lua_pushstring			(state, "__code__");

		bool					result = m_world.load_string(body,body);
		if (!result)
			lua_pushnil			(state);

		lua_rawset				(state, -3);
	}
}; // struct predicate

void property_holder::w_code		(pcstr S, pcstr L, pcstr V, pcstr comment)
{
	w							(S,L,V,comment, predicate(m_world));
}

void property_holder::remove_line	(pcstr S, pcstr L)
{
	lua_State*					state = object().interpreter();
	create_section_object(S).interpreter	();
	lua_pushstring				(state,L);
	lua_pushnil					(state);
	lua_rawset					(state,-3);
	lua_pop						(state,1);
}

struct remove_section_predicate {
	lua_State*					m_state;

	inline		remove_section_predicate(lua_State* state)
	{
		ASSERT					(state, "null lua state passed");
		m_state					= state;
	}

	inline bool	operator()				(int const index, pcstr string, u32 const length, bool const last) const
	{
		CS_UNREFERENCED_PARAMETER	( index );
		CS_UNREFERENCED_PARAMETER	( length );

		if (last) {
			lua_pushstring		(m_state,string);
			lua_pushnil			(m_state);
			lua_rawset			(m_state,-3);
			lua_pop				(m_state,1);
			return				true;
		}

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-2);
		if (!lua_isnil(m_state,-1)) {
			lua_replace			(m_state,-2);
			return				true;
		}

		lua_pop					(m_state,1);
		lua_pushstring			(m_state,string);
		lua_newtable			(m_state);
		lua_rawset				(m_state,-3);

		lua_pushstring			(m_state,"create_section");
		lua_gettable			(m_state,LUA_GLOBALSINDEX);

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-3);

		lua_call				(m_state,1,1);
		
		lua_replace				(m_state,-2);

		return					true;
	}
};

void property_holder::remove_section(pcstr S)
{
	lua_State*					state = object().interpreter();
	object().push				(state);
	iterate_items				(S,remove_section_predicate(state),'.');
}

namespace cs {
namespace script {
namespace detail {

struct reference_section_predicate {
	property_holder*				m_holder;
	lua_State*					m_state;
	pcstr						m_old_section_id;

	inline		reference_section_predicate	(property_holder* holder, pcstr old_section_id)
	{
		ASSERT					(holder, "null property holder passed");
		m_holder				= holder;

		m_state					= m_holder->object().interpreter();

		ASSERT					(old_section_id, "null string passed");
		m_old_section_id		= old_section_id;
	}

	inline bool	operator()					(int const index, pcstr string, u32 const length, bool const last) const
	{
		CS_UNREFERENCED_PARAMETER	( index );
		CS_UNREFERENCED_PARAMETER	( length );

		if (last) {
			lua_pushstring		(m_state,string);

			m_holder->create_section_object(
				m_old_section_id
			).push				(m_state);

			lua_rawset			(m_state,-3);
			lua_pop				(m_state,1);
			return				true;
		}

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-2);
		if (!lua_isnil(m_state,-1)) {
			lua_replace			(m_state,-2);
			return				true;
		}

		lua_pop					(m_state,1);
		lua_pushstring			(m_state,string);
		lua_newtable			(m_state);
		lua_rawset				(m_state,-3);

		lua_pushstring			(m_state,"create_section");
		lua_gettable			(m_state,LUA_GLOBALSINDEX);

		lua_pushstring			(m_state,string);
		lua_gettable			(m_state,-3);

		lua_call				(m_state,1,1);
		
		lua_replace				(m_state,-2);

		return					true;
	}
};

} // namespace detail
} // namespace script
} // namespace cs

void property_holder::reference_section	(pcstr old_section_id, pcstr new_section_id)
{
	object().push				(object().interpreter());
	iterate_items				(new_section_id,reference_section_predicate(this,old_section_id),'.');
}

void property_holder::rename_section	(pcstr old_section_id, pcstr new_section_id)
{
	reference_section			(old_section_id,new_section_id);
	remove_section				(old_section_id);
}

void property_holder::setup_bases	(pcstr S, pcstr L)
{
	lua_State*					state = object().interpreter();
	create_section_object(S).push	(state);
	lua_pushstring				(state,"__bases");
	
	lua_newtable				(state);

	u32							n = sz_len(L);
	pstr						temp_string = (pstr)ALLOCA((n+1)*sizeof(char));
	pcstr						I = L;
	pstr						i = temp_string, j = i;
	int							index = 0;
	for ( ; *I; ++I, ++i) {
		if (*I != ',') {
			*i					= *I;
			continue;
		}

		*i						= 0;

		lua_pushnumber			(state,(lua_Number)++index);
		lua_pushstring			(state,j);
		lua_rawset				(state,-3);

		j						= i + 1;
	}

	if (!index) {
		lua_pushnumber			(state,(lua_Number)++index);
		lua_pushstring			(state,L);
		lua_rawset				(state,-3);
	}
	else {
		*i						= 0;
		lua_pushnumber			(state,(lua_Number)++index);
		lua_pushstring			(state,j);
		lua_rawset				(state,-3);
	}

	lua_rawset					(state,-3);
	lua_pop						(state,1);
}

bool string_comparator				(pcstr _0, pcstr _1)
{
	return						sz_cmp(_0,_1) < 0;
}

void property_holder::save			(lua_State* state, string_stream& stream, pstr indent, u32 indent_size, pstr previous_indent, u32 level)
{
	switch (lua_type(state,-1)) {
		case LUA_TBOOLEAN		: {
			stream.msg			("%s,\r\n",lua_toboolean(state,-1) ? "true" : "false");
			break;
		}
		case LUA_TNUMBER		: {
			lua_Number			number = lua_tonumber(state,-1);
			lua_Integer			integer = lua_tointeger(state,-1);
			if (number != (lua_Number)integer)
				stream.msg		("%f,\r\n",number);
			else
				stream.msg		("%d,\r\n",integer);
			break;
		}
		case LUA_TSTRING		: {
			pcstr				string = lua_tostring(state,-1);
			process_string		(stream, string, (sz_len(string) + 1)*sizeof(char));
			stream.msg			(",\r\n");
			break;
		}
		case LUA_TTABLE			: {
			pstr				temp = (pstr)ALLOCA(indent_size*sizeof(char));
			sz_cpy				(temp,((u32)indent_size)*sizeof(char),indent);
			if (level)
				stream.msg		("create_section {\r\n",lua_tostring(state,-2));
			sz_cat				(indent,indent_size,"\t");

			typedef cs_vector<pcstr>	KEYS;
			KEYS				keys;
			lua_pushnil			(state);
			for (int i=0; lua_next(state,-2); ++i) {
				if (!lua_typenumber(state,-2)) {
					ASSERT			(lua_typestring(state,-2), "invalid value in lua stack");
					pcstr string	= lua_tostring(state,-2);
					keys.push_back	(string);

					lua_pop			(state, 1);
					continue;
				}
				
				stream.msg			("%s",temp);
				save				(object().interpreter(),stream,indent,indent_size,temp,level+1);
				lua_pop				(state, 1);
			}

			std::sort				(keys.begin(),keys.end(),string_comparator);

			KEYS::const_iterator	I = keys.begin();
			KEYS::const_iterator	E = keys.end();
			for ( ; I != E; ++I) {
				lua_pushstring	(state,*I);
				lua_gettable	(state,-2);

				pcstr			string = *I;

				if (!sz_cmp(string,"__bases__")) {
					lua_pop		(state, 1);
					continue;
				}

				if (!sz_cmp(string,"__code__")) {
					lua_pop		(state, 1);
					continue;
				}

				if (valid_identifier(string))
					stream.msg	("%s%s = ",temp,string);
				else {
					if (!level)
						stream.msg	("%sthis[",temp,string);
					else
						stream.msg	("%s[",temp,string);

					process_string	(stream, string,(sz_len(string) + 1)*sizeof(char));
					stream.msg		("] = ",string);
				}

				save				(object().interpreter(),stream,indent,indent_size,temp,level+1);

				if (!sz_cmp(string,"__description__")) {
					stream.msg		("%s%s = function ()\r\n",temp,"__code__");
					pcstr string	= lua_tostring(state,-1);
					stream.msg		("%s%s\r\n",indent,string);
					stream.msg		("%send,\r\n",temp);
				}

				lua_pop				(state, 1);
			}

			sz_cpy					(indent,indent_size,temp);

			if (level)
				stream.msg			("%s}%s\r\n\r\n",previous_indent,(level > 1) ? "," : "");

			break;
		}

		case LUA_TNONE			: NODEFAULT;
		case LUA_TNIL			: NODEFAULT;
		case LUA_TFUNCTION		: NODEFAULT;
		case LUA_TLIGHTUSERDATA : NODEFAULT;
		case LUA_TUSERDATA		: NODEFAULT;
		case LUA_TTHREAD		: NODEFAULT;
		default					: NODEFAULT;
	}
}

void property_holder::save_as		(pcstr file_name)
{
	string_stream				stream;

	string1024					temp,temp2;
	sz_cpy						(temp, "");
	sz_cpy						(temp2, "");

	object().push				(object().interpreter());
	save						(object().interpreter(),stream,temp,sizeof(temp),temp,0);
	lua_pop						(object().interpreter(),1);

	m_world.engine().create_file(cs::script::engine::file_type_script,file_name,stream.buffer(),stream.buffer_size());
}

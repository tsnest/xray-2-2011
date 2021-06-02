////////////////////////////////////////////////////////////////////////////
//	Module 		: world_help.cpp
//	Created 	: 03.07.2005
//  Modified 	: 03.07.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class help 
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"
#include <cs/core/string_stream.h>

using cs::script::detail::world;

#if !defined(LUABIND_NO_ERROR_CHECKING)
#	include <luabind/config.hpp>

luabind::string to_string					(luabind::object const& o)
{
	if (luabind::type(o) == LUA_TSTRING) return luabind::object_cast<luabind::string>(o);
	lua_State* state	= o.interpreter();
	LUABIND_CHECK_STACK(state);

	string_stream		s;

	if (luabind::type(o) == LUA_TNUMBER)
	{
		s.msg			( "%g", luabind::object_cast<float>(o) );
		pstr const temp = (pstr)ALLOCA( s.buffer_size() + 1 );
		mem_copy		( temp, s.buffer(), s.buffer_size() );
		temp[s.buffer_size()] = 0;
		return			temp;
	}

	s.msg				( "<%s>\r\n", lua_typename(state, luabind::type(o)) );
	pstr const temp		= (pstr)ALLOCA( s.buffer_size() + 1 );
	mem_copy			( temp, s.buffer(), s.buffer_size() );
	temp[s.buffer_size()] = 0;
	return				temp;
}

void strreplaceall							(luabind::string& str, pcstr S, pcstr N)
{
	pcstr	A;
	int		S_len = sz_len(S);
	while ((A = sz_str(str.c_str(),S)) != 0)
		str.replace(A - str.c_str(),S_len,N);
}

luabind::string& process_signature			(luabind::string& str)
{
	strreplaceall	(str,"custom [","");
	strreplaceall	(str,"]","");
	strreplaceall	(str,"float","number");
	strreplaceall	(str,"lua_State*, ","");
	strreplaceall	(str," ,lua_State*","");
	return			str;
}

luabind::string member_to_string			(luabind::object const& e, pcstr function_signature)
{
	CS_UNREFERENCED_PARAMETER	( function_signature );

	using namespace luabind;
	lua_State* state = e.interpreter();
	LUABIND_CHECK_STACK(state);

	if ( type(e) == LUA_TFUNCTION)
	{
		e.push(state);
		detail::stack_pop p(state, 1);

		{
			if (lua_getupvalue(state, -1, 3) == 0) return to_string(e);
			detail::stack_pop p2(state, 1);
			if (lua_touserdata(state, -1) != reinterpret_cast<void*>(0x1337)) return to_string(e);
		}

		string_stream s;

		{
			lua_getupvalue(state, -1, 2);
			detail::stack_pop p2(state, 1);
		}

		{
			lua_getupvalue(state, -1, 1);
			detail::stack_pop p2(state, 1);
			//detail::method_rep* m = static_cast<detail::method_rep*>(lua_touserdata(state, -1));

			//for (luabind::vector<detail::overload_rep>::const_iterator i = m->overloads().begin();
			//	i != m->overloads().end(); ++i)
			//{
			//	luabind::string str;
			//	i->get_signature(state, str);
			//	if (i != m->overloads().begin())
			//		s.msg	( "\r\n" );

			//	s.msg		( "%s%s;", function_signature, process_signature(str) );
			//}
		}

		s.msg				( "\r\n" );
		pstr const temp		= (pstr)ALLOCA( s.buffer_size() + 1 );
		mem_copy			( temp, s.buffer(), s.buffer_size() );
		temp[s.buffer_size()] = 0;
		return				temp;
	}

    return to_string(e);
}

void list_classes						(lua_State* state, luabind::detail::class_rep* crep)
{
	luabind::string		S;
	// print class and bases
	{
		S				= (crep->get_class_type() != luabind::detail::class_rep::cpp_class) ? "LUA class " : "C++ class ";
		S.append		(crep->name());
		typedef luabind::vector<luabind::detail::class_rep::base_info> BASES;
		BASES const& bases = crep->bases();
		BASES::const_iterator	I = bases.begin(), B = I;
		BASES::const_iterator	E = bases.end();
		if (B != E)
			S.append	(" : ");
		for ( ; I != E; ++I) {
			if (I != B)
				S.append(",");
			S.append	((*I).base->name());
		}
		msg				(
			cs::message_initiator_script,
			cs::core::message_type_information,
			"%s {",
			S.c_str()
		);
	}
	// print class constants
	{
		luabind::detail::class_rep::STATIC_CONSTANTS const&	constants = crep->static_constants();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	I = constants.begin();
		luabind::detail::class_rep::STATIC_CONSTANTS::const_iterator	E = constants.end();
		for ( ; I != E; ++I)
			msg		(
				cs::message_initiator_script,
				cs::core::message_type_information,
				"    const %s = %d;",
				(*I).first,
				(*I).second
			);
		if (!constants.empty())
			msg		(
				cs::message_initiator_script,
				cs::core::message_type_information,
				"    "
			);
	}
	// print class properties
	{
		typedef luabind::map<char const*, luabind::detail::class_rep::callback, luabind::detail::ltstr> PROPERTIES;
		PROPERTIES const& properties = crep->properties();
		PROPERTIES::const_iterator	I = properties.begin();
		PROPERTIES::const_iterator	E = properties.end();
		for ( ; I != E; ++I)
			msg		(cs::message_initiator_script, cs::core::message_type_information,"    property %s;",(*I).first);
		if (!properties.empty())
			msg		(cs::message_initiator_script, cs::core::message_type_information,"    ");
	}
	// print class constructors
	{
		const luabind::vector<luabind::detail::construct_rep::overload_t>	&constructors = crep->constructors().overloads;
		luabind::vector<luabind::detail::construct_rep::overload_t>::const_iterator	I = constructors.begin();
		luabind::vector<luabind::detail::construct_rep::overload_t>::const_iterator	E = constructors.end();
		for ( ; I != E; ++I) {
			luabind::string S;
			(*I).get_signature(state,S);
			strreplaceall	(S,"custom [","");
			strreplaceall	(S,"]","");
			strreplaceall	(S,"float","number");
			strreplaceall	(S,"lua_State*, ","");
			strreplaceall	(S," ,lua_State*","");
			msg				(cs::message_initiator_script, cs::core::message_type_information,"    %s %s;",crep->name(),S.c_str());
		}
		if (!constructors.empty())
			msg				(cs::message_initiator_script, cs::core::message_type_information,"    ");
	}
	// print class methods
	{
		crep->get_table	(state);
		luabind::object	table(luabind::from_stack(state, -1));
		for (luabind::iterator i(table); i != luabind::iterator(); ++i) {
			luabind::object	object = *i;
			luabind::string	S;
			S			= "    function ";
			S.append	(to_string(i.key()).c_str());

			strreplaceall	(S,"function __add","operator +");
			strreplaceall	(S,"function __sub","operator -");
			strreplaceall	(S,"function __mul","operator* ");
			strreplaceall	(S,"function __div","operator /");
			strreplaceall	(S,"function __pow","operator ^");
			strreplaceall	(S,"function __lt","operator <");
			strreplaceall	(S,"function __le","operator <=");
			strreplaceall	(S,"function __gt","operator >");
			strreplaceall	(S,"function __ge","operator >=");
			strreplaceall	(S,"function __eq","operator ==");
			msg				(cs::message_initiator_script, cs::core::message_type_information,"%s",member_to_string(object,S.c_str()).c_str());
		}
	}
	msg						(cs::message_initiator_script, cs::core::message_type_information,"};");
	msg						(cs::message_initiator_script, cs::core::message_type_information," ");
}

void list_free_functions				(lua_State* state, luabind::object const& object, pcstr header, luabind::string const& indent)
{
	CS_UNREFERENCED_PARAMETER	( header );

	u32							count = 0;
	luabind::iterator		I(object);
	luabind::iterator		E;
	for ( ; I != E; ++I) {
		if (luabind::type(*I) != LUA_TFUNCTION)
			continue;
		
		//(*I).push			( );
		//luabind::detail::free_functions::function_rep* rep = 0;
		//if (lua_iscfunction(state, -1))
		//{
		//	if (lua_getupvalue(state, -1, 2) != 0)
		//	{
		//		// check the magic number that identifies luabind's functions
		//		if (lua_touserdata(state, -1) == (void*)0x1337)
		//		{
		//			if (lua_getupvalue(state, -2, 1) != 0)
		//			{
		//				if (!count) {
		//					msg	(cs::message_initiator_script, cs::core::message_type_information," ");
		//					msg	(cs::message_initiator_script, cs::core::message_type_information,"%snamespace %s {",indent.c_str(),header);
		//				}
		//				++count;
		//				rep = static_cast<luabind::detail::free_functions::function_rep*>(lua_touserdata(state, -1));
		//				luabind::vector<luabind::detail::free_functions::overload_rep>::const_iterator	i = rep->overloads().begin();
		//				luabind::vector<luabind::detail::free_functions::overload_rep>::const_iterator	e = rep->overloads().end();
		//				for ( ; i != e; ++i) {
		//					luabind::string	S;
		//					(*i).get_signature(state,S);
		//					msg	(cs::message_initiator_script, cs::core::message_type_information,"    %sfunction %s%s;",indent.c_str(),rep->name(),process_signature(S).c_str());
		//				}
		//				lua_pop(state, 1);
		//			}
		//		}
		//		lua_pop(state, 1);
		//	}
		//}
		//lua_pop(state, 1);
	}
	{
		luabind::string			_indent = indent;
		_indent.append			("    ");
		object.push				(state);
		lua_pushnil				(state);
		while (lua_next(state, -2) != 0) {
			if (lua_istable(state, -1)) {
				pcstr			S = lua_tostring(state, -2);
				if (sz_cmp("_G",S) && sz_cmp("package",S) && sz_cmp("_M",S)) {
					luabind::object		object(luabind::from_stack(state,-1));
					list_free_functions	(state,object,S,_indent);
					object.push			(state);
				}
			}

			lua_pop				(state, 1);
		}
	}
	if (count)
		msg	(cs::message_initiator_script, cs::core::message_type_information,"%s};",indent.c_str());
}

void world::list_exported_stuff			()
{
	msg						(cs::core::message_type_information," ");
	msg						(cs::core::message_type_information,"list of the classes exported to script");
	msg						(cs::core::message_type_information," ");

	luabind::detail::class_registry::get_registry(
		state()
	)->iterate_classes		(state(),&list_classes);

	msg						(cs::core::message_type_information,"end of list of the classes exported to script");
	msg						(cs::core::message_type_information," ");
	msg						(cs::core::message_type_information," ");
	msg						(cs::core::message_type_information,"list of the namespaces exported to script");

	list_free_functions		(state(),luabind::globals(state()),"","");

	msg						(cs::core::message_type_information," ");
	msg						(cs::core::message_type_information,"end of list of the namespaces exported to script");
	msg						(cs::core::message_type_information," ");
}
#else // !defined(LUABIND_NO_ERROR_CHECKING)
void world::list_exported_stuff			()
{
	msg						(cs::core::message_type_warning,"Cannot list exported stuff since LUABIND_NO_ERROR_CHECKING is defined");
}
#endif // !defined(LUABIND_NO_ERROR_CHECKING)
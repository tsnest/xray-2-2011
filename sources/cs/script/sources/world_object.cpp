////////////////////////////////////////////////////////////////////////////
//	Module 		: world_object.cpp
//	Created 	: 02.07.2005
//  Modified 	: 02.07.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class : object functionality
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"
#include <cs/script/callback.h>

using cs::script::detail::world;

bool world::table_loaded			(pcstr table_name)
{
#if CS_DEBUG_LIBRARIES
	int						start = lua_gettop(state());
#endif

	pcstr slash_found		= strchr(table_name, '/' );
	if ( !slash_found )
		slash_found			= strchr(table_name, '\\' );

	if ( slash_found ) {
		lua_pushstring		( state(), "modules" );
		lua_rawget 			( state(), LUA_GLOBALSINDEX ); 
		if ( lua_type(state(),-1) != LUA_TTABLE ) {
			lua_pop			( state(), 1 );
			return			false;
		}

		lua_pushstring		( state(), table_name );
		lua_rawget			( state(), -2 ); 
		if ( lua_type(state(),-1) != LUA_TTABLE ) {
			lua_pop			( state(), 2 );
			return			false;
		}

		lua_pop				( state(), 2 );
		return				true;
	}

	lua_pushstring 			(state(),"_G"); 
	lua_rawget 				(state(),LUA_GLOBALSINDEX); 

	string256				S2;
	sz_cpy					(S2, table_name);
	pstr					S = S2;
	for (;;) { 
		if (!sz_len(S))
			return			false; 

		pstr				S1 = strchr(S,'.'); 
		if (S1)
			*S1				= 0; 

		lua_pushstring 		(state(),S); 
		lua_rawget 			(state(),-2); 
		if (lua_isnil(state(),-1)) { 
			ASSERT			(lua_gettop(state()) >= 2, "unexpected lua stack state");
			lua_pop			(state(),2); 
			ASSERT			(start == lua_gettop(state()), "unexpected lua stack state");
			return			false;	//	there is no table!
		}
		else 
			if (!lua_istable(state(),-1)) { 
				ASSERT		(lua_gettop(state()) >= 1, "unexpected lua stack state");
				lua_pop		(state(),1); 
				ASSERT		(start == lua_gettop(state()), "unexpected lua stack state");
				FATAL		(" Error : table name has already been used by non-table object!\n");
				return		false; 
			} 
			lua_remove		(state(),-2); 
			if (S1)
				S			= ++S1; 
			else
				break; 
	} 

	ASSERT					(lua_gettop(state()) >= 1, "unexpected lua stack state");
	lua_pop					(state(),1);
	ASSERT					(lua_gettop(state()) == start, "unexpected lua stack state");
	return					true; 
}

void world::split_table_object		(pcstr object_description, pstr table_name, u32 table_name_size, pstr object_name, u32 object_name_size)
{
	pcstr				I = object_description, J = 0;
	for ( ; ; J=I,++I) {
		I				= strchr(I,'.');
		if (!I)
			break;
	}

	sz_cpy				(table_name,table_name_size,"_G");

	if (!J)
		sz_cpy			(object_name,object_name_size,object_description);
	else {
		mem_copy	(table_name,object_description, u32(J - object_description)*sizeof(char));
		table_name[u32(J - object_description)] = 0;
		sz_cpy			(object_name,object_name_size,J + 1);
	}
}

luabind::object world::object		(pcstr object_description, luabind::object table_object)
{
	string256			S1;
	sz_cpy				(S1, object_description);
	pstr				S = S1;
	for (;;) {
		if (!sz_len(S))
			return		table_object;

		pstr			I = strchr(S,'.');
		if (!I)
			return		table_object[S];

		*I				= 0;
		table_object	= table_object[S];
		S				= I + 1;
	}
}

bool world::object					(pcstr object_description, luabind::object& result, int object_type)
{
	result				= luabind::globals(state());

	u32					length = sz_len(object_description);
	pstr				S1 = (pstr)ALLOCA((length + 1)*sizeof(char));
	sz_cpy				(S1, length + 1, object_description);
	pstr				S = S1;
	for (;;) {
		if (!sz_len(S)) {
			if (luabind::type(result) != object_type)
				return	false;

			return		true;
		}

		if (luabind::type(result) != LUA_TTABLE)
			return		false;

		pstr			I = strchr(S,'.');
		if (!I) {
			result		= result[S];
			*S			= 0;
			continue;
		}

		*I				= 0;
		result			= result[S];
		S				= I + 1;
	}
}
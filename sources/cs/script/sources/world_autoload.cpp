////////////////////////////////////////////////////////////////////////////
//	Module 		: world_autoload.cpp
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class : autoload functionality
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"

using cs::script::detail::world;

static pcstr const modules_auto_load_meta_table	= "__cs_script_modules_auto_load_meta_table__";

int world::auto_load				(lua_State* L)
{
	if ((lua_gettop(L) < 2) || !lua_istable(L,-2) || !lua_typestring(L,-1)) {
		lua_pushnil					(L);
		return						1;
	}

	lua_world(L).process_file_internal	(lua_tostring(L,-1),false);
	lua_rawget						(L,-2);
	return							1;
}

void world::setup_auto_load			()
{
	lua_pushstring 					(state(), "modules");
	lua_newtable					(state());
	lua_settable 					(state(), LUA_GLOBALSINDEX); 

	lua_getfield 					(state(), LUA_GLOBALSINDEX, "modules"); 

	luaL_newmetatable				(state(), modules_auto_load_meta_table);
	lua_pushstring					(state(), "__index");
	lua_pushcfunction				(state(), auto_load);
	lua_settable					(state(), -3);

	lua_setmetatable				(state(), -2);

	lua_pop							(state(), 1);
}
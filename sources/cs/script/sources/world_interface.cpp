////////////////////////////////////////////////////////////////////////////
//	Module 		: world.cpp
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"
#include "thread.h"
#include "property_holder.h"
#include <cs/lua_studio_backend/interfaces.h>

#if CS_USE_LUAJIT
#	include <luajit/opt.lua.h>
#	include <luajit/opt_inline.lua.h>
#endif // #if CS_USE_LUAJIT

using cs::script::detail::world;
using cs::script::detail::thread;
using cs::script::detail::property_holder;

#if CS_USE_LUAJIT
/* ---- start of LuaJIT extensions */
static void l_message (lua_State* state, char const* msg) {
	CS_UNREFERENCED_PARAMETER	( state );
	::msg	(cs::message_initiator_script, cs::core::message_type_error, "%s", msg);
}

static int report (lua_State* L, int status) {
  if (status && !lua_isnil(L, -1)) {
    char const* msg = lua_tostring(L, -1);
    if (msg == NULL) msg = "(error object is not a string)";
    l_message(L, msg);
    lua_pop(L, 1);
  }
  return status;
}

static int loadjitmodule (lua_State* L, char const* notfound) {
  lua_getglobal(L, "require");
  lua_pushliteral(L, "jit.");
  lua_pushvalue(L, -3);
  lua_concat(L, 2);
  if (lua_pcall(L, 1, 1, 0)) {
    char const* msg = lua_tostring(L, -1);
    if (msg && !strncmp(msg, "module ", 7)) {
      l_message(L, notfound);
      return 1;
    }
    else
      return report(L, 1);
  }
  lua_getfield(L, -1, "start");
  lua_remove(L, -2);  /* drop module table */
  return 0;
}

/* JIT engine control command: try jit library first or load add-on module */
static int dojitcmd (lua_State* L, char const* cmd) {
  char const* val = strchr(cmd, '=');
  lua_pushlstring(L, cmd, val ? val - cmd : sz_len(cmd));
  lua_getglobal(L, "jit");  /* get jit.* table */
  lua_pushvalue(L, -2);
  lua_gettable(L, -2);  /* lookup library function */
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 2);  /* drop non-function and jit.* table, keep module name */
    if (loadjitmodule(L, "unknown luaJIT command"))
      return 1;
  }
  else {
    lua_remove(L, -2);  /* drop jit.* table */
  }
  lua_remove(L, -2);  /* drop module name */
  if (val) lua_pushstring(L, val+1);
  return report(L, lua_pcall(L, val ? 1 : 0, 0, 0));
}

/* start optimizer */
static int dojitopt (lua_State* L, char const* opt) {
  lua_pushliteral(L, "opt");
  if (loadjitmodule(L, "LuaJIT optimizer module not installed"))
    return 1;
  lua_remove(L, -2);  /* drop module name */
  if (*opt) lua_pushstring(L, opt);
  return report(L, lua_pcall(L, *opt ? 1 : 0, 0, 0));
}
/* ---- end of LuaJIT extensions */
#endif // #if CS_USE_LUAJIT

template <int n>
static void put_function									(lua_State* const state, u8 const (&buffer)[n], pcstr id)
{
	lua_getglobal	(state, "package");
	lua_getfield	(state, -1, "preload");
	lua_pushstring	(state, id);
	luaL_loadbuffer	(state, (pcstr)buffer, sizeof(buffer), id);
	lua_settable	(state, -3);
}

void world::jit_optimize									(char const* command)
{
#if CS_USE_LUAJIT
	put_function				(state(), opt_binary,			"jit.opt");
	put_function				(state(), opt_inline_binary,	"jit.opt_inline");
	dojitopt					(state(), command);
#else // #if CS_USE_LUAJIT
	CS_UNREFERENCED_PARAMETER	( command );
#endif // #if CS_USE_LUAJIT
}

void world::jit_command										(char const* command)
{
#if CS_USE_LUAJIT
	dojitcmd					(state(),command);
#else // #if CS_USE_LUAJIT
	CS_UNREFERENCED_PARAMETER	( command );
#endif // #if CS_USE_LUAJIT
}

lua_State* world::virtual_machine							() const
{
	return						state();
}

bool world::luabind_object									(
		pcstr object_description,
		luabind::object& result,
		int object_type
	)
{
	return						object(object_description,result,object_type);
}

void world::execute_script									(pcstr file_name, bool force_to_reload)
{
	process_file				(file_name,force_to_reload);
}

cs::script::thread* world::create_thread		(char const* const& string_to_execute)
{
	return						cs_new< ::thread >(this, string_to_execute);
}

void CS_SCRIPT_CALL world::destroy_thread	(cs::script::thread*& thread)
{
	cs_delete					((::thread*&)thread);
}

cs::script::property_holder* CS_SCRIPT_CALL world::create_property_holder	(char const* file_name, u8 const flags)
{
	return						cs_new< ::property_holder >(this, file_name, flags);
}

void CS_SCRIPT_CALL world::destroy_property_holder							(cs::script::property_holder* &instance)
{
	cs_delete					(instance);
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: thread.cpp
//	Created 	: 24.06.2005
//  Modified 	: 24.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script thread class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "thread.h"
#include "world.h"

using cs::script::detail::thread;
using cs::script::detail::world;

static pcstr unique_function_name = "execute_string_main_thread_function";

#define LUABIND_H_INCLUDEDAS_BUGS_WITH_LUA_THREADS

thread::thread							(world* world, pcstr string_to_execute) :
	m_world					(*world)
{
	ASSERT					(string_to_execute, "null string passed");

	m_thread_environment	= 0;
	m_suspended				= false;
	m_id					= string_to_execute;

	construct_internal		();
}

thread::~thread							()
{
	if (!suspended())
		return;

	unreference				();
}

void thread::unreference							()
{
	ASSERT					(suspended(), "we are not suspended");

#ifndef LUABIND_H_INCLUDEDAS_BUGS_WITH_LUA_THREADS
	luaL_unref				(m_virtual_machine,LUA_REGISTRYINDEX,lua_reference());
#endif // #ifndef LUABIND_H_INCLUDEDAS_BUGS_WITH_LUA_THREADS
}

bool thread::suspended							()
{
	return					m_suspended;
}

void thread::construct_internal					()
{
	u32						script_body_length = m_id.size() + 256;
	u32						script_body_length_full = script_body_length*sizeof(char);
	pstr					script_body = (pstr)ALLOCA(script_body_length_full);
	cs::sprintf				(script_body,script_body_length_full,"function %s() %s end",unique_function_name,*m_id);

	if (m_world.do_string(m_world.state(),script_body,*m_id))
		return;

	m_thread_environment	= lua_newthread(m_world.state());
	ASSERT					(thread_environment(),"cannot create new Lua thread");
	m_lua_reference			= luaL_ref(m_world.state(),LUA_REGISTRYINDEX);

	world::on_thread_created		(thread_environment());

	cs::sprintf				(script_body,script_body_length_full,"%s()",unique_function_name);

	int						error_code = luaL_loadbuffer(
		thread_environment(),
		script_body,
		sz_len(script_body),
		"@_thread_main"
	);

	if (error_code) {
		world::process_error		(thread_environment(),*m_id,error_code);
		world::on_thread_finished	(thread_environment());
		return;
	}

	m_suspended				= true;
}

void thread::resume								()
{
	ASSERT					(suspended(),"cannot resume dead script thread");
	resume_internal			();
}

void thread::resume_internal						()
{
	int								return_code = lua_resume(thread_environment(),0);
	
	if (return_code == LUA_YIELD) {
		m_suspended					= false;
#if CS_DEBUG_LIBRARIES
		m_world.msg					(cs::core::message_type_information,"script %s is finished!",*id());
#endif
		return;
	}

	if (return_code) {
		world::process_error		(thread_environment(),*id(),return_code);
		world::on_thread_finished	(thread_environment());
		m_suspended					= false;
		return;
	}

	m_world.msg						(cs::core::message_type_warning,"do not pass any value to coroutine.yield()!");
}

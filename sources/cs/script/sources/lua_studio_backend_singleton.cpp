////////////////////////////////////////////////////////////////////////////
//	Module 		: lua_studio_backend_singleton.cpp
//	Created 	: 12.04.2008
//  Modified 	: 12.04.2008
//	Author		: Dmitriy Iassenev
//	Description : lua studio backend singleton class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "lua_studio_backend_singleton.h"
#include "engine.h"
#include <cs/core/os_include.h>

#ifndef _MSC_VER
	typedef pvoid HMODULE;
#endif // #ifndef _MSC_VER

typedef cs::lua_studio::create_world_function_type			create_world_function_type;
typedef cs::lua_studio::destroy_world_function_type			destroy_world_function_type;
typedef cs::lua_studio::memory_allocator_function_type		memory_allocator_function_type;

static create_world_function_type		s_create_world				= 0;
static destroy_world_function_type		s_destroy_world				= 0;
static memory_allocator_function_type	s_memory_allocator			= 0;
static HMODULE							s_lua_studio_backend_handle	= 0;
static engine							s_engine;

void lua_studio_backend_singleton::cleanup					()
{
	if (s_lua_studio_backend_handle == 0)
		return;

	s_destroy_world					(m_world);

#ifdef _MSC_VER
	FreeLibrary						(s_lua_studio_backend_handle);
#endif // #ifdef _MSC_VER
	s_lua_studio_backend_handle		= 0;

	s_create_world					= 0;
	s_destroy_world					= 0;
	s_memory_allocator				= 0;
}

lua_studio_backend_singleton::lua_studio_backend_singleton	() :
	m_world							(0)
{
}

lua_studio_backend_singleton::~lua_studio_backend_singleton	()
{
	if (!m_world)
		return;

	cleanup							();
}

void lua_studio_backend_singleton::construct				()
{
	msg								(
		cs::message_initiator_script,
		cs::core::message_type_information,
		"[DLL] Loading %s",
		CS_LUA_STUDIO_BACKEND_FILE_NAME
	);

#ifdef _MSC_VER
	s_lua_studio_backend_handle		= LoadLibrary(CS_LUA_STUDIO_BACKEND_FILE_NAME);
	if (!s_lua_studio_backend_handle)
		R_CAPI						(GetLastError());

	R_ASSERT						(s_lua_studio_backend_handle, "can't load lua studio backend library");

	s_create_world					= (create_world_function_type)
		GetProcAddress(
			s_lua_studio_backend_handle,
			"_cs_lua_studio_backend_create_world@12"
		);
	R_ASSERT						(s_create_world, "can't find function \"create_world\"");

	s_destroy_world					= (destroy_world_function_type)
		GetProcAddress(
			s_lua_studio_backend_handle,
			"_cs_lua_studio_backend_destroy_world@4"
		);
	R_ASSERT						(s_destroy_world, "can't find function \"destroy_world\" in the library");

	s_memory_allocator				= (memory_allocator_function_type)
		GetProcAddress(
			s_lua_studio_backend_handle,
			"_cs_lua_studio_backend_memory_allocator@8"
		);
	R_ASSERT						(s_memory_allocator, "can't find function \"memory_allocator\" in the library");
#endif // #ifdef _MSC_VER
}

cs::lua_studio::world& lua_studio_backend_singleton::world	()
{
	if (m_world)
		return						*m_world;

	construct						();
	
	memory.switch_mt_alloc			();
	s_memory_allocator				( cs::core::memory_allocator(), cs::core::memory_allocator_parameter() );

	m_world							= s_create_world( s_engine, false, false );
	ASSERT							(m_world);

	return							*m_world;
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: world.cpp
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "world.h"
#include "engine.h"
#include "lua_studio_backend_singleton.h"

extern "C" {
#	include <cs/lua/lualib.h>
} // extern "C"

using cs::script::detail::world;

static world*						s_head		= 0;
static lua_studio_backend_singleton	s_lua_studio_backend;
static bool							s_use_lua_studio_backend = false;

void cs::script::detail::cleanup							()
{
	if (!s_head)
		return;

	u32								i = 0;
	for (world*	temp = s_head; temp; temp = temp->m_next_world, ++i);
	msg								(
		cs::message_initiator_script,
		cs::core::message_type_warning,
		"leaked %d lua states",
		i
	);

	while (s_head) {
		cs::script::world*		temp = s_head;
		cs_script_destroy_world	(temp);
	}
}

cs::script::world* CS_SCRIPT_CALL cs_script_create_world	(cs::script::engine& engine)
{
	if (s_head)
		return					cs_new< ::world >(&engine);

	cs::core::core.initialize	( "script", engine.use_debug_engine(), engine.use_logging() );
	if ( !luabind::allocator ) {
		luabind::allocator				= cs::core::memory_allocator();
		luabind::allocator_parameter	= cs::core::memory_allocator_parameter();
	}

	world*						result = cs_new< ::world >(&engine);
	msg							(
		cs::message_initiator_script,
		cs::core::message_type_information,
		"initialized successfully"
	);
	return						result;
}

void CS_SCRIPT_CALL cs_script_destroy_world					(cs::script::world*& world)
{
	cs_delete					((::world*&)world);

	if (s_head)
		return;

	s_lua_studio_backend.cleanup();
	cs::core::core.uninitialize	();
}

void CS_SCRIPT_CALL cs_lua_studio_backend_init				()
{
	s_use_lua_studio_backend					= true;

	for (::world* current = s_head; current; current = current->next_world())
		current->init_lua_studio_backend		();

//	s_lua_studio_backend.world().add_log_line	("");
}

void CS_SCRIPT_CALL	cs_lua_studio_backend_add_log_line		(char const* log_line)
{
	if (!s_use_lua_studio_backend)
		return;

	static bool entered			= false;
	if (entered)
		return;

	entered						= true;

	s_lua_studio_backend.world().add_log_line	(log_line);

	entered						= false;
}

world::world							(cs::script::engine* const& engine) :
	m_engine					(*engine),
	m_previous_world			(0),
	m_next_world				(s_head),
	m_printing					(false),
	m_printing_stack			(false)
{
	if (s_head)
		s_head->m_previous_world= this;

	s_head						= this;
	m_state_reload				= false;

#if CS_DEBUG_LIBRARIES
	m_current_thread			= 0;
#endif // #if CS_DEBUG_LIBRARIES

	struct allocator {
		static pvoid allocate			(void* ud, void* ptr, size_t osize, size_t nsize)
		{
			CS_UNREFERENCED_PARAMETER	( ud );
			CS_UNREFERENCED_PARAMETER	( osize );
#if CS_DEBUG_LIBRARIES
			return				cs_realloc( ptr, nsize, "Lua");
#else // #if CS_DEBUG_LIBRARIES
			return				luabind::allocator( luabind::allocator_parameter, ptr, nsize );
#endif // #if CS_DEBUG_LIBRARIES
		}
	}; // struct allocator

	m_state						= lua_newstate(allocator::allocate,0);
	R_ASSERT					(m_state,"Cannot create LUA state");
	construct					();
}

world::~world							()
{
	if (s_use_lua_studio_backend)
		s_lua_studio_backend.world().remove	(m_state);

	ASSERT				(m_state, "null lua state");
	lua_close			(m_state);
	m_state				= 0;

	ASSERT				(s_head);

	if (s_head == this) {
		ASSERT			(!m_previous_world);
		s_head			= m_next_world;
		return;
	}

	ASSERT								(m_previous_world);
	m_previous_world->m_next_world		= m_next_world;
	if (m_next_world)
		m_next_world->m_previous_world	= m_previous_world;
}

void world::script_export				(lua_State* state)
{
	::msg							(
		cs::message_initiator_script,
		cs::core::message_type_information,
		"exporting classes to script..."
	);

	struct log {
		static int	predicate	(lua_State* state)
		{
			ASSERT		(lua_typestring(state, -1));
			lua_world(state).msg(
				cs::core::message_type_output,
				"%s",
				lua_tostring(state, -1)
			);
			return		0;
		}
	}; // struct log

	lua_register				(state, "log", &log::predicate);
	luabind::module(state)
	[
		luabind::namespace_("cs")[ luabind::namespace_("script")[ luabind::namespace_("classes")[
			luabind::class_<world>("world")
		]]]
	];
}

//#define TEST_PROPERTY_H_INCLUDEDOLDER

#ifdef TEST_PROPERTY_H_INCLUDEDOLDER
#	include "property_holder.h"
#endif // #ifdef TEST_PROPERTY_H_INCLUDEDOLDER

#ifdef TEST_PROPERTY_H_INCLUDEDOLDER
static void print_lua_stack				( lua_State* const state )
{
	for ( int i = -1, start = lua_gettop( state ); i >= -start; --i ) {
		pcstr type		= 0;
		switch ( lua_type( state, i) ) {
			case LUA_TNONE			: type = "<none>"; break;
			case LUA_TNIL			: type = "<nil>"; break;
			case LUA_TBOOLEAN		: type = "<boolean>"; break;
			case LUA_TLIGHTUSERDATA	: type = "<light user data>"; break;
			case LUA_TNUMBER		: type = "<number>"; break;
			case LUA_TSTRING		: type = "<string>"; break;
			case LUA_TTABLE			: type = "<table>"; break;
			case LUA_TFUNCTION		: type = "<function>"; break;
			case LUA_TUSERDATA		: type = "<user data>"; break;
			case LUA_TTHREAD		: type = "<thread>"; break;
			default					: NODEFAULT;
		}

		msg				(
			cs::message_initiator_script,
			cs::core::message_type_information,
			"[%s][0x%08x]", type, lua_topointer ( state, i )
		);
	}
}

struct test_bool {
	world&				m_world;
	property_holder		m_holder;

				test_bool	( world* world) :
		m_world			( *world ),
		m_holder		( world, "test_section", property_holder::do_not_convert )
	{
		print_lua_stack		( m_world.state( ) );
		m_holder.w_bool		( "new_mega_section", "new_mega_bool", true);
		print_lua_stack		( m_world.state( ) );
		m_holder.r_bool		( "new_mega_section", "new_mega_bool");
		print_lua_stack		( m_world.state( ) );
//		m_holder.save_as	( "new_mega_section");
	}

	bool get_value			( pcstr const section_id, pcstr const value_id) const
	{
		return				m_holder.r_bool( section_id, value_id ) ; 
	}
}; // struct test_bool
#endif // #ifdef TEST_PROPERTY_H_INCLUDEDOLDER

void world::construct					()
{
	struct lua {
		static void open_lib	(lua_State* L, pcstr module_name, lua_CFunction function)
		{
			lua_pushcfunction	(L, function);
			lua_pushstring		(L, module_name);
			lua_call			(L, 1, 0);
		}
	}; // struct lua;

	lua::open_lib		(state(),	"",					luaopen_base);
	lua::open_lib		(state(),	LUA_LOADLIBNAME,	luaopen_package);
	lua::open_lib		(state(),	LUA_TABLIBNAME,		luaopen_table);
#if CS_PLATFORM_WINDOWS
	lua::open_lib		(state(),	LUA_IOLIBNAME,		luaopen_io);
	lua::open_lib		(state(),	LUA_OSLIBNAME,		luaopen_os);
#endif // #if CS_PLATFORM_WINDOWS
	lua::open_lib		(state(),	LUA_MATHLIBNAME,	luaopen_math);
	lua::open_lib		(state(),	LUA_STRLIBNAME,		luaopen_string);
#if CS_DEBUG_LIBRARIES
	lua::open_lib		(state(),	LUA_DBLIBNAME,		luaopen_debug);
#endif // #if CS_DEBUG_LIBRARIES
#if CS_USE_LUAJIT
	lua::open_lib		(state(),	LUA_JITLIBNAME,		luaopen_jit);
#endif // #if CS_USE_LUAJIT

	if (s_use_lua_studio_backend)
		init_lua_studio_backend	();
#if CS_USE_LUAJIT
	else
		jit_optimize	("2");
#endif // #if CS_USE_LUAJIT

	luabind::open		(state());
	luabind::disable_super_deprecation();

	setup_callbacks		();
	setup_auto_load		();
	script_export		(state());

#ifdef TEST_PROPERTY_H_INCLUDEDOLDER
	luabind::module( state( ) ) [
		luabind::class_<test_bool>("test_bool")
			.def(luabind::constructor<world*>( ) )
			.def( "get_value", &test_bool::get_value )
	];
#endif // #ifdef TEST_PROPERTY_H_INCLUDEDOLDER

	luabind::globals( state() )["cs"]["script"]["world"] = this;

	execute_script		("_G");

#ifdef TEST_PROPERTY_H_INCLUDEDOLDER
//	print_lua_stack		( state( ) );
//	property_holder		test_section( this, "test_section");
//	test_section.r_bool	("section", "bool_value");
//	print_lua_stack		( state( ) );
#endif // #ifdef TEST_PROPERTY_H_INCLUDEDOLDER
}

int	world::msg							(cs::core::message_type const& message_type, pcstr format,...)
{
	if (!format)
		return			0;

	if (!*format)
		return			0;

	va_list				mark;
	va_start			(mark, format);
	int					result = vsmsg(cs::message_initiator_script, message_type, format, mark);
	va_end				(mark);

	if (!m_printing) {
		m_printing		= true;
		if (cs::core::message_type_error == message_type)
			print_stack	(cs::core::message_type_error);
		m_printing		= false;
	}

	return				result;
}

void world::log							(lua_State* state, pcstr string)
{
	lua_world(state).msg(cs::core::message_type_output,string);
}

bool world::execute_script				(char const* buffer, u32 buffer_size, char const* table_name)
{
	int								start = lua_gettop(state());

	string256						table_name_description;
	sz_concat						(table_name_description, "@", table_name);

	if (!process_buffer(state(),(pcstr)buffer,buffer_size,table_name_description,table_name,false)) {
		lua_settop					(state(),start);
		return						false;
	}

	int								error_code = luabind::detail::pcall(state(),0,0);
	
	if (!error_code)
		return						true;

	process_script_output			(state(),table_name,error_code);
	lua_settop						(state(),start);
	return							false;
}

void world::process_error				(lua_State* state, pcstr file_name, int const error_code)
{
	process_script_output			(state, file_name, error_code);
	if (!s_use_lua_studio_backend)
		return;

	s_lua_studio_backend.world().on_error	(state);
}

struct string_predicate {
	pcstr							m_value;

	inline		string_predicate		(pcstr value)
	{
		m_value						= value;
	}

	inline bool operator()				(pcstr object) const
	{
		return						!sz_cmp(m_value,object);
	}
};

bool world::previous_not_exist_query	(pcstr file_name)
{
	return							(
		std::find_if(
			m_not_exist_files.begin(),
			m_not_exist_files.end(),
			string_predicate(file_name)
		) == 
		m_not_exist_files.end()
	);
}

void world::on_file_no_exist			(pcstr file_name)
{
	NOT_EXIST_FILES::iterator	I = 
		std::find_if(
			m_not_exist_files.begin(),
			m_not_exist_files.end(),
			string_predicate(file_name)
		);

	if (I != m_not_exist_files.end())
		m_not_exist_files.erase	(I);
#if CS_DEBUG_LIBRARIES
	else {
		if (!s_use_lua_studio_backend || !s_lua_studio_backend.world().evaluating_watch()) {
			msg					(
				cs::core::message_type_warning,
				"you are trying to use undefined value: %s",
				file_name
			);
			print_stack			(cs::core::message_type_warning);
		}
	}
#endif // #if CS_DEBUG_LIBRARIES

	m_not_exist_files.insert	(m_not_exist_files.begin(),file_name);
}

void world::on_thread_created		(lua_State* state)
{
	if (!s_use_lua_studio_backend)
		return;

	s_lua_studio_backend.world().add	(state);
}

void world::on_thread_finished		(lua_State* state)
{
	if (!s_use_lua_studio_backend)
		return;

	s_lua_studio_backend.world().remove	(state);
}

void world::init_lua_studio_backend	()
{
	ASSERT						(s_use_lua_studio_backend);
	s_lua_studio_backend.world().add		(m_state);
#if CS_USE_LUAJIT
	jit_command					("debug=2");
	jit_command					("off");
#endif // #if CS_USE_LUAJIT
}

////////////////////////////////////////////////////////////////////////////
//	Module 		: world.h
//	Created 	: 15.06.2005
//  Modified 	: 15.06.2005
//	Author		: Dmitriy Iassenev
//	Description : script engine class
////////////////////////////////////////////////////////////////////////////

#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

struct lua_State;

namespace luabind {
	namespace detail {
		class class_rep;
	}
}

namespace cs {
namespace script {

namespace detail {

#if CS_DEBUG_LIBRARIES
	class thread;
#endif // #if CS_DEBUG_LIBRARIES

class world :
	public cs::script::world,
	private boost::noncopyable
{
private:
	friend void cleanup ();

private:
	static	int						on_panic				(lua_State* state);
	static	int 					on_pcall_failed			(lua_State* state);

public:
	static	void					on_thread_created		(lua_State* state);
	static	void					on_thread_finished		(lua_State* state);

private:
#ifdef LUABIND_NO_EXCEPTIONS
	static	void					on_error				(lua_State* state);
	static	void					on_cast_failed			(lua_State* state, LUABIND_TYPE_INFO info);
#endif // #ifdef LUABIND_NO_EXCEPTIONS
			void					print_stack				(cs::core::message_type const& message_type);

private:
	static	int						save_chunk				(lua_State* state, void const* p, u32 size, void* u);
	static	char const*				load_chunk				(lua_State* L, void* ud, size_t* size);

private:
			void					split_table_object		(pcstr object_description, pstr table_name, u32 table_name_size, pstr object_name, u32 object_name_size);
			bool					object					(pcstr object_description, luabind::object& result, int object_type);
	template <typename T>
	inline	bool					functor					(pcstr functor_description, luabind::object& result);

private:
	static	bool					generate_this_string	(lua_State* state, pcstr table_name, pstr left_part, u32 left_size, pstr right_part, u32 right_size);

public:
			bool					do_string				(lua_State* state, pcstr string_to_do, pcstr id);
	static	bool					process_buffer			(lua_State* state, pcstr buffer, u32 buffer_size, pcstr file_name);

private:
	static	bool					process_buffer			(lua_State* state, pcstr buffer, u32 buffer_size, pcstr file_name, pcstr table_name, bool chunk);
			bool					execute_file_in_table	(pcstr file_name, pcstr table_name);
			bool					execute_file			(pcstr file_name);
			void					process_file_internal	(pcstr file_name, bool assert_if_not_exist = true);
			void					process_file			(pcstr file_name, bool state_reload = false);

public:
	static	pcstr					get_error_message		(int error_code);

private:
	static	void					log						(lua_State* state, pcstr string);
	static	void					error_message			(lua_State* state, int error_code);

public:
	static	bool					process_script_output	(lua_State* state, pcstr file_name, int error_code);

private:
	static	int						auto_load				(lua_State* state);
			void					setup_auto_load			();

private:
			void					setup_callbacks			();

public:
			int						msg						(cs::core::message_type const& message_type, pcstr format,...);
			bool					table_loaded			(pcstr table_name);
	static	luabind::object			object					(pcstr object_description, luabind::object table_object);

public:
									world					(cs::script::engine* const& engine);
	virtual							~world					();
	virtual	void					construct				();
			void					init_lua_studio_backend	();
	inline	lua_State*				state					() const;
	inline	cs::script::engine&		engine					() const;

public:
	virtual	lua_State*	CS_SCRIPT_CALL	virtual_machine		() const;
	virtual	bool		CS_SCRIPT_CALL	luabind_object		(pcstr object_description, luabind::object& result, int object_type);
	virtual void		CS_SCRIPT_CALL	execute_script		(pcstr file_name, bool force_to_reload = false);
	virtual bool		CS_SCRIPT_CALL	execute_script		(char const* buffer, u32 buffer_size, char const* table_name);
	virtual bool		CS_SCRIPT_CALL	load_string			(pcstr string_to_do, pcstr id);
	virtual void		CS_SCRIPT_CALL	list_exported_stuff	();
	virtual void		CS_SCRIPT_CALL	jit_optimize		(char const* command);
	virtual void		CS_SCRIPT_CALL	jit_command			(char const* command);

public:
	typedef cs::script::property_holder	property_holder_type;

public:
	virtual property_holder_type* CS_SCRIPT_CALL create_property_holder	(char const* file_name, u8 flags);
	virtual void CS_SCRIPT_CALL					 destroy_property_holder(cs::script::property_holder* &instance);

public:
	virtual	cs::script::thread* CS_SCRIPT_CALL create_thread(char const* const& string_to_execute);
	virtual	void CS_SCRIPT_CALL		destroy_thread			(cs::script::thread*& thread);

#if CS_DEBUG_LIBRARIES
public:
	inline	void					current_thread			(thread* thread);
	inline	thread*					current_thread			() const;
#endif // #if CS_DEBUG_LIBRARIES

private:
			bool					previous_not_exist_query(pcstr file_name);
			void					on_file_no_exist		(pcstr file_name);
	static	void					script_export			(lua_State* state);

public:
	static	void					process_error			(lua_State* state, pcstr file_name, int const error_code);
			world* const&			next_world				() const;

private:
	typedef cs_deque<pcstr>			NOT_EXIST_FILES;

private:
	NOT_EXIST_FILES					m_not_exist_files;
	world*							m_previous_world;
	world*							m_next_world;
	cs::script::engine&				m_engine;
	lua_State*						m_state;
#if CS_DEBUG_LIBRARIES
	cs::script::detail::thread*		m_current_thread;
#endif // #if CS_DEBUG_LIBRARIES
	bool							m_state_reload;
	bool							m_printing;
	bool							m_printing_stack;
};

} // namespace detail
} // namespace script
} // namespace cs

#include "world_inline.h"

#endif // #ifndef WORLD_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Module 		: world.h
//	Created 	: 15.05.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : script world interface
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_SCRIPT_ENGINE_H_INCLUDED
#define CS_SCRIPT_SCRIPT_ENGINE_H_INCLUDED

#include <cs/script/interfaces.h>

struct lua_State;

namespace luabind {
	namespace adl {
		class object;
	}
	using adl::object;

	template<class T, class ValueWrapper>
	inline T object_cast(ValueWrapper const& value_wrapper);
} // namespace luabind

namespace cs {
namespace script {

struct thread;
struct engine;
struct property_holder;

struct DECLSPEC_NOVTABLE world {
	virtual	lua_State*		CS_SCRIPT_CALL	virtual_machine			() const = 0;
	virtual	bool			CS_SCRIPT_CALL	luabind_object			(char const* object_description, luabind::object& result, int object_type) = 0;
	virtual void			CS_SCRIPT_CALL	execute_script			(char const* file_name, bool force_to_reload = false) = 0;
	virtual bool			CS_SCRIPT_CALL	execute_script			(char const* buffer, unsigned int buffer_size, char const* table_name) = 0;
	virtual bool			CS_SCRIPT_CALL	load_string				(char const* string_to_do, char const* id) = 0;
	virtual void			CS_SCRIPT_CALL	list_exported_stuff		() = 0;
	virtual void			CS_SCRIPT_CALL	jit_optimize			(char const* command) = 0;
	virtual void			CS_SCRIPT_CALL	jit_command				(char const* command) = 0;
	virtual	thread*			CS_SCRIPT_CALL	create_thread			(char const* const& string_to_execute) = 0;
	virtual	void			CS_SCRIPT_CALL	destroy_thread			(thread*& script_thread) = 0;
	virtual	property_holder*CS_SCRIPT_CALL	create_property_holder	(char const* file_name, unsigned char flags) = 0;
	virtual	void			CS_SCRIPT_CALL	destroy_property_holder	(property_holder*& property_holder) = 0;

protected:
	CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( world )
}; // struct world

} // namespace script
} // namespace cs

extern "C" {
	CS_SCRIPT_API	cs::script::world*	CS_SCRIPT_CALL	cs_script_create_world	(cs::script::engine& engine);
	CS_SCRIPT_API	void				CS_SCRIPT_CALL	cs_script_destroy_world	(cs::script::world*& world);
}

#endif // #ifndef CS_SCRIPT_SCRIPT_ENGINE_H_INCLUDED
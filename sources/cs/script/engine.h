////////////////////////////////////////////////////////////////////////////
//	Module 		: script_engine.h
//	Created 	: 15.05.2005
//  Modified 	: 23.04.2008
//	Author		: Dmitriy Iassenev
//	Description : cs script engine interface
////////////////////////////////////////////////////////////////////////////

#ifndef CS_SCRIPT_ENGINE_H_INCLUDED
#define CS_SCRIPT_ENGINE_H_INCLUDED

#include <cs/script/interfaces.h>
#include <cs/core/logging.h>

namespace cs {
namespace script {

typedef void const*	file_buffer;
typedef void const*	file_handle;

struct DECLSPEC_NOVTABLE engine {
	enum file_types {
		file_type_script		= int(0),
		file_type_script_chunk,
		file_type_config,
		
		file_type_script_max,
		file_type_dummy			= int(-1),
	};

	virtual void			CS_SCRIPT_CALL	log							(cs::message_initiator const& message_initiator, cs::core::message_type const& message_type, char const* string) = 0;
	virtual bool			CS_SCRIPT_CALL	file_exists					(int file_type, char const* file_name) = 0;
	virtual file_handle		CS_SCRIPT_CALL	open_file_buffer			(int file_type, char const* file_name, file_buffer& file_buffer, unsigned int& buffer_size) = 0;
	virtual void			CS_SCRIPT_CALL	close_file_buffer			(file_handle file_handle) = 0;
	virtual bool			CS_SCRIPT_CALL	create_file					(int file_type, char const* file_name, file_buffer const& file_buffer, unsigned int const& buffer_size) = 0;
	virtual void			CS_SCRIPT_CALL	lua_studio_backend_file_path(int file_type, char const* file_name, char* path, unsigned int const& max_size) = 0;
	virtual	bool			CS_SCRIPT_CALL	use_debug_engine			() const = 0;
	virtual	bool			CS_SCRIPT_CALL	use_logging					() const = 0;

protected:
	CS_DECLARE_PURE_VIRTUAL_DESTRUCTOR( engine )
}; // struct engine

} // namespace script
} // namespace cs

#endif // #ifndef CS_SCRIPT_ENGINE_H_INCLUDED

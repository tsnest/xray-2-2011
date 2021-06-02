////////////////////////////////////////////////////////////////////////////
//	Created		: 12.02.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_CONSOLE_COMMAND_PROCESSOR_H_INCLUDED
#define XRAY_CONSOLE_COMMAND_PROCESSOR_H_INCLUDED

namespace xray {
namespace console_commands {

class console_command;
enum command_type;
enum execution_filter;

XRAY_CORE_API void				execute		( pcstr str, execution_filter filter );
XRAY_CORE_API void				show_help	( pcstr str );
XRAY_CORE_API void				save		( pcstr fn, command_type const command_type, memory::base_allocator& a );
XRAY_CORE_API void				load		( memory::reader& F, execution_filter filter );
XRAY_CORE_API u32				get_similar	( pcstr starts_from, console_command** dst, u32 dst_size );
XRAY_CORE_API console_command*	find		( pcstr name );

bool execute_console_commands				( fs_new::native_path_string cfg_file_path, execution_filter const filter	); 

} // namespace console_commands
} // namespace xray

#endif // #ifndef XRAY_CONSOLE_COMMAND_PROCESSOR_H_INCLUDED
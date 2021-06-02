////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_BUGTRAP_H_INCLUDED
#define DEBUG_BUGTRAP_H_INCLUDED

namespace xray {
namespace debug {

enum error_mode;
enum bugtrap_usage;

namespace bugtrap {

bool initialized		( );

void change_usage		( error_mode error_mode, bugtrap_usage bugtrap_usage );
void initialize			( );
void finalize			( );

bool on_thread_spawn	( );

void add_file			( pcstr full_path_file_name );

} // namespace bugtrap
} // namespace debug
} // namespace xray

#endif // #ifndef DEBUG_BUGTRAP_H_INCLUDED
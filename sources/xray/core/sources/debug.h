////////////////////////////////////////////////////////////////////////////
//	Created 	: 06.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

namespace xray {
namespace core {
namespace debug {

enum error_mode;
enum bugtrap_usage;

void preinitialize			( );
void initialize				( core::engine * engine );
void postinitialize			( );
void finalize				( );

void notify_xbox_debugger	( pcstr message );

void on_thread_spawn		( );
void on_error				( pcstr message );

core::engine *  get_core_engine ( );

} // namespace debug
} // namespace core
} // namespace xray

#endif // #ifndef DEBUG_H_INCLUDED
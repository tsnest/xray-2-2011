////////////////////////////////////////////////////////////////////////////
//	Created		: 14.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <sys/dbg.h>
#include <cell/sysmodule.h>
#include <cell/atomic.h>
#include <xray/debug/extensions.h>
#include <sys/timer.h>

#include "platform.h"
#include "utils.h"

#pragma comment( lib, "c")			// for _Sce_assert
#pragma comment( lib, "cs")			// for _Sce_assert
#pragma comment( lib, "c_stub")		// for _Sce_assert
#pragma comment( lib, "stdc++")		// for _Sce_assert

namespace xray {
namespace debug {

atomic32 xray::debug::interlocked_increment		( atomic32 & value )
{
	return				cellAtomicIncr32( &value ) + 1;
}

atomic32 xray::debug::interlocked_decrement		( atomic32 & value )
{
	return				cellAtomicDecr32( &value ) - 1;
}

atomic32 xray::debug::interlocked_exchange (atomic32 & target, atomic32 value)
{
	return									cellAtomicStore32( &target, value );
}

atomic32 xray::debug::interlocked_compare_exchange	( atomic32 & target, atomic32 value, atomic32 comparand )
{
	return									cellAtomicCompareAndSwap32( & target, value, comparand);
}

} // namespace debug
} // namespace xray


u32 xray::debug::current_thread_id			( )
{
	sys_ppu_thread_t						result;
	sys_ppu_thread_get_id					( &result );
	return									result;
}

void xray::debug::yield						( u32 milliseconds )
{
	if ( milliseconds ) {
		// receives time in microseconds
		sys_timer_usleep		( milliseconds*1000 );
		return;
	}

	sys_ppu_thread_yield		( );
}

bool xray::debug::is_debugger_present( )
{
#ifdef DEBUG
	return						true;
#else // #ifdef DEBUG
	return						false;
#endif // #ifdef DEBUG
}

pcstr xray::debug::platform::get_graphics_api_error_description	( int const error_code )
{
	NOT_IMPLEMENTED				( return "" );
}

void xray::debug::notify_xbox_debugger ( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
}

void xray::debug::debug_message_box (pcstr message)
{
	XRAY_UNREFERENCED_PARAMETER	(message);
}

void xray::debug::output								( pcstr message )
{
	XRAY_UNREFERENCED_PARAMETER	( message );
	OUTPUT_DEBUG_STRING			( message );
}
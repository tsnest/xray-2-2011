////////////////////////////////////////////////////////////////////////////
//	Created		: 05.05.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <sys/ppu_thread.h>
#include <sys/timer.h>
#include "threading_functions.h"
#include <xtls.h>

namespace xray {
namespace threading {

u32 current_thread_id			( )
{
	sys_ppu_thread_t			result;
	sys_ppu_thread_get_id		( &result );
	return						result;
}

void yield						( u32 const yield_time_in_ms )
{
	if ( yield_time_in_ms ) {
		// receives time in microseconds
		sys_timer_usleep		( yield_time_in_ms*1000 );
		return;
	}

	sys_ppu_thread_yield		( );
}

void set_thread_name_impl		( pcstr name_for_debugger )
{
	sys_ppu_thread_rename		( current_thread_id(), name_for_debugger );
}

tls_key_id_type tls_create_key	( )
{
	tls_key_id_type				result;
	_Tlsalloc					( &result, 0 );
	return						result;
}

void tls_delete_key				( tls_key_id_type const key )
{
	_Tlsfree					( key );
}

void tls_set_value				( tls_key_id_type key, pvoid value )
{
	_Tlsset						( key, value );
}

pvoid tls_get_value				( tls_key_id_type key )
{
	return						_Tlsget( key );
}

tls_key_id_type tls_get_invalid_key	( )
{
	return						tls_key_id_type(-1);
}

bool tls_is_valid_key			( tls_key_id_type key )
{
	return						key != tls_get_invalid_key();
}

void thread_entry	( pvoid argument );

static void thread_entry_protected	( uint64_t argument )
{
	xray::debug::protected_call		( &thread_entry, horrible_cast<uint64_t,pvoid>(argument).second );
}

thread_id_type spawn_internal				(
		thread_entry_params& argument,
		u32 const stack_size
	)
{
	sys_ppu_thread_t	thread_id;
	int const error		= 
		sys_ppu_thread_create(
			&thread_id,
			&thread_entry_protected,
			horrible_cast<pvoid,uint64_t>(&argument).second,
			0,
			size_t(stack_size ? stack_size : 64*Kb),
			0,//SYS_PPU_THREAD_CREATE_JOINABLE | SYS_PPU_THREAD_CREATE_INTERRUPT,
			""
		);

	if ( error != CELL_OK ) {
		if ( error == ENOMEM )
			LOG_ERROR	( "user memory shortage for user stack area" );
		else if ( error == EAGAIN )
			LOG_ERROR	( "thread ID shortage or kernel memory shortage" );
		else if ( error == EINVAL )
			LOG_ERROR	( "priority out of range" );
		else if ( error == EFAULT )
			LOG_ERROR	( "specified address is invalid" );
		else if ( error == EPERM )
			LOG_ERROR	( "SYS_PPU_THREAD_CREATE_JOINABLE and SYS_PPU_THREAD_CREATE_INTERRUPT are specified by being ORed" );
		else
			NODEFAULT	( );
	}

	R_ASSERT_CMP		( error, ==, CELL_OK );

	return				thread_id;
}

u32 actual_core_count ( )
{
	return							1;
}

void set_current_thread_affinity_impl	(u32 const hardware_thread)
{
	XRAY_UNREFERENCED_PARAMETER	( hardware_thread );
}

void increase_thread_priority		( )
{
}

} // namespace threading
} // namespace xray
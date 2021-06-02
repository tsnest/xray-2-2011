////////////////////////////////////////////////////////////////////////////
//	Module 		: threading_functions.cpp
//	Created 	: 26.08.2006
//  Modified 	: 26.08.2006
//	Author		: Dmitriy Iassenev
//	Description : threading functions
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <cs/core/threading_functions.h>
#include <cs/core/os_include.h>				// Sleep, SwitchToThread

#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
#	include <process.h>						// _beginthread
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
#	include <sys/ppu_thread.h>
#	include <sys/timer.h>
#else // #elif CS_PLATFORM_PS3
#endif // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360

namespace debug {
	void	on_thread_spawn	();
} // namespace debug

namespace threading {

struct thread_entry_params {
	pcstr					name;
	pvoid					arguments;
	thread_function_type*	function_to_call;
};

void thread_entry				(void* argument)
{
	thread_entry_params		params = *(thread_entry_params const* )argument;
	cs_delete				(argument);

	debugger_name			(params.name);
	debug::on_thread_spawn	();
	params.function_to_call	(params.arguments);
}

} // namespace threading

void threading::spawn			(
	thread_function_type* function_to_call,
	pcstr name,
	u32 const stack_size,
	pvoid arguments
)
{
	memory.switch_mt_alloc		();

	thread_entry_params* argument = cs_new<thread_entry_params>();
	argument->name				= name;
	argument->arguments			= arguments;
	argument->function_to_call	= function_to_call;

#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	_beginthread				(&thread_entry,stack_size,argument);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	struct helper {
		static void thread_entry_wrapper	( uint64_t argument )
		{
			thread_entry		( horrible_cast<uint64_t,pvoid>(argument).second );
		}
	}; // struct helper

	sys_ppu_thread_create		(
		0,
		&helper::thread_entry_wrapper,
		horrible_cast<pvoid,uint64_t>(&argument).second,
		0,
		stack_size,
		0,//SYS_PPU_THREAD_CREATE_JOINABLE | SYS_PPU_THREAD_CREATE_INTERRUPT,
		name
	);
#else // #elif CS_PLATFORM_PS3
#endif // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
}

void threading::yield			(u32 const milliseconds)
{
#if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	if (milliseconds) {
		Sleep	(milliseconds);
		return;
	}
	
	if (SwitchToThread())
		return;

	Sleep		(0);
#elif CS_PLATFORM_PS3 // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
	if ( milliseconds ) {
		// receives time in microseconds
		sys_timer_usleep		( milliseconds*1000 );
		return;
	}

	sys_ppu_thread_yield		( );
#else // #elif CS_PLATFORM_PS3
#	error implement yield function for your platform here
#endif // #if CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360
}

void threading::debugger_name	(pcstr debugger_name)
{
#if CS_DEBUG_LIBRARIES && (CS_PLATFORM_WINDOWS | CS_PLATFORM_XBOX_360)
#	pragma pack(push,8)
	struct thread_profile {
		u32		type;
		pcstr	name;
		u32		thread_id;
		u32		flags;
	};
#	pragma pack(pop)

	thread_profile		profile;
	profile.type		= 4096;
	profile.name		= debugger_name;
	profile.thread_id	= u32(-1);
	profile.flags		= 0;
	__try {
		RaiseException(0x406D1388,0,sizeof(profile)/sizeof(u32),(ULONG_PTR const*)&profile);
	}
	__except(EXCEPTION_CONTINUE_EXECUTION) {
	}
#else // #if CS_DEBUG_LIBRARIES
	CS_UNREFERENCED_PARAMETER	( debugger_name );
#endif // #if CS_DEBUG_LIBRARIES
}
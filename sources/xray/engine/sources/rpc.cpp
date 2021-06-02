////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "rpc.h"
#include <xray/resources.h>

static xray::fixed_vector<xray::rpc::callback, xray::rpc::count>	g_threads;

void xray::rpc::initialize				( )
{
	g_threads.resize		( count );
}

void xray::rpc::assign_thread_id		( threads_enum thread, u32 thread_id )
{
	g_threads[ thread ].m_thread_id	=	thread_id;
}

void xray::rpc::wait					( threads_enum const thread_id )
{
	rpc::callback& thread	= g_threads[thread_id];
	while ( thread.m_pending )
		threading::yield	( 1 );
}

bool xray::rpc::is_same_thread			( threads_enum const thread_id )
{
	return					( g_threads[thread_id].m_thread_id == threading::current_thread_id() );
}

static inline void run					(
		xray::rpc::threads_enum const thread_id,
		xray::rpc::callback_type const& callback,
		xray::rpc::break_parameters const break_parameters,
		xray::rpc::wait_parameters const wait_parameters,
		bool remote_only
	)
{
	if ( is_same_thread(thread_id) ) {
		if ( remote_only )
			return;

		callback			( );
		return;
	}

	xray::rpc::wait			( thread_id );

	xray::rpc::callback& thread	= g_threads[thread_id];
	R_ASSERT				( !thread.m_pending );

	thread.m_callback		= callback;
	thread.m_break_parameters = break_parameters;
	xray::threading::interlocked_exchange	( thread.m_pending, 1 );

	if ( wait_parameters == xray::rpc::wait_for_completion )
		wait				( thread_id );
	else
		R_ASSERT			( wait_parameters == xray::rpc::dont_wait_for_completion );
}

void xray::rpc::run						(
		threads_enum const thread_id,
		callback_type const& callback,
		break_parameters const break_parameters,
		wait_parameters const wait_parameters
	)
{
	::run					( thread_id, callback, break_parameters, wait_parameters, false );
}

void xray::rpc::run_remote_only			(
		threads_enum const thread_id,
		callback_type const& callback,
		break_parameters const break_parameters,
		wait_parameters const wait_parameters
	)
{
	::run					( thread_id, callback, break_parameters, wait_parameters, true );
}

void xray::rpc::process					( threads_enum const thread_id )
{
	R_ASSERT				( rpc::is_same_thread(thread_id) );

	callback& thread		= g_threads[thread_id];

	for (;;)  {
		while ( !thread.m_pending )
			threading::yield( 1 );

		break_parameters const break_parameters = thread.m_break_parameters;
		thread.m_callback	( );
		threading::interlocked_exchange	( thread.m_pending, 0 );
		if ( break_parameters == break_process_loop )
			break;
	}
}

bool xray::rpc::try_process_single_call	( threads_enum const thread_id )
{
	callback& thread		= g_threads[thread_id];
	if ( !thread.m_pending )
		return				false;

	thread.m_callback		( );
	threading::interlocked_exchange	( thread.m_pending, false );
	return					true;
}
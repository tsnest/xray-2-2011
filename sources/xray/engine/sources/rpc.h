////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2009
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef RPC_H_INCLUDED
#define RPC_H_INCLUDED

#include <boost/function.hpp>

namespace xray {
namespace rpc {

enum threads_enum {
	render,
	logic,
	editor,
	network,
	sound,
	count
}; // enum threads_enum

typedef boost::function< void () >		callback_type;

enum break_parameters {
	break_process_loop,
	continue_process_loop,
}; // enum break_parmeters

enum wait_parameters {
	wait_for_completion,
	dont_wait_for_completion,
}; // enum break_parmeters

struct callback {

	inline	callback	( )
	{
		m_pending				= 0;
		m_break_parameters		= break_process_loop;
		m_thread_id				= xray::threading::current_thread_id( );
	}

	callback_type				m_callback;
	u32							m_thread_id;
	break_parameters			m_break_parameters;
	threading::atomic32_type	m_pending;
}; // struct callback

void	initialize				( );

void	assign_thread_id		( threads_enum thread, u32 thread_id );

void	wait					( threads_enum const thread_id );

bool	is_same_thread			( threads_enum const thread_id );

void	run						( threads_enum const thread_id, callback_type const& callback, break_parameters break_parameters, wait_parameters wait_parameters );
void	run_remote_only			( threads_enum const thread_id, callback_type const& callback, break_parameters break_parameters, wait_parameters wait_parameters );

void	process					( threads_enum const thread_id );
bool	try_process_single_call	( threads_enum const thread_id );

} // namespace rpc
} // namespace xray

#endif // #ifndef RPC_H_INCLUDED
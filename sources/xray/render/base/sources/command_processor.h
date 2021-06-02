////////////////////////////////////////////////////////////////////////////
//	Created 	: 10.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef COMMAND_PROCESSOR_H_INCLUDED
#define COMMAND_PROCESSOR_H_INCLUDED

#include <xray/render/base/engine_wrapper.h>
#include <xray/threading_event.h>
#include <xray/intrusive_spsc_queue.h>

namespace xray {
namespace render {

typedef fastdelegate::FastDelegate< void (engine::command*) >	delete_on_tick_callback_type;

class command_processor : private boost::noncopyable {
public:
					command_processor	( delete_on_tick_callback_type engine );
			void	push_command		( engine::command* command );
			bool	run					( bool wait_for_command );
			void	purge_orders		( );
			void	set_push_thread_id	( );
			void	set_frame_id		( u32 frame_id ) { m_command_frame_id = frame_id; }
			void	initialize_queue	( engine::command* command );

private:
	typedef engine::command									command;
//	typedef limited_intrusive_list<command, &command::next>			queue_type;
	typedef intrusive_spsc_queue<command, command, &command::next>	queue_type;

private:
	delete_on_tick_callback_type	delete_on_tick;
	queue_type						m_orders;
	threading::event				m_wait_form_command_event;
	command*						m_next_frame_orders_first;
	command*						m_next_frame_orders_last;
	u32								m_command_frame_id;
	bool							m_process_next_frame_orders;
}; // struct command_processor

} // namespace render
} // namespace xray

#endif // #ifndef COMMAND_PROCESSOR_H_INCLUDED
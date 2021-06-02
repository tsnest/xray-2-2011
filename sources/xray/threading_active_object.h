////////////////////////////////////////////////////////////////////////////
//	Created		: 03.02.2011
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef THREADING_ACTIVE_OBJECT_H_INCLUDED
#define THREADING_ACTIVE_OBJECT_H_INCLUDED

namespace xray {
namespace threading {

class active_object 
{
public:
								active_object		(pcstr thread_name, pcstr thread_log_name, bool sleep_after_tick = true);
	virtual						~active_object		();

			void				tick				();

protected:
			void				wakeup_thread		();
	virtual	void		 		tick_impl			() = 0;

private:
			void				thread_proc_main	();

private:
	threading::event			m_wakeup_event;
	threading::atomic32_type	m_thread_id;
	threading::atomic32_type	m_thread_started;
	threading::atomic32_type	m_thread_must_exit;
	threading::atomic32_type	m_thread_exited;
	bool						m_sleep_after_tick;

}; // class active_object

} // namespace threading
} // namespace xray

#endif // #ifndef THREADING_ACTIVE_OBJECT_H_INCLUDED
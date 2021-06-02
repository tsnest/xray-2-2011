////////////////////////////////////////////////////////////////////////////
//	Created 	: 09.04.2009
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_THREADING_EVENT_H_INCLUDED
#define XRAY_THREADING_EVENT_H_INCLUDED

namespace xray {
namespace threading {

extern u32 const XRAY_CORE_API wait_time_infinite;

class XRAY_CORE_API event_tasks_unaware : private core::noncopyable {
public:
	enum wait_result_enum	{ wait_result_failed, wait_result_signaled, wait_result_still_nonsignaled };

public:
						event_tasks_unaware	( bool initial_state = false );
						~event_tasks_unaware( );
	void				set					( bool value);
	wait_result_enum	wait				( u32 max_wait_time_ms );
	pvoid				get_handle			( ) const;

private:
#if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	char XRAY_DEFAULT_ALIGN	m_event[sizeof(pvoid)]; // sizeof(CRITICAL_SECTION);
#elif XRAY_PLATFORM_PS3 // #if XRAY_PLATFORM_WINDOWS | XRAY_PLATFORM_XBOX_360
	char XRAY_DEFAULT_ALIGN	m_event[4];				// sizeof(sys_event_flag_t);
#endif // #elif XRAY_PLATFORM_XBOX_360
}; // class event_tasks_unaware

class XRAY_CORE_API event : private core::noncopyable {
public:
	typedef event_tasks_unaware::wait_result_enum	wait_result_enum;

public:
	inline						event		( bool initial_state = false ) : m_event(initial_state)	{ }
	inline	void				set			( bool value)											{ m_event.set(value); }
			wait_result_enum	wait		( u32 max_wait_time_ms );
	inline	pvoid				get_handle	( ) const												{ return m_event.get_handle(); }

private:
	event_tasks_unaware			m_event;
}; // class event

} // namespace threading
} // namespace xray

#endif // #ifndef XRAY_THREADING_EVENT_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Sergey Prishchepa
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////
#ifndef XRAY_PROFILER_INLINE_H_INCLUDED
#define XRAY_PROFILER_INLINE_H_INCLUDED

inline profile_portion::profile_portion(shared_string timer_id)
{
	m_timer_id = timer_id;
	m_time = get_profiler().get_elapsed_ticks();
}

inline profile_portion::~profile_portion()
{
	u64 temp = get_profiler().get_elapsed_ticks();
	m_time = temp - m_time;
	get_profiler().add_profile_portion(*this);
}

inline profiler& get_profiler()
{
	return (*g_profiler);
}

inline profile_stats::profile_stats()
{
	m_update_time	= 0;
	m_name			= "";
	m_time			= 0.f;
	m_min_time		= 0.f;
	m_max_time		= 0.f;
	m_total_time	= 0.f;
	m_count			= 0;
	m_call_count	= 0;
}

#endif // #ifndef XRAY_PROFILER_INLINE_H_INCLUDED
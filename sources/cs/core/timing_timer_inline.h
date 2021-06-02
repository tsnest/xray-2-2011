////////////////////////////////////////////////////////////////////////////
//	Module 		: timer_inline.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : timer class inline functions
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_TIMING_TIMER_INLINE_H_INCLUDED
#define CS_CORE_TIMING_TIMER_INLINE_H_INCLUDED

inline timing::timer::timer					()
{
	m_start_time	= 0;
}
inline void timing::timer::start			()
{
	m_start_time	= cs::core::g_platform.get_QPC();
}

inline u64 timing::timer::get_elapsed_ticks	() const
{
	return			cs::core::g_platform.get_QPC() - m_start_time;
}

inline u32 timing::timer::get_elapsed_ms	() const
{
	return			u32(get_elapsed_ticks()*u64(1000)/cs::core::g_platform.qpc_per_second);
}

inline float timing::timer::get_elapsed_sec	() const
{
	return			float(double(get_elapsed_ticks())/float(cs::core::g_platform.qpc_per_second));
}

#endif // #ifndef CS_CORE_TIMING_TIMER_INLINE_H_INCLUDED
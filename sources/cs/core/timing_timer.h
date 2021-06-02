////////////////////////////////////////////////////////////////////////////
//	Module 		: timer.h
//	Created 	: 27.08.2006
//  Modified 	: 27.08.2006
//	Author		: Dmitriy Iassenev
//	Description : timer class
////////////////////////////////////////////////////////////////////////////

#ifndef CS_CORE_TIMING_TIMER_H_INCLUDED
#define CS_CORE_TIMING_TIMER_H_INCLUDED

namespace timing {

class timer {
private:
	u64				m_start_time;

public:
	inline			timer				();
	inline	void	start				();
	inline	u64		get_elapsed_ticks	() const;
	inline	u32		get_elapsed_ms		() const;
	inline	float	get_elapsed_sec		() const;
};

}

#include "timing_timer_inline.h"

#endif // #ifndef CS_CORE_TIMING_TIMER_H_INCLUDED
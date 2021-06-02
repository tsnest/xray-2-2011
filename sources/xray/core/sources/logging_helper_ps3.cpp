////////////////////////////////////////////////////////////////////////////
//	Created		: 30.04.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

namespace xray {
namespace logging {

void fill_local_time			( xray::buffer_string& dest )
{
	time_t current_time			= time( 0 );
	tm* const calendar_time		= gmtime( &current_time );
	dest.appendf				(
		" %02d:%02d:%02d:%03d}", 
		calendar_time->tm_hour,
		calendar_time->tm_min,
		calendar_time->tm_sec,
		0
	);

	//u64 const time				= timing::query_performance_counter();
	//u64 const ticks_per_second	= timing::g_qpc_per_second;

	//u64 accumulator				= 0;
	//u64 const days				= time/(ticks_per_second*u64(60)*u64(60)*u64(24));
	//accumulator					+= days*ticks_per_second*u64(60)*u64(60)*u64(24);

	//u64 const hours				= (time - accumulator)/(ticks_per_second*u64(60)*u64(60));
	//accumulator					+= hours*ticks_per_second*u64(60)*u64(60);

	//u64 const minutes			= (time - accumulator)/(ticks_per_second*u64(60));
	//accumulator					+= minutes*ticks_per_second*u64(60);

	//u64 const seconds			= (time - accumulator)/ticks_per_second;
	//accumulator					+= seconds*ticks_per_second;

	//u64 const milliseconds		= (time - accumulator)*u64(1000)/ticks_per_second;

	//dest.appendf				(
	//	" %02d:%02d:%02d:%03d}", 
	//	hours, 
	//	minutes, 
	//	seconds, 
	//	milliseconds
	//);
}

} // namespace logging
} // namespace xray
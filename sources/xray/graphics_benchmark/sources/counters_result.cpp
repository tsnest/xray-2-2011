////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "counters_result.h"
#include "benchmark.h"
#include "benchmark_types.h"

namespace xray {
namespace graphics_benchmark {

std::string counters_result::as_text(counters_result& base, bool is_base, benchmark& benchmark, group_to_execute&, test_to_execute&)
{
	std::string result;
	char buffer[4096];

	::sprintf(buffer,"%10f   ", elapsed_time);
	result = buffer;

	u64 const max_frequency		 = 1000000000;

	if (!is_base) 
	{
		::sprintf(buffer,"%10f    ", (elapsed_time - base.elapsed_time));
		result += buffer;
	}
	else
	{
		::sprintf(buffer,"              ");
		result += buffer;
	}

	::sprintf(buffer,"%10d    ", max_frequency * elapsed_ticks / frequency);
	result += buffer;

	if (!is_base) 
	{
		u32 wnd_size_x, wnd_size_y;
		benchmark.get_window_resolution(wnd_size_x,wnd_size_y);
		
		u64 const magic_scale_number = 100000;


		double diff			= max_frequency * double((s64)elapsed_ticks - (s64)base.elapsed_ticks) / frequency,
			   diff_approx	= magic_scale_number * diff / double( wnd_size_x * wnd_size_y);
		
		::sprintf(buffer,"%10.0f  %10.1f" , diff, diff_approx);//   /   (  2*(s64)elapsed_ticks/1000   )     );
		result += buffer;
	}
	else
	{
		::sprintf(buffer,"                      ");
		result += buffer;
	}

	return result;
}

} // namespace graphics_benchmark
} // namespace xray
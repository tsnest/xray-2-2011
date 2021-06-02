////////////////////////////////////////////////////////////////////////////
//	Created		: 17.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef COUNTERS_RESULT_H_INCLUDED
#define COUNTERS_RESULT_H_INCLUDED

#include "benchmark_renderer.h"

namespace xray {
namespace graphics_benchmark {

class benchmark;
struct group_to_execute;
struct test_to_execute;

struct counters_result
{
	double elapsed_time;		// in microseconds
	float fps;					// 1/sec
	float clear_pass_duration;  // in microseconds
	double pixels_per_tick;
	u64 elapsed_ticks;
	u32 triangles_count;
	u32 batch_count;

	u64 frequency;

	std::string as_text(counters_result& base, bool is_base, benchmark& benchmark, group_to_execute& group, test_to_execute& test);
	// more
}; // struct counters_result

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef COUNTERS_RESULT_H_INCLUDED
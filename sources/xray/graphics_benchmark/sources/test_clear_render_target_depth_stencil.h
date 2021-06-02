////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef TEST_CLEAR_RENDER_TARGET_DEPTH_STENCIL_H_INCLUDED
#define TEST_CLEAR_RENDER_TARGET_DEPTH_STENCIL_H_INCLUDED

#include "benchmark_test.h"

namespace xray {
namespace graphics_benchmark {

////////////////////////////////////////////////////////////////////////////
class test_clear_depth_stencil: public benchmark_test, public creator_base<test_clear_depth_stencil>
{
public:
	virtual void execute	(benchmark&, u32, u32, test_to_execute&, gpu_timer&);
}; // class test_clear_depth_stencil


////////////////////////////////////////////////////////////////////////////
class test_clear_render_target: public benchmark_test, public creator_base<test_clear_render_target>
{
public:
	virtual void execute	(benchmark&, u32, u32, test_to_execute&, gpu_timer&);
}; // class test_clear_render_target

} // namespace graphics_benchmark
} // namespace xray

#endif // #ifndef TEST_CLEAR_RENDER_TARGET_DEPTH_STENCIL_H_INCLUDED
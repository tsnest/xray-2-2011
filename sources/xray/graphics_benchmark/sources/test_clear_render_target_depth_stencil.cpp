////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "test_simple_shader_op.h"
#include "gpu_timer.h"
#include "benchmark.h"
#include "xray/render/core/effect_descriptor.h"
#include "test_clear_render_target_depth_stencil.h"
#include "xray/math_randoms_generator.h"

namespace xray {
namespace graphics_benchmark {


void test_clear_depth_stencil::execute	(benchmark&, u32 current_pass, u32 num_passes, test_to_execute&, gpu_timer&)
{
	xray::math::random32 r;
	r.seed(100);
	xray::render::backend::ref().clear_depth_stencil( D3D_CLEAR_DEPTH | D3D_CLEAR_STENCIL, (float)current_pass/(float)num_passes, static_cast_checked<u8>(r.random(255)));
}

void test_clear_render_target::execute	(benchmark&, u32 current_pass, u32 num_passes, test_to_execute&, gpu_timer&)
{
	float val = (float)current_pass/(float)num_passes;
	xray::render::backend::ref().clear_render_targets( xray::math::color( val, val, val));
}

} // namespace graphics_benchmark
} // namespace xray
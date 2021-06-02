////////////////////////////////////////////////////////////////////////////
//	Created		: 15.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "xray\render\core\res_effect.h"
#include "test_2d_shader_op.h"
#include "benchmark.h"
#include "benchmark_renderer.h"
#include "gpu_timer.h"
#include "xray/math_randoms_generator.h"
#include "benchmark_helpers.h"
#include "xray\render\core\pix_event_wrapper.h"
#include "xray\render\core\res_texture.h"

namespace xray {
namespace graphics_benchmark {

bool test_2d_shader_op::initialize	( benchmark& benchmark, group_to_execute& group, test_to_execute& test, std::map<std::string,std::string>& parameters, std::string& out_comments)	
{ 
	m_vs_code = 
		"#include \"graphics_benchmark_common.h\"			\n"
		"v2p_GB_Test main ( v_GB_Test i)					\n"
		"{													\n"
		"	v2p_GB_Test result;								\n"
		"	result.HPos = float4(i.P.xyz,1);				\n"
		"	result.Color = i.Color;							\n"
		"	result.Color0 = i.Color0;						\n"
		"	result.Color1 = i.Color1;						\n"
		"	result.Color2 = i.Color2;						\n"
		"	result.Color3 = i.Color3;						\n"
		"	result.Color4 = i.Color4;						\n"
		"	result.Color5 = i.Color5;						\n"
		"	result.Color6 = i.Color6;						\n"
		"	result.Tex0 = i.Tex0;							\n"
		"	result.Normal = i.Normal;						\n"
		"	return result;									\n"
		"}													\n";
	
	// Initialize shader.
	test_simple_shader_op::initialize(benchmark,group,test,parameters,out_comments);
	
	return true;
}

void test_2d_shader_op::execute	( benchmark& benchmark, u32, u32, test_to_execute& test, gpu_timer& gtimer)
{
	test_simple_shader_op::execute(benchmark,0,1,test,gtimer);
	benchmark.renderer ( ).render_fullscreen_quad( gtimer);
}

} // namespace graphics_benchmark
} // namespace xray
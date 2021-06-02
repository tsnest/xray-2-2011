////////////////////////////////////////////////////////////////////////////
//	Created		: 13.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "effect_complex_post_process_blend.h"
 
namespace xray {
namespace render {
//
//void effect_complex_post_process_blend::compile( effect_compiler& compiler, const custom_config_value& custom_config)
//{
//	XRAY_UNREFERENCED_PARAMETERS(custom_config);
//
//	compiler.begin_technique();
//		compiler.begin_pass("copy_image", "complex_post_process_blend");
//			compiler.set_texture("t_blurred_frame_color", r2_rt_blur0); // set here?
//			compiler.set_texture("t_frame_color", r2_rt_generic0);		// set here?
//			compiler.set_texture("t_position", r2_rt_p);				// set here?
//			compiler.set_texture("t_frame_luminance", r2_rt_frame_luminance);
//			compiler.set_texture("t_frame_luminance_histogram", r2_rt_frame_luminance_histogram);
//			
//			compiler.set_depth( true, false);
//		compiler.end_pass();
//	compiler.end_technique();
//}

} // namespace render 
} // namespace xray 

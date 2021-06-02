////////////////////////////////////////////////////////////////////////////
//	Created		: 13.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef EFFECT_COMPLEX_POST_PROCESS_BLEND_H_INCLUDED
#define EFFECT_COMPLEX_POST_PROCESS_BLEND_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/effect_compiler.h>
#include <xray/render/core/shader_configuration.h>
#include "shared_names.h"

namespace xray {
namespace render {

template< bool use_bokeh_dof = false, bool use_bokeh_image = false >
class effect_complex_post_process_blend: public effect_descriptor
{
public:
	virtual void compile( effect_compiler& compiler, const custom_config_value& custom_config)
	{
		XRAY_UNREFERENCED_PARAMETERS(custom_config);
		
		shader_configuration configuration;
		configuration.use_bokeh_dof		= use_bokeh_dof;
		configuration.use_bokeh_image	= use_bokeh_image;
		
		compiler.begin_technique();
			compiler.begin_pass("copy_image", NULL, "complex_post_process_blend", configuration, NULL);
				compiler.set_texture("t_blurred_frame_color", r2_rt_blur0); // set here?
				compiler.set_texture("t_frame_color", r2_rt_generic0);		// set here?
				compiler.set_texture("t_position", r2_rt_p);				// set here?
				compiler.set_texture("t_frame_luminance", r2_rt_frame_luminance);
				compiler.set_texture("t_frame_luminance_previous", r2_rt_frame_luminance_previous);
				compiler.set_texture("t_frame_luminance_histogram", r2_rt_frame_luminance_histogram);
				compiler.set_texture("t_light_scattering", r2_rt_light_scattering);
				compiler.set_texture("t_sphere_falloff", "fx/sphere_falloff");
				compiler.set_texture("t_bokeh_image", "fx/bokeh_image");
				//compiler.set_texture("t_skin_position", r2_rt_skin_scattering_position);
				//compiler.set_texture("t_skin_scattering", r2_rt_skin_scattering);
				//compiler.set_texture("t_skin_scattering_temp", r2_rt_skin_scattering_temp);
				//compiler.set_texture("t_skin_scattering_blurred_0", r2_rt_skin_scattering_blurred_0);
				//compiler.set_texture("t_skin_scattering_blurred_1", r2_rt_skin_scattering_blurred_1);
				//compiler.set_texture("t_skin_scattering_blurred_2", r2_rt_skin_scattering_blurred_2);
				//compiler.set_texture("t_skin_scattering_blurred_3", r2_rt_skin_scattering_blurred_3);
				//compiler.set_texture("t_skin_scattering_blurred_4", r2_rt_skin_scattering_blurred_4);
				//compiler.set_texture("t_mlaa_edges", r2_rt_mlaa_edges);
				//compiler.set_texture("t_mlaa_blend", r2_rt_mlaa_blended_weights);
				//compiler.set_texture("t_bug", "engine/bug");
				//compiler.set_texture("t_lpv_rsm_albedo", r2_rt_lpv_rsm_albedo);
				//compiler.set_texture("t_lpv_rsm_normal", r2_rt_lpv_rsm_normal);
				//compiler.set_texture("t_lpv_rsm_position", r2_rt_lpv_rsm_position);
				//compiler.set_texture("t_lpv_rsm_albedo_source", r2_rt_lpv_rsm_albedo_source);
				//compiler.set_texture("t_lpv_rsm_normal_source", r2_rt_lpv_rsm_normal_source);
				//compiler.set_texture("t_lpv_rsm_position_source", r2_rt_lpv_rsm_position_source);
				//compiler.set_depth(true, false);
			compiler.end_pass();
		compiler.end_technique();
	}
}; // class effect_complex_post_process_blend

} // namespace render 
} // namespace xray 


#endif // #ifndef EFFECT_COMPLEX_POST_PROCESS_BLEND_H_INCLUDED
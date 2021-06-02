////////////////////////////////////////////////////////////////////////////
//	Created		: 20.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef PARALLEL_LIGHT_EFFECT_H_INCLUDED
#define PARALLEL_LIGHT_EFFECT_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>

namespace xray {
namespace render {

class parallel_light_effect : public effect_descriptor {
private:
	virtual	void compile	( effect_compiler& compiler, custom_config_value const& config )
	{
		XRAY_UNREFERENCED_PARAMETER	( config);

		bool		blend = TRUE;	//RImplementation.o.fp16_blend;
		D3D_BLEND	dest = blend ? D3D_BLEND_ONE : D3D_BLEND_ZERO;

		compiler.begin_technique( /*unshadowed*/)
			.begin_pass	( "light", "parallel_light_accumulator")
				.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL)
				.set_stencil		( true, 0x81,0x81,0x00, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP,D3D_STENCIL_OP_KEEP)
				//.set_alpha_blend	( blend, D3D_BLEND_ONE, dest)
				.set_alpha_blend	( true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
				.set_cull_mode		( D3D_CULL_BACK)
				.set_texture		( "t_position",    r2_rt_p)
				.set_texture		( "t_normal",      r2_rt_n)
				.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse)
				.set_texture		( "t_decals_normal", r2_rt_decals_normal)
				//.set_texture		( "t_accumulator", r2_rt_accum) //used when fp16 blend is not available
				.set_texture		( "t_material",    r2_material)
				.set_texture		( "t_skin_shadow",  "skin/skin_shadow")
				.set_texture		( "t_target_ex",    r2_rt_p_ex)
				.set_texture		( "t_emissive",    r2_rt_emissive)
				.set_texture		( "t_tangents", r2_rt_tangents)
			.end_pass()
		.end_technique();
	}
}; // class parallel_light_effect

} // namespace render
} // namespace xray

#endif // #ifndef PARALLEL_LIGHT_EFFECT_H_INCLUDED
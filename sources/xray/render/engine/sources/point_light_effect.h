////////////////////////////////////////////////////////////////////////////
//	Created		: 20.12.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef POINT_LIGHT_EFFECT_H_INCLUDED
#define POINT_LIGHT_EFFECT_H_INCLUDED

#include <xray/render/core/effect_descriptor.h>
#include <xray/render/core/effect_compiler.h>
#include "shared_names.h"
#include "geometry_type.h"

namespace xray {
namespace render {

template < bool shadowed, bool shadower >
class point_light_effect : public effect_descriptor {
private:
	enum
	{
		front_side_pass,
		back_side_pass,
	};
	virtual	void compile	( effect_compiler& compiler, custom_config_value const& config )
	{
		XRAY_UNREFERENCED_PARAMETER	( config);

		shader_configuration configuration;
		configuration.shadowed_light		= shadowed;
		
		compiler.begin_technique(); // front side of sphere
			compiler.begin_pass	( "light", NULL, "point_light_accumulator", configuration, NULL);
				compiler.set_depth			( true, false, D3D_COMPARISON_LESS_EQUAL);
				//compiler.set_stencil		( true, 0x81, 0x81, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR, D3D_STENCIL_OP_DECR);
				//compiler.set_stencil		( true, terrain_geometry_type + 1, 0xff, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR, D3D_STENCIL_OP_DECR);
				compiler.set_stencil		( true, 0xff, check_all_geometry_type, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT, D3D_STENCIL_OP_INVERT);
				
				if (identity(shadower))
				{
					compiler.set_alpha_blend(true,D3D_BLEND_ZERO,D3D_BLEND_SRC_COLOR);
				}
				else
				{
					compiler.set_alpha_blend	( true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
				}
				
				compiler.set_cull_mode		( D3D_CULL_BACK);
				compiler.set_texture		( "t_position",    r2_rt_p);
				compiler.set_texture		( "t_normal",      r2_rt_n);
				compiler.set_texture		( "t_material",    r2_material);
				compiler.set_texture		( "t_target_ex",   r2_rt_p_ex);
				compiler.set_texture		( "t_emissive",    r2_rt_emissive);
				compiler.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse);
				compiler.set_texture		( "t_decals_normal", r2_rt_decals_normal);
				
				compiler.set_texture		( "t_tangents", r2_rt_tangents);
				
				//compiler.set_texture		( "shadowmap_texture", r2_rt_shadow_map);
			compiler.end_pass();
		compiler.end_technique();
		
		compiler.begin_technique(); // back side of sphere
			compiler.begin_pass	( "light", NULL, "point_light_accumulator", configuration, NULL);
				compiler.set_depth			( false, false, D3D_COMPARISON_LESS_EQUAL);
				//compiler.set_stencil		( true, 0x81, 0x81, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR, D3D_STENCIL_OP_KEEP);
				//compiler.set_stencil		( true, terrain_geometry_type + 1, 0xff, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_DECR, D3D_STENCIL_OP_KEEP);
				compiler.set_stencil		( true, 0xff, check_all_geometry_type, 0xff, D3D_COMPARISON_EQUAL, D3D_STENCIL_OP_KEEP, D3D_STENCIL_OP_INVERT, D3D_STENCIL_OP_KEEP);
				
				if (identity(shadower))
				{
					compiler.set_alpha_blend(true,D3D_BLEND_ZERO,D3D_BLEND_SRC_COLOR);
				}
				else
				{
					compiler.set_alpha_blend	( true, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD, D3D_BLEND_ONE, D3D_BLEND_ONE, D3D_BLEND_OP_ADD);
				}
				
				compiler.set_cull_mode		( D3D_CULL_FRONT);
				compiler.set_texture		( "t_position",    r2_rt_p);
				compiler.set_texture		( "t_normal",      r2_rt_n);
				compiler.set_texture		( "t_material",    r2_material);
				compiler.set_texture		( "t_target_ex",   r2_rt_p_ex);
				compiler.set_texture		( "t_emissive",    r2_rt_emissive);
				compiler.set_texture		( "t_decals_diffuse", r2_rt_decals_diffuse);
				compiler.set_texture		( "t_decals_normal", r2_rt_decals_normal);

				compiler.set_texture		( "t_tangents", r2_rt_tangents);

				//compiler.set_texture		( "shadowmap_texture", r2_rt_shadow_map);
			compiler.end_pass();
		compiler.end_technique();
	}
}; // class point_light_effect

} // namespace render
} // namespace xray

#endif // #ifndef POINT_LIGHT_EFFECT_H_INCLUDED
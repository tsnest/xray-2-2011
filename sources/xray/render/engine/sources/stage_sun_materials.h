////////////////////////////////////////////////////////////////////////////
//	Created		: 10.02.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SUN_MATERIALS_H_INCLUDED
#define STAGE_SUN_MATERIALS_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render {

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class shader_constant_host;

class stage_sun_materials: public stage
{
public:
	enum sun_cascade
	{
		cascade_near	= 0,
		cascade_middle	= 1,
		cascade_far		= 2
	};

			stage_sun_materials		( renderer_context* context);
			~stage_sun_materials	();

			void	execute			( sun_cascade cascade );

private:
			//float4x4	calc_sun_matrix	(const float4x4& frustum, const float3& dir);
			void		render_sun_mask	(const float3& light_dir, u32 material_marker);

	static	void	rm_near			();
	static	void	rm_normal		();
	static	void	rm_far			();

private:
	ref_effect		m_sh_accum_mask;
	ref_effect		m_sh_accum_direct[4];
	
	shader_constant_host*	m_c_light_direction;
	shader_constant_host*	m_c_light_color;
	shader_constant_host*	m_sunmask;
	shader_constant_host*	m_out_filter;
	shader_constant_host*	m_c_light_attenuation_power;
}; // class stage_sun_materials


} // namespace render
} // namespace xray



#endif // #ifndef STAGE_SUN_MATERIALS_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SUN_H_INCLUDED
#define STAGE_SUN_H_INCLUDED


#include "sun_cascade.h"
#include "stage.h"

namespace xray {
namespace render {

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class shader_constant_host;

class stage_sun: public stage
{
public:
					stage_sun		( renderer_context* context );
					~stage_sun		( );
	virtual void	execute			( );
			void	execute_cascade	( u32 cascade );
private:
			bool	is_effects_ready() const;
		//float4x4	calc_sun_matrix	( const float4x4& frustum, const float3& dir);
			void	phase_sun_mask	(
						u32 material_id,
						float3 const &light_dir,
						float3 const& light_color,
						float light_spec,
						float intensity,
						int lighting_mode,
						float diffuse_influence_factor,
						float specular_influence_factor
					);

	static	void	rm_near			( );
	static	void	rm_normal		( );
	static	void	rm_far			( );

private:
	ref_effect				m_sh_accum_mask;
	ref_effect				m_sh_accum_direct[4];

private:
	shader_constant_host*	m_c_light_direction;
	shader_constant_host*	m_c_light_color;
	shader_constant_host*	m_c_shadow_transparency;
	shader_constant_host*	m_c_light_intensity;
	shader_constant_host*	m_c_lighting_model;
	shader_constant_host*	m_shadow[ sun_cascade::num_max_sun_shadow_cascades ];
	shader_constant_host*	m_c_diffuse_influence_factor;
	shader_constant_host*	m_c_specular_influence_factor;
	
	shader_constant_host*	m_c_environment_skylight_upper_color;
	shader_constant_host*	m_c_environment_skylight_lower_color;
}; // class stage_sun


} // namespace render
} // namespace xray



#endif // #ifndef STAGE_SUN_H_INCLUDED
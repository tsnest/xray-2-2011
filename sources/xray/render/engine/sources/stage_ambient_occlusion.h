////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_STAGE_AMBIENT_OCCLUSION_H_INCLUDED
#define XRAY_RENDER_ENGINE_STAGE_AMBIENT_OCCLUSION_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render {

class resource_intrusive_base;

class res_effect;
typedef	xray::resources::resource_ptr<
	xray::render::res_effect, 
	xray::resources::unmanaged_intrusive_base> 
ref_effect;

class res_geometry;
typedef	intrusive_ptr<
	res_geometry, 
	resource_intrusive_base, 
	threading::single_threading_policy>	
ref_geometry;

class untyped_buffer;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class shader_constant_host;

class stage_ambient_occlusion: public stage
{
public:
					stage_ambient_occlusion		(renderer_context* context);

	virtual void	execute						();
	virtual void	execute_disabled			();
	
private:
			bool	is_effects_ready	() const;
	
	ref_effect							m_sh_combine;
	ref_effect							m_sh_ssao_accumulation;
	ref_effect							m_sh_ssao_filter4x4;
	ref_effect							m_post_process_antialiasing_shader;
	ref_effect							m_post_process_deferred_transparency_shader;
	
	ref_geometry						m_g_combine;
	untyped_buffer_ptr					m_vb;
	
	shader_constant_host*				m_ao_parameters;
	
}; // class stage_ambient_occlusion

} // namespace render
} // namespace xray


#endif // #ifndef XRAY_RENDER_ENGINE_STAGE_AMBIENT_OCCLUSION_H_INCLUDED
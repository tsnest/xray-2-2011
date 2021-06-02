////////////////////////////////////////////////////////////////////////////
//	Created		: 15.04.2009
//	Author		: Mykhailo Parfeniuk
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_SKYBOX_H_INCLUDED
#define STAGE_SKYBOX_H_INCLUDED

#include "stage.h"

namespace xray {
namespace render {


class shader_constant_host;
class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class stage_skybox: public stage
{
public:
					stage_skybox	(renderer_context* context);
					~stage_skybox	();
	virtual bool	is_enabled		() const;
	virtual void	execute			();

private:
			bool	is_effects_ready() const;
			void	render_sky		();
			void	render_clouds	();

private:
	ref_effect				m_sh_sky;
	ref_geometry			m_g_skybox;
	shader_constant_host*	m_c_sun_direction;
}; // class stage_skybox

} // namespace render
} // namespace xray


#endif // #ifndef STAGE_SKYBOX_H_INCLUDED
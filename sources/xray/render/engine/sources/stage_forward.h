////////////////////////////////////////////////////////////////////////////
//	Created		: 02.12.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef STAGE_FORWARD_H_INCLUDED
#define STAGE_FORWARD_H_INCLUDED

#include "stage.h"
#include "render_model.h"
#include <xray/render/core/shader_constant_host.h>

namespace xray {
namespace render {

class stage_forward: public stage
{
public:
							stage_forward		(renderer_context* context);
							~stage_forward		();
	virtual void			execute				();
			bool			is_effects_ready	() const;
			
			ref_effect							m_opaque_geometry_mask_effect;
	
			render_surface_instances			m_dynamic_visuals;
	
			shader_constant_host*				m_tree_position_and_scale_parameter;
			shader_constant_host*				m_tree_rotation_parameter;
	
			shader_constant_host*				m_far_fog_color_and_distance;
			shader_constant_host*				m_near_fog_distance;
}; // class stage_forward

} // namespace render
} // namespace xray

#endif // #ifndef STAGE_FORWARD_H_INCLUDED

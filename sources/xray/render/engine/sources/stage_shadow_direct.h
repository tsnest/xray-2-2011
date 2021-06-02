////////////////////////////////////////////////////////////////////////////
//	Created		: 18.11.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_STAGE_SHADOW_DIRECT_H_INCLUDED
#define XRAY_RENDER_ENGINE_STAGE_SHADOW_DIRECT_H_INCLUDED

#include "stage.h"
#include "render_model.h"

namespace xray {
namespace render {

class shader_constant_host;

class stage_shadow_direct: public stage
{
public:
	enum enum_sun_cascade
	{
		enum_cascade_near		= 0,
		enum_cascade_middle		= 1,
		enum_cascade_far		= 2
	};

					stage_shadow_direct			(renderer_context* context);
					~stage_shadow_direct		();
			
	virtual void	execute						();
	virtual void	execute_disabled			();
	
			void	execute_cascade				(u32 cascade, u32 cascade_index, u32 shadow_map_size);
			void	draw_debug					(u32 in_cascade_id);
private:
			float3	compute_aligment			(float3 const & lightXZshift, float4x4 const & light_space_transform, float smap_res);
			float3	compute_aligment1			(float3 const & lightXZshift, float4x4 const & light_space_transform, float smap_res);
			u32		index_to_shadow_size		(u32 size_index ) const;
			void	render_speedtree_instances	(math::float3 const& viewer_position, u32 cascade_index);
			void	render_terrain				(math::float3 const& viewer_position);

			bool	is_effects_ready			() const;			
	shader_constant_host*						m_c_light_direction;
	shader_constant_host*						m_c_light_position;
	shader_constant_host*						m_c_light_attenuation_power;
	shader_constant_host*						m_c_start_corner;
	
	ref_effect									m_effect_shadow_direct;
	
	render_surface_instances								m_caster_model;
	u32											m_old_shadow_map_size;
}; // class stage_shadow_direct


} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_STAGE_SHADOW_DIRECT_H_INCLUDED

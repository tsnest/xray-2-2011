////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_STAGE_LIGHT_PROPAGATION_VOLUMES_H_INCLUDED
#define XRAY_RENDER_ENGINE_STAGE_LIGHT_PROPAGATION_VOLUMES_H_INCLUDED

#include "stage.h"
#include "render_model.h"
#include <xray/render/core/shader_constant_host.h>
#include <xray/render/facade/vertex_input_type.h>
#include "radiance_volume.h"

namespace xray {
namespace render {

class light;

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

class res_texture;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class untyped_buffer;
class resource_intrusive_base;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class stage_light_propagation_volumes: public stage
{
public:
							stage_light_propagation_volumes		(renderer_context* context);
							~stage_light_propagation_volumes	();
	virtual void			execute								();
			bool			is_effects_ready					() const;
			void			draw_debug							();
	
private:
			//void			process_cascade						(u32 const cascade_index);
			//void			process_spot_light					(u32 const cascade_index, xray::render::light* in_light);
			// Render to Reflective Shadow Map
			void			render_to_rms						(//xray::render::light* in_light,
																 xray::math::float3 const& light_color,
																 float const light_intensity,
																 xray::math::float4x4 const& view_matrix, 
																 xray::math::float4x4 const& projection_matrix);
			
			void			render_to_point_rms					(xray::render::light* l, u32 const face_index);
			void			render_to_sky_rms					(u32 const face_index, u32 const cascade_index);
			void			render_to_spot_rms					(xray::render::light* l);
			void			render_to_sun_rms					(xray::render::light* sun, u32 const cascade_index);
			
			void			downsample_rsm						(xray::float3 const& light_direction, 
																 xray::float3 const& grid_origin, 
																 float grid_scale);

			void			downsample_gbuffer					();
			
			void			inject_lighting						(u32 const cascade_index,
																 xray::math::float3 const& light_position,
																 xray::math::float3 const& light_direction,
																 float light_fov);
			
			void			inject_occluders					(u32 const cascade_index,
																 xray::math::float3 const& light_position,
																 xray::math::float3 const& light_direction);
			
			void			propagate_lighting					(u32 const cascade_index);
			
			void			register_light_constans				();
			void			register_rsm_constans				();
			
			void			set_rsm_contants					(xray::math::float3 const& light_direction, 
																 xray::math::float3 const& grid_origin, 
																 float grid_scale);
			void			set_light_constans					(xray::render::light* l);
			
			void			render_quad							();
			
			radiance_volume* m_radiance_volume;
			
			u32				m_num_cascades;
			
			u32				m_rsm_source_size;
			u32				m_rsm_downsampled_size;
			u32				m_grid_size;
			
			bool			m_has_indirect_lighting;
			
			ref_rt			m_rt_rms_albedo_source;
			ref_texture		m_t_rms_albedo_source;

			ref_rt			m_rt_downsampled_scene;
			ref_texture		m_t_downsampled_scene;
			
			ref_rt			m_rt_rms_normal_source;
			ref_texture		m_t_rms_normal_source;
			
			ref_rt			m_rt_rms_position_source;
			ref_texture		m_t_rms_position_source;
			
			
			ref_rt			m_rt_rms_albedo;
			ref_texture		m_t_rms_albedo;
			
			ref_rt			m_rt_rms_normal;
			ref_texture		m_t_rms_normal;
			
			ref_rt			m_rt_rms_position;
			ref_texture		m_t_rms_position;
			
			ref_rt			m_rms_depth_stencil_source;
			
			ref_effect		m_fill_rsm_effect[num_vertex_input_types];
			
			ref_effect		m_downsample_rsm_effect;
			ref_effect		m_apply_indirect_lighting_effect;
			ref_effect		m_downsample_gbuffer_effect;
					
			untyped_buffer_ptr		m_screen_vertex_ib;
			ref_geometry			m_screen_vertex_geometry;
			
			shader_constant_host*	m_c_radiance_blend_factor;
			
			shader_constant_host*	m_c_light_type;
			shader_constant_host*	m_c_light_color;
			shader_constant_host*	m_c_light_intensity;
			shader_constant_host*	m_c_light_position;
			shader_constant_host*	m_c_light_direction;
			shader_constant_host*	m_c_light_attenuation_power;
			shader_constant_host*	m_c_light_range;
			shader_constant_host*	m_c_view_to_light_matrix;
			shader_constant_host*	m_c_diffuse_influence_factor;
			shader_constant_host*	m_c_specular_influence_factor;
			shader_constant_host*	m_c_light_spot_penumbra_half_angle_cosine;
			shader_constant_host*	m_c_light_spot_umbra_half_angle_cosine;
			shader_constant_host*	m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine;
			shader_constant_host*	m_c_light_spot_falloff;
			shader_constant_host*	m_c_light_capsule_half_width;
			shader_constant_host*	m_c_light_capsule_radius;
			shader_constant_host*	m_c_light_sphere_radius;
			shader_constant_host*	m_c_lighting_model;
			shader_constant_host*	m_c_light_local_to_world;
			shader_constant_host*	m_c_near_far;
			
			shader_constant_host*	m_c_grid_origin;
			shader_constant_host*	m_c_grid_cell_size;
			shader_constant_host*	m_c_invert_rsm_size;
			
			shader_constant_host*	m_c_interreflection_contribution;
			shader_constant_host*	m_c_cascade_index;
			shader_constant_host*	m_c_num_cascades;
			
			shader_constant_host*	m_c_smaller_cascade_grid_cell_size;
			shader_constant_host*	m_c_smaller_cascade_grid_size;
			shader_constant_host*	m_c_smaller_cascade_grid_origin;
			
			shader_constant_host*	m_c_grid_size;
			shader_constant_host*	m_c_eye_ray_corner;
			
}; // class stage_light_propagation_volumes

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_STAGE_LIGHT_PROPAGATION_VOLUMES_H_INCLUDED
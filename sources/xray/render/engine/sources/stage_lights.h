////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_STAGE_LIGHTS_H_INCLUDED
#define XRAY_RENDER_ENGINE_STAGE_LIGHTS_H_INCLUDED

#include "stage.h"
#include "render_model.h"

namespace SpeedTree {
	class CInstance;
	struct SInstanceLod;
}
namespace xray {
namespace render {

class speedtree_tree_component;
struct lod_entry;

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

class render_particle_emitter_instance;
class light;

class untyped_buffer;
class resource_intrusive_base;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class res_geometry;
typedef	intrusive_ptr<res_geometry, resource_intrusive_base, threading::single_threading_policy>	ref_geometry;

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

class res_texture;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;


class shader_constant_host;

class stage_lights : public stage {
public:
					stage_lights			( renderer_context* context, bool is_forward_lighting_pass );
	virtual			~stage_lights			( );
	virtual void	execute					( );
	virtual void	execute_disabled		( );
	virtual	void	debug_render			( );
	
private:
			bool	is_effects_ready		( ) const;
			
			void	render_model_lighting	( render_surface_instance* instance, light* L );
			void	render_speedtree_lighting	(xray::render::lod_entry const* lod,
												 SpeedTree::CInstance const* instance,
												 SpeedTree::SInstanceLod const*	instance_lod,
												 xray::render::speedtree_tree_component* tree_component, 
												 light* L);
			
			void	render_particle_lighting( render_particle_emitter_instance* instance, light* l, u32 num_particles);
			void	render_light			( light* l, bool shadowers_pass);
			void	render_shadowed_light		( light* l);
			
			void	draw_geometry			( light* l);
			
			u32		index_to_shadow_size	( u32 size_index ) const;
private:
			void	new_sphere_geometry	( );
			void	create_pyramid_geometry	( );
			void	create_obb_geometry		( );
			
			void	fill_surface			( ref_rt surf );
			
			void	render_to_cubemap		( u32 shadow_quality, light* l );
			void	render_to_cubemap_face	( u32 face_index, math::float3 const& eye_position, float far_plane );
			
			void	make_spot_light_shadowmap( u32 shadow_quality, light* l );
			void	make_plane_spot_light_shadowmap( u32 shadow_quality, light* l );
			void	render_to_hw_shadowmap	 ( u32 shadow_quality, float z_bias, u32 smap_size, u32 smap_size_index, xray::math::float4x4 const& view_matrix, xray::math::float4x4 const& projection_matrix, u32 marge );
			
			void	make_skin_scattering_texture( xray::render::render_surface_instance* instance, light* l );
			
private:
	struct light_geometry {
		untyped_buffer_ptr	vertex_buffer;
		untyped_buffer_ptr	index_buffer;
		ref_geometry		geometry;
	}; // struct geometry
	
private:
	render_surface_instances			m_dynamic_visuals;
	render_surface_instances			m_dynamic_visuals_to_shadow;
	
	ref_rt					m_rt_skin_scattering_position;
	ref_texture				m_t_skin_scattering_position;
	
	ref_rt					m_rt_skin_scattering_temp;
	ref_texture				m_t_skin_scattering_temp;
	
	ref_rt					m_rt_skin_scattering;
 	ref_texture				m_t_skin_scattering;
	
	ref_rt					m_rt_skin_scattering_stretch;
	ref_texture				m_t_skin_scattering_stretch;
 	
 	ref_rt					m_rt_skin_scattering_small;
 	ref_texture				m_t_skin_scattering_small;
	
 	ref_rt					m_rt_skin_scattering_blurred_0;
 	ref_texture				m_t_skin_scattering_blurred_0;
 	
 	ref_rt					m_rt_skin_scattering_blurred_1;
 	ref_texture				m_t_skin_scattering_blurred_1;
	
 	ref_rt					m_rt_skin_scattering_blurred_2;
 	ref_texture				m_t_skin_scattering_blurred_2;
	
 	ref_rt					m_rt_skin_scattering_blurred_3;
 	ref_texture				m_t_skin_scattering_blurred_3;
	
 	ref_rt					m_rt_skin_scattering_blurred_4;
 	ref_texture				m_t_skin_scattering_blurred_4;
	
	ref_rt					m_skin_scattering_render_target;
	ref_rt					m_skin_scattering_depth_stencil;
	ref_texture				m_skin_scattering_texture;
	
 	ref_rt					m_shadow_depth_stencil[3];
 	ref_texture				m_shadow_depth_stencil_texture[3];
	
	ref_rt					m_vcm_render_target;
	ref_rt					m_vcm_depth_stencil;
	
	ref_rt					m_lookup_vcm_render_target[6];
	ref_rt					m_lookup_vcm_depth_stencil[6];
	ref_texture				m_lookup_vcm_texture;
	ref_texture				m_lookup_vcm_depth_stencil_texture;
	fixed_string<64>		m_lookup_vcm_render_target_names[6];
	fixed_string<64>		m_lookup_vcm_depth_stencil_names[6];
	untyped_buffer_ptr		m_lookup_vcm_ib;
	ref_geometry			m_lookup_vcm_geometry;
	
	// TODO: quality index [low(256), medium(512), high(1024)]
	ref_rt					m_cubemap_face_render_target[6];
	ref_rt					m_cubemap_face_depth_stencil[6];
	ref_texture				m_cubemap_texture;
	ref_texture				m_depth_stencil_cubemap_texture;
	
	fixed_string<64>		m_cubemap_face_render_target_names[6];
	fixed_string<64>		m_cubemap_face_depth_stencil_names[6];
	
	ref_effect				m_effect_accum_mask;
	
	ref_effect				m_point_light_shadower;
	
	ref_effect				m_point_light_accumulator;
	ref_effect				m_shadowed_point_light_accumulator;
	
	ref_effect				m_spot_light_accumulator;
	ref_effect				m_shadowed_spot_light_accumulator;
	
	ref_effect				m_capsule_light_accumulator;
	ref_effect				m_obb_light_accumulator;
	ref_effect				m_shadowed_obb_light_accumulator;
	ref_effect				m_sphere_light_accumulator;
	ref_effect				m_shadowed_sphere_light_accumulator;
	ref_effect				m_plane_spot_light_accumulator;
	ref_effect				m_shadowed_plane_spot_light_accumulator;
	
	ref_effect				m_sh_downsample_skin_irradiance_texture;
	ref_effect				m_sh_fix_irradiance_texture;
	
	
	ref_effect				m_shadow_effect;
	
	light_geometry			m_sphere_geometry;
	light_geometry			m_pyramid_geometry;
	light_geometry			m_obb_geometry;
	
	untyped_buffer_ptr		m_screen_vertex_ib;
	ref_geometry			m_screen_vertex_geometry;
	
	shader_constant_host*	m_gamma_correction_factor;
	
	shader_constant_host*	m_kernel_offsets;
	shader_constant_host*	m_blur_offsets_weights;
	
	shader_constant_host*	m_c_light_type;
	
	shader_constant_host*	m_c_light_color;
	shader_constant_host*	m_c_light_intensity;
	shader_constant_host*	m_c_light_position;
	shader_constant_host*	m_c_light_direction;
	shader_constant_host*	m_c_light_attenuation_power;
	shader_constant_host*	m_c_light_range;
	
	shader_constant_host*	m_far_fog_color_and_distance;
	shader_constant_host*	m_near_fog_distance;
	
	shader_constant_host*	m_c_diffuse_influence_factor;
	shader_constant_host*	m_c_specular_influence_factor;
	
	shader_constant_host*	m_c_is_shadower;
	
	shader_constant_host*	m_c_light_spot_penumbra_half_angle_cosine;
	shader_constant_host*	m_c_light_spot_umbra_half_angle_cosine;
	shader_constant_host*	m_c_light_spot_inversed_umbra_half_angle_cosine_minus_penumbra_half_angle_cosine;
	shader_constant_host*	m_c_light_spot_falloff;

	shader_constant_host*	m_c_light_capsule_half_width;
	shader_constant_host*	m_c_light_capsule_radius;

	shader_constant_host*	m_c_light_sphere_radius;

	shader_constant_host*	m_c_light_local_to_world;

	shader_constant_host*	m_c_eye_ray_corner;
	shader_constant_host*	m_c_near_far;
	
	shader_constant_host*	m_c_view_to_light_matrix;
	shader_constant_host*	m_c_shadow_z_bias;
	shader_constant_host*	m_c_shadow_map_size;
	shader_constant_host*	m_c_use_shadows;
	
	shader_constant_host*	m_c_shadow_transparency;
	
	shader_constant_host*	m_c_is_unwrap_pass;
	
	shader_constant_host*	m_c_lighting_model;

	shader_constant_host*	m_ambient_color;
	
	xray::math::float4x4	m_view_to_light_matrix;
	xray::math::float4x4	m_light_to_texture_matrix;
	float					m_shadow_z_bias;
	float					m_shadow_map_size;
	
	bool const				m_is_forward_lighting_pass;
}; // class stage_lights

	} // namespace render
} // namespace xray


#endif // #ifndef XRAY_RENDER_ENGINE_STAGE_LIGHTS_H_INCLUDED

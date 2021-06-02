////////////////////////////////////////////////////////////////////////////
//	Created		: 21.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_ENGINE_RADIANCE_VOLUME_H_INCLUDED
#define XRAY_RENDER_ENGINE_RADIANCE_VOLUME_H_INCLUDED

namespace xray {
namespace render {

class shader_constant_host;

class renderer_context;

class res_effect;
class resource_intrusive_base;
typedef	xray::resources::resource_ptr < xray::render::res_effect, xray::resources::unmanaged_intrusive_base > ref_effect;

class render_target;
class resource_intrusive_base;
typedef intrusive_ptr<render_target, resource_intrusive_base, threading::single_threading_policy> ref_rt;

class res_texture;
class resource_intrusive_base;
typedef intrusive_ptr<res_texture, resource_intrusive_base, threading::single_threading_policy> ref_texture;

class untyped_buffer;
class resource_intrusive_base;
typedef	intrusive_ptr<
	untyped_buffer,
	resource_intrusive_base,
	threading::single_threading_policy
> untyped_buffer_ptr;

class res_declaration;
typedef intrusive_ptr<res_declaration, resource_intrusive_base, threading::single_threading_policy>	ref_declaration;

struct injection_geometry: public boost::noncopyable
{
	injection_geometry		(u32 rsm_size);
	injection_geometry		(u32 rsm_size_x, u32 rsm_size_y);
	void					draw();
	void					prepare(u32 rsm_size_x, u32 rsm_size_y);
	
private:
	ref_declaration			m_vertext_declaration;
	untyped_buffer_ptr		m_vertex_buffer;
	u32						m_num_points;
	u32	const				m_stride;
	u32						m_rsm_size_x;
	u32						m_rsm_size_y;
}; // struct injection_geometry

struct sliced_cube_geometry: public boost::noncopyable
{
	struct sliced_cube_geometry_vertex
	{
		math::float4 position;
		math::float4 xy_and_slice_index;
	}; // struct sliced_cube_geometry_vertex

	sliced_cube_geometry	(u32 const num_cells);
	void					draw();
	
private:
	ref_declaration			m_vertext_declaration;
	untyped_buffer_ptr		m_vertex_buffer;
	untyped_buffer_ptr		m_index_buffer;
	u32	const				m_slices;
	u32						m_stride;
}; // struct sliced_cube_geometry

struct radiance_volume: public boost::noncopyable
{
						radiance_volume			(u32 const		in_rsm_size, 
											     u32 const		in_num_cells, 
												 u32 const		in_num_propagate_iterations,
												 float const	in_scale,
												 float const	in_flux_amplifier);
	
						~radiance_volume		();
	
	void				set_origin				(xray::math::float3 const& in_origin);
	math::float3 const& get_origin				() const;
	float				get_scale				() const;
	u32					get_num_cells			() const;
	
	ref_texture const&	get_radiance_r_texture	() const { return m_3d_t_radiance_r; }
	ref_texture const&	get_radiance_g_texture	() const { return m_3d_t_radiance_g; }
	ref_texture const&	get_radiance_b_texture	() const { return m_3d_t_radiance_b; }
	
	ref_texture const&  get_occluders_texture	() const { return m_3d_t_occluders; }
	
	ref_texture const&	get_previous_radiance_r_texture	() const { return m_3d_t_previous_radiance_r; }
	ref_texture const&	get_previous_radiance_g_texture	() const { return m_3d_t_previous_radiance_g; }
	ref_texture const&	get_previous_radiance_b_texture	() const { return m_3d_t_previous_radiance_b; }
	
	ref_texture const&	get_accumulated_propagation_r	() const { return m_3d_t_accumulated_propagation_r; }
	ref_texture const&	get_accumulated_propagation_g	() const { return m_3d_t_accumulated_propagation_g; }
	ref_texture const&	get_accumulated_propagation_b	() const { return m_3d_t_accumulated_propagation_b; }
	
#ifndef MASTER_GOLD
	
	void				draw_debug						(render::renderer_context* context);
	void				draw_debug_radiance				(render::renderer_context* context);
	void				draw_debug_prapagated_radiance	(render::renderer_context* context);
	void				draw_debug_radiance_occluders	(render::renderer_context* context);
	
#endif // #ifndef MASTER_GOLD
	
	void				inject_lighting			(xray::math::float3 const& light_position,
												 xray::math::float3 const& light_direction,
												 float light_fov,
												 u32 rsm_size);
	void				inject_occluders		(renderer_context* context,
												 xray::math::float3 const& light_position, 
												 xray::math::float3 const& light_direction, 
												 u32 rsm_size);
	void				inject_camera_occluders	(renderer_context* context);
	void				propagate_lighting		(u32 const cascade_index);
	bool				is_effects_ready		() const;
	void				prepare					(math::float3 const& view_position, 
												 math::float3 const& view_direction, 
												 float offset_from_center = 0.5f);
	void				prepare_gv				();
	void				fill_previous_result	();
	
private:
	
	void				begin_render_to_cells	();
	void				end_render_to_cells		();
	
	D3D_VIEWPORT		m_saved_viewport;
	
	injection_geometry		m_injection_geometry;
	injection_geometry		m_injection_geometry_from_camera;
	sliced_cube_geometry	m_sliced_cube_geometry;
	
	float				m_cell_size;
	float				m_scale;
	float				m_flux_amplifier;
	u32	const			m_num_cells;
	math::aabb			m_bbox;
	
	u32 const			m_num_propagate_iterations;
	
	ref_rt				m_3d_rt_previous_radiance_r;
	ref_rt				m_3d_rt_previous_radiance_g;
	ref_rt				m_3d_rt_previous_radiance_b;
	
	ref_texture			m_3d_t_previous_radiance_r;
	ref_texture			m_3d_t_previous_radiance_g;
	ref_texture			m_3d_t_previous_radiance_b;
	
	ref_rt				m_3d_rt_radiance_r;
	ref_rt				m_3d_rt_radiance_g;
	ref_rt				m_3d_rt_radiance_b;
	
	ref_texture			m_3d_t_radiance_r;
	ref_texture			m_3d_t_radiance_g;
	ref_texture			m_3d_t_radiance_b;
	
	ref_rt				m_3d_rt_radiance_intermediate_r;
	ref_rt				m_3d_rt_radiance_intermediate_g;
	ref_rt				m_3d_rt_radiance_intermediate_b;
	
	ref_texture			m_3d_t_radiance_intermediate_r;
	ref_texture			m_3d_t_radiance_intermediate_g;
	ref_texture			m_3d_t_radiance_intermediate_b;
	
	//
	ref_rt				m_3d_rt_accumulated_propagation_r;
	ref_rt				m_3d_rt_accumulated_propagation_g;
	ref_rt				m_3d_rt_accumulated_propagation_b;
	
	ref_texture			m_3d_t_accumulated_propagation_r;
	ref_texture			m_3d_t_accumulated_propagation_g;
	ref_texture			m_3d_t_accumulated_propagation_b;
	
	ref_rt				m_3d_rt_occluders;
	ref_texture			m_3d_t_occluders;
	
#ifndef MASTER_GOLD
	// For debugging.
	ref_texture			m_3d_t_radiance_r_lockable;
	ref_texture			m_3d_t_radiance_g_lockable;
	ref_texture			m_3d_t_radiance_b_lockable;
	ref_texture			m_3d_t_propagated_radiance_r_lockable;
	ref_texture			m_3d_t_propagated_radiance_g_lockable;
	ref_texture			m_3d_t_propagated_radiance_b_lockable;
	ref_texture			m_3d_t_occluders_lockable;
#endif // #ifndef MASTER_GOLD
	
	ref_rt				m_radiance_depth_stencil;
	
	//ref_effect			m_inject_lighting_effect;
	//ref_effect			m_propagate_lighting_effect;
	ref_effect			m_lpv_effect;
	
	shader_constant_host*	m_c_rsm_size;
	shader_constant_host*	m_c_grid_size;
	shader_constant_host*	m_c_grid_origin;
	shader_constant_host*	m_c_grid_origin_and_inv_grid_scale;
	shader_constant_host*	m_c_grid_cell_size;
	shader_constant_host*	m_c_num_grid_cells;
	shader_constant_host*	m_c_light_direction;
	shader_constant_host*	m_c_light_position;
	shader_constant_host*	m_c_propagate_iteration_index;
	shader_constant_host*	m_c_inject_flux_weight;
	shader_constant_host*	m_c_flux_amplifier;
	shader_constant_host*	m_c_eye_ray_corner;
	shader_constant_host*	m_c_cascade_index;
	shader_constant_host*	m_c_occlusion_amplifier;
	
	bool					m_is_position_changed;
	math::float3			m_prev_position;
}; // struct radiance_volume

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_ENGINE_RADIANCE_VOLUME_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 01.09.2011
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_OPTIONS2_H_INCLUDED
#define XRAY_RENDER_OPTIONS2_H_INCLUDED

#include <xray/render/core/quasi_singleton.h>
#include <xray/render/core/shader_macros.h>

namespace xray {

namespace console_commands {
	class console_command;
}

namespace render {

class render_cc
{
public:
						render_cc	(pcstr define_name);
			pcstr		define_name	() const { return m_define_name; }
private:
	friend class options;
	virtual bool		fill_macro	(shader_macro& out_macro) const = 0;
			render_cc*	render_next;
			pcstr		m_define_name;
};

class options: public quasi_singleton<options>
{
public:
			options();
	
	void	set_default_values			 ();
	void	register_console_commands	 ();
	
	void	save						 (pcstr file_name);
	void	load						 (pcstr file_name);
	
	void	load_from_config			 (xray::configs::binary_config_value const& config);

	fs_new::virtual_path_string	get_current_configuration	 ();
	
#ifndef MASTER_GOLD
	void	save_to_config				 (xray::configs::lua_config_value& config);
	void	load_from_file				 (pcstr file_name);
	void	save_to_file				 (pcstr file_name);
#endif // #ifndef MASTER_GOLD
	void	save_current_configuration	 ();
	
	void	fill_global_macros			 (shader_defines_list& out_defines);
	
private:
	friend	class render_cc;
	
	void	load_impl					 (xray::memory::reader& F);
	void	on_config_loaded			 (resources::queries_result& data);
	void	on_config_loaded2			 (resources::queries_result& data);
	
	render_cc*							 first_render_command;
	
public:
	xray::console_commands::console_command* first_command;
	xray::console_commands::console_command* last_command;
	
	bool	m_enabled_g_stage;
	bool	m_enabled_g_stage_pre_pass;
	bool	m_enabled_g_stage_material_pass;
	bool	m_enabled_decals_accumulate_stage;
	bool	m_enabled_distortion_stage;
	bool	m_enabled_sun_shadows_stage;
	bool	m_enabled_sun_stage;
	bool	m_enabled_lighting_stage;
	bool	m_enabled_ambient_occlusion_stage;
	bool	m_enabled_forward_lighting_stage;
	bool	m_enabled_deferred_lighting_stage;
	bool	m_enabled_forward_stage;
	bool	m_enabled_particles_stage;
	bool	m_enabled_post_process_stage;
	bool	m_enabled_sky_box_stage;
	bool	m_enabled_clouds_stage;
	bool	m_enabled_light_propagation_volumes_stage;
	
	// Terrain.
	bool	m_enabled_terrain_shadows;
	bool	m_enabled_draw_terrain;
	
	// SpeedTree.
	bool	m_enabled_draw_speedtree;
	bool	m_enabled_draw_speedtree_billboards;
	bool	m_enabled_draw_speedtree_branches;
	bool	m_enabled_draw_speedtree_fronds;
	bool	m_enabled_draw_speedtree_leafcards;
	bool	m_enabled_draw_speedtree_leafmeshes;
	
	bool	m_enabled_local_light_shadows;
	
	float	m_test_float_option;
	
	bool	m_use_parallax;
	bool	m_use_branching;
	bool	m_use_loop_unrolling;
	
	bool	m_enabled_fxaa;
	bool	m_enabled_mlaa;
	bool	m_use_cpu_mlaa;
	
	u32		m_organic_irradiance_texture_size;
	u32		m_shadow_map_size;
	u32		m_spot_shadow_map_size;
	u32		m_shadow_quality;
	u32		m_gbuffer_pos_packing;
	u32		m_gbuffer_normal_packing;
	
	u32		m_light_propagation_volumes_rsm_size;
	u32		m_num_radiance_volume_cells;
	float	m_radiance_volume_scale;
	u32		m_num_propagate_iterations;
	bool	m_enabled_lpv_occluders;
	
	float	m_lpv_flux_amplifier;
	float	m_lpv_interreflection_contribution;
	bool	m_lpv_movable;
	u32		m_lpv_num_cascades;
	
	bool	m_lpv_gather_occluders_from_light_view;
	bool	m_lpv_gather_occluders_from_camera_view;
	
	bool	m_lpv_disable_rsm_generating;
	bool	m_lpv_disable_rsm_downsampling;
	bool	m_lpv_disable_vpl_injection;
	bool	m_lpv_disable_gv_injection;
	bool	m_lpv_disable_propagation;
	bool	m_lpv_disable_lpv_lookup;
	u32		m_lpv_refresh_once_per_frames;
	float	m_lpv_occlusion_amplifier;
	
	bool	m_enabled_draw_models;
}; // struct options

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_OPTIONS2_H_INCLUDED
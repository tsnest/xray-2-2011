////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DX10_WORLD_H_INCLUDED
#define DX10_WORLD_H_INCLUDED

#include <xray/render/core/render_include.h>

#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include <xray/render/base/renderer_dx11.h>

#include <xray/render/core/device.h>
#include <xray/render/core/effect_manager.h>
#include <xray/render/core/resource_manager.h>
#include <xray/render/engine/material_manager.h>
#include <xray/render/core/backend.h>
#include <xray/render/engine/model_manager.h>
#include <xray/render/engine/lights_db.h>
#include <xray/render/engine/terrain.h>
#include <xray/render/engine/environment.h>
#include <xray/render/engine/system_renderer.h>
#include <xray/render/engine/scene_render.h>

namespace xray {
namespace render {
namespace dx10 {

class platform :
	public render::platform,
	public dx10::renderer
{
public:
	typedef xray::render::vector< xray::render::vertex_colored >	colored_vertices_type;
	typedef xray::render::vector< u16 >								colored_indices_type;
	typedef ui::vertex const *										ui_vertices_type;

public:
							platform				( render::engine::wrapper& wrapper, HWND window_handle );
	virtual					~platform				( );
	virtual	void			clear_resources			( );

// world methods
public:
	virtual	renderer&		extended				( );

// platform methods
public:
	virtual	void			set_view_matrix			( math::float4x4 const& view_matrix );
	virtual	void			set_projection_matrix	( xray::float4x4 const& projection );
	virtual	float4x4 const&	get_view_matrix			( ) const;
	virtual	float4x4 const&	get_projection_matrix	( ) const;
	virtual	math::rectangle<math::int2_pod> get_viewport( ) const;
	virtual	void			draw_debug_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_debug_triangles	( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_editor_triangles	( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			setup_grid_render_mode	( u32 grid_density );
	virtual	void			remove_grid_render_mode	( );
	virtual	void			setup_rotation_control_modes ( bool color_write );	
	virtual	void			draw_frame			( );
	virtual	u32				frame_id				( );
	virtual	void			add_static				( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals);
													

	virtual	void			draw_static				( ) {}
	virtual	void			add_visual				( u32 id, visual_ptr v, float4x4 const& transform, bool selected, bool system_object, bool beditor );
	virtual	void			update_visual			( u32 id, xray::math::float4x4 const& transform, bool selected );
	virtual	void			remove_visual			( u32 id );
	virtual	void			update_visual_vertex_buffer	( xray::render::visual_ptr v, vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			update_visual_index_buffer	( xray::render::visual_ptr v, vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture );
	virtual	void			add_light					( u32 id, render::light_props const& props, bool beditor );
	virtual	void			update_light				( u32 id, render::light_props const& props, bool beditor );
	virtual	void			remove_light				( u32 id, bool beditor );

	virtual	void			render_visuals				( );

	virtual	void			terrain_add_cell			( visual_ptr v, bool beditor);
	virtual	void			terrain_remove_cell			( visual_ptr v, bool beditor);
	virtual	void 			terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform);
	virtual	void 			terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id);
	virtual	void 			terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id);
	virtual	void			terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture);

	virtual	void			set_editor_render_mode		( bool draw_editor, bool draw_game );	

	virtual	void			draw_ui_vertices		( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type );
	virtual	void			clear_zbuffer			( float z_value );
	virtual	void			draw_screen_lines		( float2 const* points, u32 count, color clr, float width, u32 pattern );
	virtual	void			draw_3D_screen_lines	( float3 const* points, u32 count, color clr, float width, u32 pattern, bool use_depth );
	virtual	void 			test_cooperative_level	( );

// world methods
public:
	virtual	pcstr			type					( );

public:
	typedef render::engine::wrapper			wrapper;

private:
	
	render_dx10::device					m_device;
	render_dx10::resource_manager		m_res_mgr;
	render_dx10::backend				m_backend;
	

	render_dx10::effect_manager			m_sh_mgr;
	render_dx10::material_manager		m_mat_mgr;
	render_dx10::model_manager			m_model_mgr;
	render_dx10::terrain				m_terrain;
	render_dx10::scene_render			m_scene;
	render_dx10::lights_db				m_lights_db;
	render_dx10::environment			m_env;

	render_dx10::system_renderer		m_system_renderer;

	HWND					m_window_handle;
	render::world*			m_base;
	u32						m_frame_id;
}; // class world

} // namespace dx10
} // namespace render
} // namespace xray

#endif // #ifndef DX10_WORLD_H_INCLUDED
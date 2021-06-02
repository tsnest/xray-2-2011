////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DX9_WORLD_H_INCLUDED
#define DX9_WORLD_H_INCLUDED

#include <xray/render/dx9/renderer.h>
#include <xray/render/base/platform.h>

#include <xray/render/common/sources/d3d_defs.h>
#include <xray/render/dx9/dx9_include.h>
#include <xray/render/common/sources/hw_wrapper.h>
#include <xray/render/common/sources/render_device.h>
#include <xray/render/dx9/sources/visual.h>
#include <xray/render/dx9/sources/scene_render.h>

#include <xray/render/common/sources/environment.h>
#include <xray/render/common/sources/lights_db.h>
#include <xray/render/common/sources/terrain.h>

#include <xray/render/dx9/sources/model_manager.h>

namespace xray {
namespace render {

class render_visual;
struct render_mesh;

namespace dx9 {

class platform :
//	public xray::render::base_world,
	public xray::render::platform,
	public xray::render::dx9::renderer
{
public:
	typedef xray::render::vector< xray::render::vertex_colored >	colored_vertices_type;
	typedef xray::render::vector< u16 >								colored_indices_type;

	typedef xray::render::ui::vertex const *	ui_vertices_type;

public:
							platform					( xray::render::engine::wrapper& wrapper, HWND window_handle );
	virtual					~platform					( );
	virtual	void			clear_resources			( );
	// platform methods
public:
	virtual	void			set_view_matrix			( math::float4x4 const& view_matrix );
	virtual	void			set_projection_matrix	( xray::float4x4 const& projection );
	virtual	float4x4 const&	get_view_matrix			( ) const;
	virtual	float4x4 const&	get_projection_matrix	( ) const;
	virtual	math::rectangle<math::int2_pod> get_viewport( ) const;
	virtual	void			set_world_matrix		( xray::float4x4 const& w );
	xray::float4x4 const&	get_world_matrix		( ) const;
	virtual	void			draw_debug_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_debug_triangles	( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			clear_zbuffer			( float z_value );
	virtual	void			draw_frame				( );
	virtual	u32				frame_id				( );
	virtual	void 			test_cooperative_level	( );
	virtual	void			add_static				( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals );
	
	virtual	void			add_visual					( u32 id, xray::render::visual_ptr v, float4x4 const& transform, bool selected /*= false*/, bool system_object /*= false*/, bool beditor );
	virtual	void			update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected );
	virtual	void			remove_visual				( u32 id );
	virtual	void			update_visual_vertex_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			update_visual_index_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture );
	virtual	void			add_light					( u32 id, light_props const& props, bool beditor );
	virtual	void			update_light				( u32 id, light_props const& props, bool beditor );
	virtual	void			remove_light				( u32 id, bool beditor );
	virtual	void			render_visuals				( );

	virtual	void			terrain_add_cell			( visual_ptr v, bool beditor );
	virtual	void			terrain_remove_cell			( visual_ptr v, bool beditor );

	virtual	void 			terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform);
	virtual	void 			terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id);
	virtual	void 			terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id);
	virtual	void			terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture);


	virtual	void			draw_static				( ) { }
	virtual	void			draw_ui_vertices		( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type );

	// Editor specific functions
	virtual	void			draw_editor_lines		( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_editor_triangles	( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_screen_lines		( math::float2 const* points, u32 count, color clr, float width, u32 pattern );
	virtual	void			draw_3D_screen_lines	( math::float3 const* points, u32 count, color clr, float width, u32 pattern, bool use_depth );

	virtual	void			setup_grid_render_mode	( u32 grid_density );
	virtual	void			remove_grid_render_mode	( );
	virtual	void			setup_rotation_control_modes ( int mode, int ref_value );	

	virtual	void			set_editor_render_mode ( bool draw_editor, bool draw_game );	

	// renderer methods
public:
	virtual	pcstr			type					( );

public:
	typedef render::engine::wrapper		wrapper;

private:
			void 			create_resources		( );
			void 			destroy_resources		( );
			void 			on_ui_texture_loaded	( resources::queries_result& data );
			void 			initialize_grid_textures( );
			void 			release_grid_textures	( );
			//void			on_test_visual_loaded	( resources::queries_result& data );
			//void			test					( pcstr args );
public:
			void			setup_screen_res		( res_constant* c );
	//		void 			setup_mat_vp			( res_constant* c );
	//		void 			setup_mat_v				( res_constant* c );
	//		void 			setup_mat_wvp			( res_constant* c );

	//D3DXMATRIXA16	view_d3d;
	//D3DXMATRIXA16	projection_d3d;

	res_constant*	m_c_mat_vp;
	res_constant*	m_c_mat_wvp;
	math::float4x4	m_view_matrix;
	math::float4x4	m_proj_matrix;
	math::float4x4	m_world_matrix;

public:
	void setup_editor_selected_color(res_constant* c);

private:
	typedef render::base_world				super;

public:
	inline	IDirect3DDevice9& device			( ) const;
	inline	wrapper&		engine				( ) const;

private:
	environment				m_env;
	hw_wrapper				m_hw;
	render_device			m_render_device;
	
	//Do not change order of initialization of managers!!! 
	resource_manager		m_res_mgr;
	shader_manager			m_sh_mgr;

	backend					m_backend;

	material_manager		m_mat_mgr;
	model_manager			m_model_mgr;
	scene_render*			m_scene;
	lights_db				m_lights_db;
	terrain					m_terrain;

	IDirect3DVertexBuffer9* m_ui_vb;
	IDirect3DIndexBuffer9*	m_ui_ib;
	IDirect3DPixelShader9*	m_ui_ps;
	IDirect3DPixelShader9*	m_ui_ps_font;
	IDirect3DPixelShader9*	m_ui_ps_fill;
	IDirect3DTexture9*      m_ui_texture;
	IDirect3DTexture9*      m_ui_texture_font;

	IDirect3DPixelShader9*	m_ps_geom;
	IDirect3DVertexBuffer9* m_debug_vb;
	IDirect3DIndexBuffer9*	m_debug_ib;

	IDirect3DVertexBuffer9* m_editor_vb;
	IDirect3DIndexBuffer9*	m_editor_ib;
	float4					m_editor_selected_color;

	u32						m_render_frame_id;
	math::float4x4			m_current_view_matrix;
	ID3DXLine*				m_line;
	IDirect3DTexture9*		m_grid_texture_50;
	IDirect3DTexture9*		m_grid_texture_25;

	bool					m_render_test_scene;

}; // class world

} // namespace dx9
} // namespace render 
} // namespace xray 


#include <xray/render/base/sources/platform_dx9_inline.h>

#endif // #ifndef DX9_WORLD_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XBOX360_WORLD_H_INCLUDED
#define XBOX360_WORLD_H_INCLUDED

#include <xray/render/base/renderer_xbox360.h>
#include <xray/render/base/platform.h>
#include <xray/render/base/world.h>

#include <xray/os_preinclude.h>
#undef NOD3D
#pragma warning(push)
#pragma warning(disable:4995)
#include <xray/os_include.h>
#pragma warning(pop)

namespace xray {
namespace render {
namespace debug {
	struct vertex;
} // namespace debug

namespace xbox360 {

class platform :
	public render::platform,
	public xbox360::renderer
{
public:
	typedef xray::render::vector< xray::render::vertex_colored >	colored_vertices_type;
	typedef xray::render::vector< u16 >								colored_indices_type;
	typedef xray::render::ui::vertex const *						ui_vertices_type;

public:
	explicit				platform					( render::engine::wrapper& wrapper );
	virtual					~platform					( );
	virtual	void			clear_resources				( );

// world methods
public:
	virtual	renderer&		extended					( );

// platform methods
public:
	virtual	void			set_view_matrix				( math::float4x4 const& view_matrix );
	virtual	void			set_projection_matrix		( xray::float4x4 const& projection );
	virtual	float4x4 const&	get_view_matrix				( ) const;
	virtual	float4x4 const&	get_projection_matrix		( ) const;
	virtual	math::rectangle<math::int2_pod> get_viewport	( ) const;
	virtual	void			set_world_matrix			( xray::float4x4 const& w );
	xray::float4x4 const&	get_world_matrix			( ) const;

	virtual	void			draw_debug_lines			( vector< vertex_colored > const& vertices, vector< u16 > const& indices );
	virtual	void			draw_debug_triangles		( vector< vertex_colored > const& vertices, vector< u16 > const& indices );

	virtual	void			clear_zbuffer				( float z_value );

	virtual	void			draw_frame				( );
	virtual	u32				frame_id					( );
	virtual	void 			test_cooperative_level		( );
	virtual	void			add_static					( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals );
	virtual	void			draw_static					( );
	virtual	void			draw_ui_vertices			( ui_vertices_type vertices, u32 const & count, int prim_type, int point_type );

	virtual	void			add_visual					( u32 id, xray::render::visual_ptr v, float4x4 const& transform, bool selected, bool system_object, bool beditor );
	virtual	void			update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected );
	virtual	void			remove_visual				( u32 id );
	virtual	void			update_visual_vertex_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			update_visual_index_buffer	( xray::render::visual_ptr v, xray::vectora<xray::render::buffer_fragment> const& fragments );
	virtual	void			change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture );
	virtual	void			add_light					( u32 id, xray::render::light_props const& props, bool beditor );
	virtual	void			update_light				( u32 id, xray::render::light_props const& props, bool beditor );
	virtual	void			remove_light				( u32 id, bool beditor );

	virtual	void			render_visuals				( );

	virtual	void			terrain_add_cell			( visual_ptr v, bool beditor);
	virtual	void			terrain_remove_cell			( visual_ptr v, bool beditor);
	virtual	void 			terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform);
	virtual	void 			terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id);
	virtual	void 			terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id);
	virtual	void			terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture);

	virtual	void			set_editor_render_mode		( bool draw_editor, bool draw_game );	

	virtual	void			draw_editor_lines			( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_3D_screen_lines		( math::float3 const* points, u32 count, math::color clr, float width, u32 pattern, bool use_depth );
	virtual	void			draw_editor_triangles		( colored_vertices_type const& vertices, colored_indices_type const& indices );
	virtual	void			draw_screen_lines			( math::float2 const* points, u32 count, math::color clr, float width, u32 pattern );
	virtual	void			setup_grid_render_mode		( u32 grid_density );
	virtual	void			remove_grid_render_mode		( );
	virtual	void			setup_rotation_control_modes( int mode, int ref_value );	

// renderer methods
public:
	virtual	pcstr			type						( );

public:
	typedef render::engine::wrapper	wrapper;

public:
	inline	D3DDevice&		device						( ) const;
	inline	wrapper&		engine						( ) const;

private:
			void			create_device				( );
			void			create_vertex_shader		( );
			void			create_pixel_shader			( );
			void			create_vertex_buffer		( );
			void			create_index_buffer			( );
			void			create_vertex_declaration	( );
			void			setup_parameters			( );

private:
	float4x4				m_world;
	float4x4				m_view;
	float4x4				m_projection;
	float4x4				m_transform;
	D3DDevice*				m_device;
	Direct3D*				m_direct3d;
	IDirect3DVertexShader9*	m_vertex_shader;
	IDirect3DPixelShader9*	m_pixel_shader;
	IDirect3DVertexBuffer9*	m_vertex_buffer;
	IDirect3DIndexBuffer9*	m_index_buffer;
	IDirect3DVertexDeclaration9* m_vertex_declaration;

	u32						m_frame_id;
	bool					m_started_scene;
}; // class world

} // namespace xbox360
} // namespace render
} // namespace xray

#include "platform_xbox360_inline.h"

#endif // #ifndef XBOX360_WORLD_H_INCLUDED
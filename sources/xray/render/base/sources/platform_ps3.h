////////////////////////////////////////////////////////////////////////////
//	Created 	: 28.10.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef PS3_WORLD_H_INCLUDED
#define PS3_WORLD_H_INCLUDED

#include <xray/render/base/renderer_ps3.h>
#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>

namespace xray {
namespace render {
namespace ps3 {

class platform :
	public render::platform,
	public ps3::renderer,
	private boost::noncopyable
{
public:
					platform					( engine::wrapper& engine );
	virtual			~platform					( );
	virtual	pcstr	type						( ) { return "PS3 renderer"; }

public: // platform methods
	virtual	void	clear_resources				( );
	virtual	void	set_view_matrix				( float4x4 const& view );
	virtual	void	set_projection_matrix		( float4x4 const& view );
	virtual	float4x4 const&	get_view_matrix		( ) const;
	virtual	float4x4 const&	get_projection_matrix ( ) const;
	virtual	math::rectangle<math::int2_pod> get_viewport ( ) const;
	virtual	void	draw_debug_lines			( vector< vertex_colored > const& vertices, vector< u16 > const& indices );
	virtual	void	draw_debug_triangles		( vector< vertex_colored > const& vertices, vector< u16 > const& indices );

	virtual	void	draw_ui_vertices			( ui::vertex const * vertices, u32 const & count, int prim_type, int point_type );
	virtual	void	clear_zbuffer				( float z_value );

	virtual	void	draw_frame				( );
	virtual	u32		frame_id					( );

	// Editor specific functions
	virtual	void	draw_editor_lines			( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) { UNREACHABLE_CODE( ); }
	virtual	void	draw_editor_triangles		( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) { UNREACHABLE_CODE( ); }
	virtual	void	draw_screen_lines			( float2 const* points, u32 count, math::color clr, float width, u32 pattern ) { UNREACHABLE_CODE( ); }
	virtual	void	draw_3D_screen_lines		( float3 const* points, u32 count, math::color clr, float width, u32 pattern, bool use_depth ) { UNREACHABLE_CODE( ); }
	virtual	void	setup_grid_render_mode		( u32 grid_density ) { UNREACHABLE_CODE( ); }
	virtual	void	remove_grid_render_mode		( ) { UNREACHABLE_CODE( ); }
	virtual	void 	test_cooperative_level		( ) { }

public: // base_world methods
	virtual	void	add_static					( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals );
	virtual	void	draw_static					( );
	virtual	void	add_visual					( u32 id, visual_ptr v, float4x4 const& transform, bool selected /*= false*/, bool system_object /*= false*/, bool beditor );
	virtual	void	update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected );
	virtual	void	remove_visual				( u32 id );
	virtual	void	update_visual_vertex_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments );
	virtual	void	update_visual_index_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments );
	virtual	void	change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture );
	virtual	void	add_light					( u32 id, light_props const& props, bool beditor );
	virtual	void	update_light				( u32 id, light_props const& props, bool beditor );
	virtual	void	remove_light				( u32 id, bool beditor );

	virtual	void	render_visuals				( );

	virtual	void	terrain_add_cell			( visual_ptr v, bool beditor );
	virtual	void	terrain_remove_cell			( visual_ptr v, bool beditor );
	virtual	void 	terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform);
	virtual	void 	terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id);
	virtual	void 	terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id);
	virtual	void	terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture);

	virtual	void	set_editor_render_mode		( bool draw_editor, bool draw_game );

private:
	typedef math::rectangle<math::int2_pod>	viewport_type;

private:
	float4x4		m_view_matrix;
	float4x4		m_projection_matrix;
	viewport_type	m_viewport;
	u32				m_frame_id;
}; // class world

} // namespace ps3
} // namespace render
} // namespace xray

#endif // #ifndef PS3_WORLD_H_INCLUDED
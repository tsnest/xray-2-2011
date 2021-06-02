////////////////////////////////////////////////////////////////////////////
//	Created 	: 17.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_PLATFORM_H_INCLUDED
#define XRAY_RENDER_BASE_PLATFORM_H_INCLUDED

#include "xray/render/base/common_types.h"
#include <xray/render/base/light_props.h>
#include <xray/render/base/visual.h>
#include <xray/render/base/common_types.h>


namespace xray {
namespace render {

namespace ui {
	struct vertex;
} // namespace ui

template < typename T >
class vector;

struct XRAY_NOVTABLE platform {
	virtual	void	clear_resources					( ) = 0;
	virtual	void	set_editor_render_mode			( bool draw_editor, bool draw_game ) = 0;
	virtual	void	set_view_matrix					( float4x4 const& view ) = 0;
	virtual	void	set_projection_matrix			( float4x4 const& view ) = 0;
	virtual	float4x4 const&	get_view_matrix			( ) const = 0;
	virtual	float4x4 const&	get_projection_matrix	( ) const = 0;
	virtual	math::rectangle<math::int2_pod> get_viewport( ) const = 0;
	virtual	void	draw_debug_lines				( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_debug_triangles			( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;

	virtual	void	draw_ui_vertices				( ui::vertex const * vertices, u32 const & count, int prim_type, int point_type )	  = 0;
	virtual	void	clear_zbuffer					( float z_value )	  = 0;

	virtual	void	draw_frame					( ) = 0;
	virtual	u32		frame_id						( ) = 0;


	virtual	void				add_static					( resources::managed_resource_ptr const& collision, resources::managed_resource_ptr const& geometry, resources::managed_resource_ptr const& visuals ) = 0;
	virtual	void				draw_static					( ) = 0;
	virtual	void				add_visual					( u32 id, visual_ptr v, float4x4 const& transform, bool selected /*= false*/, bool system_object /*= false*/, bool beditor ) = 0;
	virtual	void				update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected ) = 0;
	virtual	void				remove_visual				( u32 id ) = 0;
	virtual	void				update_visual_vertex_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments ) = 0;
	virtual	void				update_visual_index_buffer	( xray::render::visual_ptr v, vectora<buffer_fragment> const& fragments ) = 0;
	virtual	void				change_visual_shader		( xray::render::visual_ptr v, char const* shader, char const* texture ) = 0;
	virtual	void				add_light					( u32 id, light_props const& props, bool beditor ) = 0;
	virtual	void				update_light				( u32 id, light_props const& props, bool beditor ) = 0;
	virtual	void				remove_light				( u32 id, bool beditor ) = 0;

	virtual	void				render_visuals				( ) = 0;

	virtual	void				terrain_add_cell			( visual_ptr v, bool beditor ) = 0;
	virtual	void				terrain_remove_cell			( visual_ptr v, bool beditor ) = 0;
	virtual	void 				terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> const& fragments, float4x4 const& transform) = 0;
	virtual	void 				terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id) = 0;
	virtual	void 				terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id) = 0;
	virtual	void				terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture) = 0;

	// Editor specific functions
	virtual	void	draw_editor_lines				( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_editor_triangles			( vector< vertex_colored > const& vertices, vector< u16 > const& indices ) = 0;
	virtual	void	draw_screen_lines				( float2 const* points, u32 count, math::color clr, float width, u32 pattern ) = 0;
	virtual	void	draw_3D_screen_lines			( float3 const* points, u32 count, math::color clr, float width, u32 pattern, bool use_depth ) = 0;
	virtual	void	setup_grid_render_mode			( u32 grid_density ) = 0;
	virtual	void	remove_grid_render_mode			( ) = 0;
	virtual	void	setup_rotation_control_modes	(  bool color_write) {XRAY_UNREFERENCED_PARAMETERS( color_write);};
	virtual	void 	test_cooperative_level			( ) = 0;

//protected:
	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( platform )
}; // class platform

} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_PLATFORM_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_BASE_EDITOR_RENDERER_H_INCLUDED
#define XRAY_RENDER_BASE_EDITOR_RENDERER_H_INCLUDED

#include <xray/render/base/world.h>
#include <xray/render/base/platform.h>
#include <xray/render/base/common_types.h>
#include <xray/render/base/light_props.h>
#include <xray/render/base/command_common.h>

namespace xray {
namespace render {

namespace debug		{	struct renderer;	}

namespace editor {


struct XRAY_NOVTABLE renderer {
public:
	virtual	debug::renderer& debug					( ) = 0;
	virtual	void 	tick							( ) = 0;

	virtual	void	set_command_push_thread_id		( ) = 0;
	virtual	void	set_command_processor_frame_id	( u32 frame_id ) = 0;
	virtual	void	initialize_command_queue		( engine::command* null_command ) = 0;
	virtual	void	purge_orders					( ) = 0;
	virtual	void	draw_frame						( ) = 0;

	virtual	void	clear_zbuffer					( float z_value ) = 0;
	virtual	void	setup_grid_render_mode			( u32 grid_density ) = 0;
	virtual	void	remove_grid_render_mode			( ) = 0;
	virtual	void setup_rotation_control_modes		( bool color_write) = 0;

	//// wireframe functions
	virtual	void 	draw_line					( math::float3 const& start_point, math::float3 const& end_point, math::color color ) = 0;
	virtual	void 	draw_lines					( vectora< vertex_colored > const &vertices, vectora< u16 > const& indices ) = 0;
	virtual	void 	draw_obb					( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void	draw_rectangle				( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_aabb					( math::float3 const& center, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_ellipse				( math::float4x4 const& matrix, math::color color ) = 0;
	virtual	void 	draw_ellipsoid				( math::float4x4 const& matrix, math::color color ) = 0;
	virtual	void 	draw_ellipsoid				( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_sphere					( math::float3 const& center, const float &radius, math::color color ) = 0;
	virtual	void 	draw_cone					( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;

	// solid functions
	virtual	void 	draw_triangle				( math::float3 const& point_0, math::float3 const& point_1, math::float3 const& point_2, math::color color ) = 0;
	virtual	void 	draw_triangle				( vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2 ) = 0;
	virtual	void 	draw_triangles				( vectora< vertex_colored > const &vertices ) = 0;
	virtual	void 	draw_triangles				( vectora< vertex_colored > const &vertices, vectora< u16 > const& indices ) = 0;

	virtual	void 	draw_rectangle_solid		( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_cube_solid				( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_cone_solid				( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_cylinder_solid			( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;
	virtual	void 	draw_ellipsoid_solid		( math::float4x4 const& matrix, math::float3 const& size, math::color color ) = 0;

	virtual	void	flush						( ) = 0;

	virtual	void	draw_screen_lines			( math::float2 const* points, u32 count, math::color cl, float width = 1.f, u32 pattern = 0xFFFFFFFF ) = 0;
	virtual	void	draw_screen_rectangle		( math::float2 left_top, math::float2 right_bottom, math::color clr, u32 pattern = 0xFFFFFFFF ) = 0;

	virtual	void	draw_3D_screen_lines		( math::float3 const* points, u32 count, math::float4x4 const& tranform, math::color clr, float width, u32 pattern = 0xFFFFFFFF, bool use_depth = true ) = 0;
	virtual	void	draw_3D_screen_line			( math::float3 start, math::float3 end,  math::color clr, float width, u32 pattern, bool use_depth = true ) = 0;
	virtual	void 	draw_3D_screen_ellipse		( math::float4x4 const& matrix, math::color clr, float width, u32 pattern = 0xFFFFFFFF, bool use_depth = true ) = 0;
	virtual	void 	draw_3D_screen_rectangle	( math::float4x4 const& matrix, math::color clr, float width, u32 pattern = 0xFFFFFFFF, bool use_depth = true ) = 0;
	virtual	void	draw_3D_screen_point		( math::float3 position, float width, math::color clr, bool use_depth = true ) = 0;

	virtual void	add_visual					( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected = false, bool system_object = false ) = 0;
	virtual void	update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected = false ) = 0;
	virtual void	remove_visual				( u32 id ) = 0;
	virtual void	render_visuals				( ) = 0;
	virtual void	update_visual_vertex_buffer	( xray::render::visual_ptr const& object, vectora<buffer_fragment> const& fragments ) = 0;
	virtual void	change_visual_shader		( xray::render::visual_ptr const& object, char const* shader, char const* texture ) = 0;

	virtual void	add_light					( u32 id, xray::render::light_props const& props) = 0;
	virtual void	update_light				( u32 id, xray::render::light_props const& props) = 0;
	virtual void	remove_light				( u32 id) = 0;

	virtual	void	terrain_add_cell			( visual_ptr v ) = 0;
	virtual	void	terrain_remove_cell			( visual_ptr v ) = 0;
	virtual	void 	terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> & fragments, float4x4 const& transform, command_finished_callback const& finish_callback, u32 finish_callback_argument) = 0;
	virtual	void 	terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id) = 0;
	virtual	void 	terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id) = 0;
	virtual	void	terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture) = 0;

	virtual	void	set_editor_render_mode		( bool draw_editor, bool draw_game) = 0;

	virtual math::float4x4	get_view_matrix		( ) = 0;
	virtual void	set_view_matrix				( math::float4x4 view_matrix ) = 0;

	virtual math::float4x4 get_projection_matrix( ) = 0;
	virtual void	set_projection_matrix		( math::float4x4 proj_matrix ) = 0;

	XRAY_DECLARE_PURE_VIRTUAL_DESTRUCTOR( renderer )
}; // struct render

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_BASE_EDITOR_RENDERER_H_INCLUDED
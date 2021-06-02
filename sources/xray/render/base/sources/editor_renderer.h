////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef EDITOR_RENDERER_H_INCLUDED
#define EDITOR_RENDERER_H_INCLUDED

#include <xray/render/base/editor_renderer.h>
#include "transform_controls.h"

namespace xray {
namespace render {

struct platform;
class command_processor;
class base_world;

namespace editor {

class editor_renderer :
	public xray::render::editor::renderer,
	private boost::noncopyable 
{
public:
	editor_renderer( xray::render::base_world& world, platform& platform );


public:
		typedef vectora< vertex_colored >				editor_vertices_type;
		typedef vectora< u16 >							editor_indices_type;

public:
		virtual			~editor_renderer				( );
		virtual	debug::debug_renderer& debug			( );
		virtual	void 	tick							( );

		virtual	void	set_command_push_thread_id		( );
		virtual	void	set_command_processor_frame_id	( u32 frame_id );
		virtual	void	initialize_command_queue		( engine::command* null_command );
		virtual	void	purge_orders					( );
		virtual	void	draw_frame						( );

		virtual	void	clear_zbuffer					( float z_value );
		virtual	void	setup_grid_render_mode			( u32 grid_density );
		virtual	void	remove_grid_render_mode			( );
		virtual	void	setup_rotation_control_modes	(  bool color_write);

		virtual	void 	draw_line			( float3 const& start_point, float3 const& end_point, color color );
		virtual	void 	draw_lines			( vectora< vertex_colored > const &vertices, vectora< u16 > const& indices );
		virtual	void 	draw_obb			( float4x4 const& matrix, float3 const& size, color color );
		virtual	void	draw_rectangle		( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_aabb			( float3 const& center, float3 const& size, color color );
		virtual	void 	draw_ellipse		( float4x4 const& matrix, color color );
		virtual	void 	draw_ellipsoid		( float4x4 const& matrix, color color );
		virtual	void 	draw_ellipsoid		( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_sphere			( float3 const& center, const float &radius, color color );
		virtual	void 	draw_cone			( float4x4 const& matrix, float3 const& size, color color );

		virtual	void 	draw_triangle		( float3 const& point_0, float3 const& point_1, float3 const& point_2, color color );
		virtual	void 	draw_triangle		( vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2 );
		virtual	void 	draw_triangle		( vertex_colored const ( &vertices )[ 3 ] );
		virtual	void 	draw_triangles		( editor_vertices_type const &vertices );
		virtual	void 	draw_triangles		( editor_vertices_type const &vertices, editor_indices_type const& indices );

		virtual	void 	draw_rectangle_solid( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_cube_solid		( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_cone_solid		( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_cylinder_solid	( float4x4 const& matrix, float3 const& size, color color );
		virtual	void 	draw_ellipsoid_solid( float4x4 const& matrix, float3 const& size, color color );

		virtual	void	flush();

		virtual	void	draw_screen_lines		( float2 const* points, u32 count, color clr, float width, u32 pattern );
		virtual	void	draw_3D_screen_line		( math::float3 start, math::float3 end,  color clr, float width, u32 pattern, bool use_depth = true );
		virtual	void	draw_screen_rectangle	( math::float2 left_top, math::float2 right_bottom, color clr, u32 pattern );
		virtual	void	draw_3D_screen_lines	( math::float3 const* points, u32 count, math::float4x4 const& tranform, color clr, float width, u32 pattern, bool use_depth = true );
		virtual	void 	draw_3D_screen_ellipse	( math::float4x4 const& matrix, color clr, float width, u32 pattern, bool use_depth = true );
		virtual	void 	draw_3D_screen_rectangle( math::float4x4 const& matrix, color clr, float width, u32 pattern, bool use_depth = true );
		virtual	void	draw_3D_screen_point	( math::float3 position, float width, color clr, bool use_depth = true );

		virtual void	add_visual					( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected = false, bool system_object = false );
		virtual void	update_visual				( u32 id, xray::math::float4x4 const& transform, bool selected = false );
		virtual void	remove_visual				( u32 id );
		virtual void	render_visuals				( );
		virtual void	update_visual_vertex_buffer	( xray::render::visual_ptr const& object, vectora<buffer_fragment> const& fragments );
		virtual void	change_visual_shader		( xray::render::visual_ptr const& object, char const* shader, char const* texture );

		virtual void	add_light					( u32 id, light_props const& props);
		virtual void	update_light				( u32 id, light_props const& props);
		virtual void	remove_light				( u32 id);

		virtual	void	terrain_add_cell			( visual_ptr v );
		virtual	void	terrain_remove_cell			( visual_ptr v );
		virtual	void 	terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> & fragments, float4x4 const& transform, command_finished_callback const& finish_callback, u32 finish_callback_argument);
		virtual	void 	terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id);
		virtual	void 	terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id);
		virtual	void	terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture);

		virtual	void	set_editor_render_mode		( bool draw_editor, bool draw_game);

		virtual math::float4x4	get_view_matrix			( );
		virtual void			set_view_matrix			( math::float4x4 view_matrix );

		virtual math::float4x4	get_projection_matrix	( );
		virtual void			set_projection_matrix	( math::float4x4 proj_matrix );

public:
		//void 	draw_lines_impl		( editor_vertices_type const &vertices, editor_indices_type const& indices );
		//void	draw_triangles_impl	( editor_vertices_type const &vertices, editor_indices_type const& indices );
		void	flush_impl			( );
		void	draw_screen_lines_impl		( float2 const* points, u32 count, color clr, float width ,u32 pattern );
		void	draw_3D_screen_lines_impl	( float3 const* points, u32 count, color clr, float width ,u32 pattern, bool use_depth );
		void	clear_zbuffer_impl			( float z_value );
		void	setup_grid_render_mode_impl	( u32 grid_density );
		void	remove_grid_render_mode_impl( );
		void	setup_rotation_control_modes_impl ( bool color_write);

private:
		inline	void 	update_lines		( u32 add_count );
				void 	render_lines		( );
		inline	void 	update_triangles	( u32 add_count );
				void 	render_triangles	( );

				void	create_primitive	( xray::vectora< float3 >& avertices, 
												xray::vectora< u16 >& aindices, 
												math::float4x4 transform, 
												float const* vertices,  
												u32 vertex_count, 
												u16 const* faces, 
												u32 index_count );

		inline  float3 	calculate_torus_vertex( float outer_radius, float inner_radius, float alpha, float beta);
private:
		typedef vector< vertex_colored >	vertices_type;
		typedef vector< u16 >				indices_type;

	struct screen_line_desc
	{
		u32		count;
		u32		clr;
		float	width;
		u32	pattern;
		bool use_depth;
	};

private:

	math::float4x4				m_view_matrix;
	math::float4x4				m_projection_matrix;

	vertices_type				m_line_vertices;
	indices_type				m_line_indices;
	vertices_type				m_triangle_vertices;
	indices_type				m_triangle_indices;

	render::vector< float2 >			m_screen_lines;
	render::vector< screen_line_desc >	m_screen_lines_desc;

	render::vector< float3 >			m_3D_screen_lines;
	render::vector< screen_line_desc >	m_3D_screen_lines_desc;

	int							m_z_holles_position;

	render::base_world&			m_world;
	platform&					m_platform;
	debug::debug_renderer*		m_debug;
}; // class renderer

} // namespace editor
} // namespace render
} // namespace xray

#endif // #ifndef EDITOR_RENDERER_H_INCLUDED
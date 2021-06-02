////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/editor_renderer.h>
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/facade/ui_renderer.h>
#include <xray/render/facade/scene_renderer.h>
#include <xray/render/engine/world.h>
#include <xray/geometry_primitives.h>
#include "functor_command.h"
#include "functor_with_big_buffer_to_copy_command.h"
#include "editor_draw_screen_lines_command.h"
#include "editor_draw_3d_screen_lines_command.h"
#include "terrain_cell_update_buffer_command.h"
#include "terrain_exchange_texture_command.h"
#include <xray/render/world.h>

#include <xray/console_command.h>
#include <xray/render/core/options.h>

using xray::render::editor::renderer;
using xray::render::vertex_colored;
using xray::render::engine::world;

namespace xray {
namespace render {

void defer_execution	( xray::render::base_command& command, xray::render::scene_ptr const& scene );

namespace editor {

renderer::renderer	(
		world& world,
		engine::world& engine_world
	):
	m_world								( world ),
	m_render_engine_world				( engine_world )//,
	//m_terrain_debug_mode				( false ),
	//m_looping_preview_particle_system	( true )
{
	m_debug				= NEW ( debug::renderer )	( world.editor_channel(), *xray::render::editor::g_allocator, engine_world );
	m_ui				= NEW ( ui::renderer )		( world.editor_channel(), *xray::render::editor::g_allocator, engine_world );
	m_scene				= NEW ( scene_renderer )	( world.editor_channel(), *xray::render::editor::g_allocator, engine_world, &m_debug->frustum );




	static xray::console_commands::cc_delegate apply_changes_cc("r_apply_changes", boost::bind( &renderer::apply_render_options_changes, this, _1 ), true );
}

void renderer::set_renderer_configuration( fs_new::virtual_path_string const& config_name )
{
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_with_big_buffer_to_copy_command< fs_new::virtual_path_string > ) (
			boost::bind(
				&engine::world::set_renderer_configuration,
				&m_render_engine_world,
				_1,
				false
			),
			config_name
		)
	);
}

void renderer::apply_render_options_changes( pcstr )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind(
				&render::engine::world::apply_render_options_changes,
				&m_render_engine_world
			)
		)
	);
}

renderer::~renderer	( )
{
	DELETE				( m_scene );
	DELETE				( m_ui );
	DELETE				( m_debug );
}

debug::renderer& renderer::debug( ) const
{
	ASSERT	( m_debug );
	return	*m_debug;
}

ui::renderer& renderer::ui( ) const
{
	ASSERT	( m_ui );
	return	*m_ui;
}

xray::render::scene_renderer& renderer::scene( ) const
{
	ASSERT	( m_scene );
	return	*m_scene;
}

void renderer::draw_screen_lines	( xray::render::scene_ptr const& scene, float2 const* points, u32 count, xray::math::color const& color, float width ,u32 pattern )
{
	m_world.editor_channel().owner_push_back( E_NEW( editor::draw_screen_lines_command ) ( m_render_engine_world, scene, points, count, color, width , pattern ) );	
}

void renderer::draw_screen_rectangle	( xray::render::scene_ptr const& scene, math::float2 const& left_top, math::float2 const& right_bottom, xray::math::color const& color, u32 const pattern )
{
	XRAY_UNREFERENCED_PARAMETERS	(pattern);
	float2 points[5];
	points[0] = left_top;
	points[1] =  float2( right_bottom.x, left_top.y );
	points[2] =  right_bottom;
	points[3] =  float2( left_top.x, right_bottom.y );
	points[4] =  left_top;

	draw_screen_lines	( scene, points, 5, color, 1.f, 0xF0F0F0F0 );
}

void renderer::clear_zbuffer	( float const z_value )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind(
				&render::engine::world::clear_zbuffer,
				&m_render_engine_world,
				z_value
			)
		)
	);
}

void renderer::setup_grid_render_mode	( u32 const grid_density )
{
	R_ASSERT									( grid_density );
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_command ) (
			boost::bind(
				&render::engine::world::setup_grid_render_mode,
				&m_render_engine_world,
				grid_density
			)
		)
	);
}

void renderer::remove_grid_render_mode	( )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind(
				&render::engine::world::remove_grid_render_mode,
				&m_render_engine_world
			)
		)
	);
}

void renderer::draw_3D_screen_lines	( xray::render::scene_ptr const& scene, math::float3 const* points, u32 count, math::float4x4 const& tranform, xray::math::color const& color, float width, u32 pattern, bool use_depth )
{
	editor::vector< math::float3 > points_3d;
	points_3d.resize( count );

	for ( u32 i = 0; i < points_3d.size(); ++i)
		points_3d[i] = tranform.transform_position(points[i]);

	m_world.editor_channel().owner_push_back( E_NEW( draw_3d_screen_lines_command ) ( m_render_engine_world, scene, &(points_3d[0]), count, color, width , pattern, use_depth ) );
}

void	renderer::draw_3D_screen_line	( xray::render::scene_ptr const& scene, math::float3 const& start, math::float3 const& end, xray::math::color const& color, float const width, u32 const pattern, bool const use_depth )
{
	float3 points_3d[] = { start, end };
	m_world.editor_channel().owner_push_back( E_NEW( draw_3d_screen_lines_command ) ( m_render_engine_world, scene, points_3d, 2, color, width , pattern, use_depth ) );
}

void renderer::draw_3D_screen_ellipse	( xray::render::scene_ptr const& scene, math::float4x4 const& matrix, xray::math::color const& color, float width, u32 pattern, bool use_depth )
{
	draw_3D_screen_lines			(
		scene,
		pointer_cast<float3 const*>( &geometry_utils::ellipse::vertices[0] ),
		geometry_utils::ellipse::vertex_count,
		matrix,
		color,
		width,
		pattern,
		use_depth);

	float3 last_line[2];
	last_line[0] = *((float3*)&geometry_utils::ellipse::vertices[0]);
	last_line[1] = *(((float3*)&geometry_utils::ellipse::vertices[0]) + geometry_utils::ellipse::vertex_count-1);

	draw_3D_screen_lines			(
		scene,
		last_line,
		2,
		matrix,
		color,
		width,
		pattern,
		use_depth
	);
}

void renderer::draw_3D_screen_rectangle( xray::render::scene_ptr const& scene, math::float4x4 const& matrix, xray::math::color const& color, float width, u32 pattern, bool use_depth )
{
	float3 arr[5];
	xray::buffer_vector<float3> verts(
		arr,
		sizeof(arr),
		pointer_cast<float3 const*>( &geometry_utils::rectangle::vertices[0] ),
		pointer_cast<float3 const*>( &geometry_utils::rectangle::vertices[0] ) + geometry_utils::rectangle::vertex_count
	);
	verts.push_back( *pointer_cast<float3 const*>( &geometry_utils::rectangle::vertices[0] ) );

	draw_3D_screen_lines			(
		scene,
		&(verts[0]),
		(u32)verts.size(),
		matrix,
		color,
		width,
		pattern,
		use_depth
	);
}

void	renderer::setup_rotation_control_modes ( bool color_write)
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind(
				&render::engine::world::setup_rotation_control_modes,
				&m_render_engine_world,
				color_write
			)
		)
	);
}

void	renderer::draw_3D_screen_point	( xray::render::scene_ptr const& scene, math::float3 const& position, float width, xray::math::color const& color, bool use_depth )
{
	m_world.editor_channel().owner_push_back(
		E_NEW( functor_with_big_buffer_to_copy_command< float3 > ) (
			boost::bind( &xray::render::engine::world::draw_3D_screen_point, &m_render_engine_world, scene, _1, color, width, use_depth ),
			position,
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void renderer::select_particle_system_instance	( scene_ptr const& scene, particle::particle_system_instance_ptr const& instance, bool selected )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::select_particle_system_instance, &m_render_engine_world, scene, instance, selected ),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void renderer::update_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps, xray::configs::lua_config_value const& config_value )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW ( functor_command ) (
			boost::bind( &engine::world::update_preview_particle_system, &m_render_engine_world, preview_ps, scene, config_value),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::update_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps, math::float4x4 const& transform )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_with_big_buffer_to_copy_command< math::float4x4 > ) (
			boost::bind( &engine::world::update_preview_particle_system_transform, &m_render_engine_world, preview_ps, scene, _1),
			transform,
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::add_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps, xray::configs::lua_config_value const& init_values, math::float4x4 const& transform )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_with_big_buffer_to_copy_command< math::float4x4 > ) (
			boost::bind( &engine::world::add_preview_particle_system, &m_render_engine_world, preview_ps, scene, init_values, _1),
			transform,
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::restart_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::restart_preview_particle_system, &m_render_engine_world, preview_ps, scene),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::set_looping_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps, bool looping )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::set_looping_preview_particle_system, &m_render_engine_world, preview_ps, scene, looping),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::remove_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::remove_preview_particle_system, &m_render_engine_world, preview_ps, scene),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void	renderer::show_preview_particle_system	( scene_ptr const& scene, xray::particle::particle_system** preview_ps )
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::show_preview_particle_system, &m_render_engine_world, preview_ps, scene),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

u32 renderer::get_num_preview_particle_system_emitters( scene_ptr const& scene, xray::particle::particle_system** preview_ps) const
{
	R_ASSERT	( scene );
	return		m_render_engine_world.get_num_preview_particle_system_emitters(preview_ps, scene);
}

void renderer::gather_preview_particle_system_statistics	( scene_ptr const& scene, xray::particle::particle_system** preview_ps, xray::particle::preview_particle_emitter_info* out_info )
{
	R_ASSERT	( scene );
	m_render_engine_world.gather_preview_particle_system_statistics(preview_ps, scene, out_info);
}

void renderer::select_speedtree_instance( scene_ptr const& scene, render::speedtree_instance_ptr const& instance, bool is_selected )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::select_speedtree_instance, &m_render_engine_world, scene, instance, is_selected ),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void renderer::select_model( scene_ptr const& scene, render_model_instance_ptr const& v, bool is_selected )
{
	m_world.editor_channel().owner_push_back  (
		E_NEW( functor_command ) (
			boost::bind( &engine::world::select_model, &m_render_engine_world, scene, v, is_selected ),
			boost::bind( &render::defer_execution, _1, scene )
		)
	);
}

void renderer::end_frame 			( )
{
	m_world.editor_channel().owner_push_back( E_NEW( functor_command ) ( boost::bind( &render::world::end_frame_editor, &m_world) ) );
}

void renderer::draw_scene			( scene_ptr const& scene, scene_view_ptr const& scene_view, render_output_window_ptr const& render_output_window, viewport_type const& viewport )
{
	R_ASSERT	( scene );
	R_ASSERT	( scene_view );
	R_ASSERT	( render_output_window );
	m_world.editor_channel().owner_push_back( E_NEW( functor_with_big_buffer_to_copy_command< viewport_type > ) ( boost::bind( &renderer::draw_scene_impl, this, scene, scene_view, render_output_window, _1), viewport ) );
}

//void renderer::on_draw_scene	( )
//{
//	//if (m_terrain_debug_mode)
//	//{
//	//	m_render_engine_world.draw_terrain_debug();
//	//}
//	
//	m_render_engine_world.setup_view_and_output( scene_view, render_output_window, viewport );
//	m_world.editor_channel().render_on_draw_scene( scene, scene_view );
//}
//
void renderer::draw_scene_impl		( scene_ptr const& scene, scene_view_ptr const& scene_view, render_output_window_ptr const& render_output_window, viewport_type const& viewport )
{
	R_ASSERT	( scene );
	R_ASSERT	( scene_view );
	R_ASSERT	( render_output_window );
	m_render_engine_world.draw_scene(
		scene,
		scene_view,
		render_output_window,
		viewport,
		boost::bind( &one_way_render_channel::render_on_draw_scene, &m_world.editor_channel(), scene, scene_view, _1 )
	);
}

void renderer::terrain_update_cell_buffer( scene_ptr const& scene, render::render_model_instance_ptr const& v, xray::vectora<terrain_buffer_fragment> & fragments, float4x4 const& transform, on_command_finished_functor_type const& on_command_finished_functor)
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back	( E_NEW( terrain_cell_update_buffer_command ) ( m_render_engine_world, scene, v, fragments, transform, on_command_finished_functor) );
}

void renderer::terrain_update_cell_aabb	( scene_ptr const& scene, render::render_model_instance_ptr const& render_model_instance, math::aabb aabb)
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_with_big_buffer_to_copy_command< math::aabb > ) (
			boost::bind(
				&engine::world::terrain_update_cell_aabb,
				&m_render_engine_world,
				scene,
				render_model_instance,
				_1
			),
			aabb
		)
	);
}

void renderer::terrain_add_cell_texture	( scene_ptr const& scene, render::render_model_instance_ptr const& render_model_instance, fs_new::virtual_path_string const& texture, u32 user_tex_id)
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_with_big_buffer_to_copy_command< fs_new::virtual_path_string > ) (
			boost::bind(
				&engine::world::terrain_add_cell_texture,
				&m_render_engine_world,
				scene,
				render_model_instance,
				_1,
				user_tex_id
			),
			texture
		)
	);
}

void renderer::terrain_remove_cell_texture( scene_ptr const& scene, render::render_model_instance_ptr const& render_model_instance, u32 user_tex_id)
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_command ) (
			boost::bind(
				&engine::world::terrain_remove_cell_texture,
				&m_render_engine_world,
				scene,
				render_model_instance,
				user_tex_id
			)
		)
	);
}

void renderer::terrain_exchange_texture	( scene_ptr const& scene, fs_new::virtual_path_string const& old_texture, fs_new::virtual_path_string const& new_texture)
{
	R_ASSERT	( scene );
	m_world.editor_channel().owner_push_back	( E_NEW( terrain_exchange_texture_command ) ( m_render_engine_world, scene, old_texture, new_texture ) );
}

void renderer::set_selection_parameters	( xray::math::float4 const& selection_color, float selection_rate )
{
	m_world.editor_channel().owner_push_back  ( E_NEW( functor_command ) ( boost::bind( &engine::world::set_selection_parameters, &m_render_engine_world, selection_color, selection_rate) ));
}

void renderer::enable_terrain_debug_mode	( bool const is_enabled )
{
	m_world.editor_channel().owner_push_back  ( E_NEW( functor_command ) ( boost::bind( &engine::world::enable_terrain_debug_mode, &m_render_engine_world, is_enabled) ));
}

void renderer::apply_material_changes( fs_new::virtual_path_string const& material_name, xray::configs::lua_config_value const& config_value )
{
	m_world.editor_channel().owner_push_back	(
		E_NEW( functor_with_big_buffer_to_copy_command< fs_new::virtual_path_string > ) (
			boost::bind(
				&engine::world::apply_material_changes,
				&m_render_engine_world,
				_1,
				config_value
			),
			material_name
		)
	);
}

} // namespace editor
} // namespace render
} // namespace xray
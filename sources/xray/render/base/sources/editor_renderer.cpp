////////////////////////////////////////////////////////////////////////////
//	Created 	: 27.01.2009
//	Author		: Armen Abroyan
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "editor_renderer.h"
#include <xray/render/base/editor_renderer.h>
#include <xray/render/base/platform.h>
#include <xray/render/base/world.h>
#include <xray/geometry_primitives.h>
#include "command_processor.h"
#include "editor_flush_command.h"
#include "editor_draw_screen_lines_command.h"
#include "editor_draw_3d_screen_lines_command.h"
#include "editor_clear_zbuffer_command.h"
#include "editor_setup_grid_render_mode_command.h"
#include "editor_setup_rotation_control_modes_command.h"
#include "command_add_visual.h"
#include "command_remove_visual.h"
#include "command_update_visual.h"
#include "command_render_visuals.h"
#include "command_update_visual_vertex_buffer.h"
#include "command_change_visual_shader.h"
#include "command_view_matrix.h"
#include "command_projection_matrix.h"
#include "command_delegate.h"
#include "debug_renderer.h"
#include "command_add_light.h"
#include "command_update_light.h"
#include "command_remove_light.h"
#include "command_terrain_add_cell.h"
#include "command_terrain_remove_cell.h"
#include "command_terrain_cell_add_texture.h"
#include "command_terrain_cell_remove_texture.h"
#include "command_terrain_exchange_texture.h"
#include "command_terrain_cell_update_buffer.h"

using xray::render::editor::editor_renderer;
using xray::render::vertex_colored;
using xray::render::platform;

using namespace xray::geometry_utils;

namespace xray {
namespace render {
namespace editor {

//static const u32 s_max_vertex_count	= 32*1024;

editor_renderer::editor_renderer	( xray::render::base_world& world, platform& platform ):
	m_world				( world ),
	m_platform			( platform )
{
	m_debug				= NEW ( debug::debug_renderer )	( world.commands_editor(), *xray::render::editor::g_allocator, platform );
}

editor_renderer::~editor_renderer	( )
{
	DELETE				( m_debug );
}

void editor_renderer::draw_line		( float3 const& start_point, float3 const& end_point, color const color )
{
	debug().draw_line	( start_point, end_point, color );
}

void editor_renderer::draw_triangle	( float3 const& point_0, float3 const& point_1, float3 const& point_2, color const color )
{
	debug().draw_triangle	( point_0, point_1, point_2, color );
}

void editor_renderer::draw_triangle	( vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2 )
{
	debug().draw_triangle	( vertex_0, vertex_1, vertex_2 );
}

void editor_renderer::draw_triangle	( vertex_colored const ( &vertices )[ 3 ] )
{
	debug().draw_triangle	( vertices );
}

void editor_renderer::draw_lines		( editor_vertices_type const &vertices, editor_indices_type const& indices )
{
	debug().draw_lines		( vertices, indices );
}

void editor_renderer::draw_obb			( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_obb		( matrix, size, color );
}

void editor_renderer::draw_rectangle ( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_rectangle	( matrix, size, color );
}

void editor_renderer::draw_aabb		( float3 const& center, float3 const& size, color const color )
{
	debug().draw_aabb		( center, size, color );
}

void editor_renderer::draw_ellipse		( float4x4 const& matrix, color const color )
{
	debug().draw_ellipse	( matrix, color );
}

void editor_renderer::draw_ellipsoid	( float4x4 const& matrix, color const color )
{
	debug().draw_ellipsoid	( matrix, color );
}

void editor_renderer::draw_ellipsoid	( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_ellipsoid	( matrix, size, color );
}

void editor_renderer::draw_sphere		( float3 const& center, const float &radius, color const color )
{
	debug().draw_sphere		( center, radius, color );
}

void editor_renderer::draw_cone ( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_cone		( matrix, size, color );
}

void editor_renderer::draw_cone_solid		( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_cone_solid	( matrix, size, color );
}

void editor_renderer::draw_rectangle_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_rectangle_solid( matrix, size, color );
}

void editor_renderer::draw_cube_solid		( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_cube_solid	( matrix, size, color );
}

void editor_renderer::draw_cylinder_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_cylinder_solid	( matrix, size, color );
}

void editor_renderer::draw_ellipsoid_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
	debug().draw_ellipsoid_solid( matrix, size, color );
}

//void editor_renderer::draw_primitive_solid	( float4x4 const& matrix, float3 const& size, float *vertices,  u32 vertex_count, u16* faces, u32 index_count, color const color )
//{
//	debug().draw_primitive_solid( matrix, size, vertices, vertex_count, faces, index_count, color );
//}
//
//void editor_renderer::draw_lines		(
//		float4x4 const& matrix,
//		float* const vertices,
//		u32 const vertex_count,
//		u16* const pairs,
//		u32 const pair_count,
//		color const color
//	)
//{
//	debug().draw_lines			( matrix, vertices, vertex_count, pairs, pair_count, color );
//}

void editor_renderer::draw_triangles	( editor_vertices_type const &vertices )
{
	debug().draw_triangles		( vertices );
}

//void editor_renderer::update_lines		( u32 add_count )
//{
//	if ( m_line_indices.size( ) + add_count >= s_max_vertex_count )
//		render_lines			( );
//
//	ASSERT						( m_line_indices.size() + add_count < s_max_vertex_count );
//}
//
//void editor_renderer::render_lines		( )
//{
//	if ( m_line_vertices.empty( ) ) {
//		ASSERT					( m_line_indices.empty( ), "lines are empty, but not the pairs" );
//		return;
//	}
//
//	ASSERT						( !m_line_indices.empty( ), "lines aren't empty, but not the pairs" );
//
//	m_platform.draw_editor_lines( m_line_vertices, m_line_indices );
//	
//	m_line_vertices.resize		( 0 );
//	m_line_indices.resize		( 0 );
//}
//
//void editor_renderer::update_triangles	( u32 const add_count )
//{
//	if ( m_triangle_vertices.size( ) + add_count >= s_max_vertex_count )
//		render_triangles		( );
//
//	ASSERT						( m_triangle_vertices.size( ) + add_count < s_max_vertex_count );
//}
//
//void editor_renderer::render_triangles	( )
//{
//	if ( m_triangle_vertices.empty( ) )
//		return;
//
//	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );
//
//	m_platform.draw_editor_triangles( m_triangle_vertices, m_triangle_indices );
//	m_triangle_vertices.resize	( 0 );
//	m_triangle_indices.resize	( 0 );
//}

//void editor_renderer::draw_lines		(
//		float4x4 const& matrix,
//		float3 const& size,
//		float* const vertices,
//		u32 const vertex_count,
//		u16* const pairs,
//		u32 const pair_count,
//		color const color
//	)
//{
//	draw_lines					(
//		math::create_scale( size ) * matrix,
//		vertices,
//		vertex_count,
//		pairs,
//		pair_count,
//		color
//	);
//}
//
void editor_renderer::tick				( )
{
	flush_impl();
}

void editor_renderer::draw_triangles	( editor_vertices_type const &vertices, editor_indices_type const& indices )
{
	debug().draw_triangles	( vertices, indices );
}

void editor_renderer::flush()
{
	m_world.commands_editor( ).push_command( E_NEW( editor::flush_command ) ( *this ) );	
}

void editor_renderer::flush_impl()
{
	debug().tick	( );
//	render_lines();
//	render_triangles();

	vector< screen_line_desc >::iterator it				= m_screen_lines_desc.begin();
	vector< screen_line_desc >::const_iterator const en = m_screen_lines_desc.end();

	int cur_position = 0;
	for( ; it != en; ++it ) 
	{
//		xray::buffer_vector< float2 > tmp_points( &m_screen_lines[cur_position], it->count );
		m_platform.draw_screen_lines( &m_screen_lines[cur_position], it->count, it->clr, it->width, it->pattern );
		cur_position += it->count;
	}
	m_screen_lines.clear();
	m_screen_lines_desc.clear();

	
	// 3D lines pass.
	{
		vector< screen_line_desc >::iterator it				= m_3D_screen_lines_desc.begin();
		vector< screen_line_desc >::const_iterator const en = m_3D_screen_lines_desc.end();

		int cur_position = 0;
		for( ; it != en; ++it ) 
		{
			m_platform.draw_3D_screen_lines( &m_3D_screen_lines[cur_position], it->count, it->clr, it->width, it->pattern, it->use_depth );
			cur_position += it->count;
		}
		m_3D_screen_lines.clear();
		m_3D_screen_lines_desc.clear();
	}
}

void editor_renderer::draw_screen_lines	( float2 const* points, u32 count, color clr, float width ,u32 pattern )
{
	m_world.commands_editor( ).push_command( E_NEW( editor::draw_screen_lines_command ) ( *this, points, count, clr, width , pattern ) );	
}

void editor_renderer::draw_screen_lines_impl	( float2 const* points, u32 count, color clr, float width ,u32 pattern )
{
	m_screen_lines.insert( m_screen_lines.end(), points, points+count );

	screen_line_desc desc;
	desc.count		= count;
	desc.clr		= clr.get_d3dcolor();
	desc.width		= width;
	desc.pattern	= pattern;
	desc.use_depth	= false;

	m_screen_lines_desc.push_back( desc );
}

void editor_renderer::draw_3D_screen_lines_impl	( float3 const* points, u32 count, color clr, float width ,u32 pattern, bool use_depth )
{
	m_3D_screen_lines.insert( m_3D_screen_lines.end(), points, points+count );

	screen_line_desc desc;
	desc.count		= count;
	desc.clr		= clr.get_d3dcolor();
	desc.width		= width;
	desc.pattern	= pattern;
	desc.use_depth	= use_depth;

	m_3D_screen_lines_desc.push_back( desc );
}

void editor_renderer::draw_screen_rectangle	( math::float2 left_top, math::float2 right_bottom, color clr, u32 pattern )
{
	XRAY_UNREFERENCED_PARAMETERS	(pattern);
	float2 points[5];
	points[0] = left_top;
	points[1] =  float2( right_bottom.x, left_top.y );
	points[2] =  right_bottom;
	points[3] =  float2( left_top.x, right_bottom.y );
	points[4] =  left_top;

	draw_screen_lines	( points, 5, clr, 1.f, 0xF0F0F0F0 );
}

void editor_renderer::clear_zbuffer	( float z_value )
{
	m_world.commands_editor( ).push_command( E_NEW( editor::clear_zbuffer_command ) ( *this, z_value ) );	
}
void editor_renderer::clear_zbuffer_impl	( float z_value )
{
	m_platform.clear_zbuffer( z_value );
}

void editor_renderer::setup_grid_render_mode	( u32 grid_density )
{
	m_world.commands_editor( ).push_command( E_NEW( editor::setup_grid_render_mode_command ) ( *this, grid_density ) );	
}

void editor_renderer::remove_grid_render_mode	( )
{
	m_world.commands_editor( ).push_command( E_NEW( editor::setup_grid_render_mode_command ) ( *this, 0 ) );	
}


void editor_renderer::setup_grid_render_mode_impl	( u32 grid_density )
{
	m_platform.setup_grid_render_mode( grid_density ) ;
}

void editor_renderer::remove_grid_render_mode_impl	( )
{
	m_platform.remove_grid_render_mode();
}


void editor_renderer::draw_3D_screen_lines	( math::float3 const* points, u32 count, math::float4x4 const& tranform, color clr, float width, u32 pattern, bool use_depth )
{
	//float4x4 proj_mat = math::mul4x4(math::mul4x4(tranform, m_view_matrix), m_projection_matrix);
	//math::rectangle<math::int2_pod> viewport = m_platform.get_viewport		( );

	//editor::vector< math::float2 > points_2d;
	//points_2d.resize( count );
	//editor::vector< math::float2 >::iterator			it_r	= points_2d.begin();

	//float4 tmp;
	//for( u32 i = 0; i < count; ++i, ++it_r )
	//{
	//	float3 const& pt = points[i];
	//	tmp.set( pt.x, pt.y, pt.z, 1 );
	//	tmp = proj_mat.transform( tmp );
	//	*it_r = math::float2( (1+tmp.x/tmp.w)*viewport.max.x/2, (1-tmp.y/tmp.w)*viewport.max.y/2 );
	//}
	//
	//draw_screen_lines	( &(points_2d[0]), points_2d.size(), clr, width, pattern );

	editor::vector< math::float3 > points_3d;
	points_3d.resize( count );

	for ( u32 i = 0; i < points_3d.size(); ++i)
		points_3d[i] = tranform.transform_position(points[i]);

	m_world.commands_editor( ).push_command( E_NEW( draw_3d_screen_lines_command ) ( *this, &(points_3d[0]), count, clr, width , pattern, use_depth ) );
}

void	editor_renderer::draw_3D_screen_line	( math::float3 start, math::float3 end,  color clr, float width, u32 pattern, bool use_depth )
{
	//float4x4 proj_mat = math::mul4x4( m_view_matrix, m_projection_matrix );
	//math::rectangle<math::int2_pod> viewport = m_platform.get_viewport		( );

	//float2 points_2d[2];
	//float4 tmp;
	//tmp.set( start.x, start.y, start.z, 1 );
	//tmp = proj_mat.transform( tmp );
	//
	//points_2d[0] = math::float2( (1+tmp.x/tmp.w)*viewport.max.x/2, (1-tmp.y/tmp.w)*viewport.max.y/2 );

	//tmp.set( end.x, end.y, end.z, 1 );
	//tmp = proj_mat.transform( tmp );

	//points_2d[1] = math::float2( (1+tmp.x/tmp.w)*viewport.max.x/2, (1-tmp.y/tmp.w)*viewport.max.y/2 );

	//draw_screen_lines	( &(points_2d[0]), 2, clr, width, pattern );

	float3 points_3d[] = { start, end };
	m_world.commands_editor( ).push_command( E_NEW( draw_3d_screen_lines_command ) ( *this, points_3d, 2, clr, width , pattern, use_depth ) );
}

void editor_renderer::draw_3D_screen_ellipse	( math::float4x4 const& matrix, color clr, float width, u32 pattern, bool use_depth )
{
	draw_3D_screen_lines			(
		( float3* )ellipse::vertices,
		ellipse::vertex_count,
		matrix,
		clr,
		width,
		pattern,
		use_depth);

	float3 last_line[2];
	last_line[0] = *((float3*)&ellipse::vertices[0]);
	last_line[1] = *(((float3*)&ellipse::vertices[0]) + ellipse::vertex_count-1);

	draw_3D_screen_lines			(
		last_line,
		2,
		matrix,
		clr,
		width,
		pattern,
		use_depth);
}

void editor_renderer::draw_3D_screen_rectangle( math::float4x4 const& matrix, color clr, float width, u32 pattern, bool use_depth )
{
	float3 arr[5];
	xray::buffer_vector<float3> verts( arr, sizeof(arr), (float3*)rectangle::vertices, (float3*)rectangle::vertices + rectangle::vertex_count);
	verts.push_back( *((float3*)rectangle::vertices) );

	draw_3D_screen_lines			(
		&(verts[0]),
		(u32)verts.size(),
		matrix,
		clr,
		width,
		pattern,
		use_depth);
}

void	editor_renderer::setup_rotation_control_modes ( bool color_write)
{
	m_world.commands_editor( ).push_command( E_NEW( editor::setup_rotation_control_modes_command ) ( *this, color_write ) );	
}

void	editor_renderer::setup_rotation_control_modes_impl ( bool color_write)
{
	m_platform.setup_rotation_control_modes( color_write);
}

void	editor_renderer::draw_3D_screen_point	( math::float3 position, float width, color const color, bool use_depth )
{
	(void)&use_depth;
	//float4x4 proj_mat = math::mul4x4( m_view_matrix, m_projection_matrix );
	//math::rectangle<math::int2_pod> viewport = m_platform.get_viewport		( );

	//float4 tmp;
	//tmp.set( position.x, position.y, position.z, 1 );
	//tmp = proj_mat.transform( tmp );
	//float2 scr_pos = math::float2( (1+tmp.x/tmp.w)*viewport.max.x/2, (1-tmp.y/tmp.w)*viewport.max.y/2 );

	//float2 shift (width/2, 0);
	//float2 points_2d[2];
	//points_2d[0] = scr_pos + shift; 
	//points_2d[1] = scr_pos - shift; 

	//draw_screen_lines	( &(points_2d[0]), 2, color, width, 0xFFFFFFFF);

	math::rectangle<math::int2_pod> viewport = m_platform.get_viewport( );

	float4x4 inv_view_proj_matrix = math::mul4x4( m_view_matrix, m_projection_matrix );;
	inv_view_proj_matrix.try_invert(inv_view_proj_matrix);

	float4x4 inv_view_matrix = m_view_matrix;
	inv_view_matrix.try_invert(inv_view_matrix);

	float dist = (float3(inv_view_matrix.e30, inv_view_matrix.e31, inv_view_matrix.e32) - position).magnitude();
	
	float3 offset_by_X = inv_view_proj_matrix.transform_direction( float3( 1.f,  0.f, 0.f ) ).normalize() * 1.0f / (float)viewport.width()  *width*0.5f * dist,
		   offset_by_Y = inv_view_proj_matrix.transform_direction( float3( 0.f, -1.f, 0.f ) ).normalize() * 1.0f / (float)viewport.height() *width*0.5f * dist;
	
	float3 vertices[4] = {
		(position - offset_by_X - offset_by_Y), 
		(position - offset_by_X + offset_by_Y), 
		(position + offset_by_X + offset_by_Y), 
		(position + offset_by_X - offset_by_Y)};

	draw_triangle(vertices[2],vertices[1],vertices[0],color);
	draw_triangle(vertices[3],vertices[2],vertices[0],color);
}

//void editor_renderer::draw_lines_impl		( editor_vertices_type const &vertices, editor_indices_type const& indices )
//{
//	ASSERT						( indices.size( ) % 2 == 0 );
//	update_lines				( indices.size( ) / 2 );
//
//	u16	const n					= ( u16 ) m_line_vertices.size( );
//	m_line_vertices.insert		( m_line_vertices.end(), vertices.begin( ), vertices.end( ) );
//
//	editor_indices_type::const_iterator	i = indices.begin( ); 
//	editor_indices_type::const_iterator	e = indices.end( );
//	for ( ; i != e; ++i )
//		m_line_indices.push_back		( n + *i );
//}
//
//void editor_renderer::draw_triangles_impl	( editor_vertices_type const &vertices, editor_indices_type const& indices )
//{
//	update_triangles			( indices.size( ) );
//
//	u16	const n					= ( u16 ) m_triangle_vertices.size( );
//	m_triangle_vertices.insert	( m_triangle_vertices.end(), vertices.begin( ), vertices.end( ) );
//
//	editor_indices_type::const_iterator	i = indices.begin( ); 
//	editor_indices_type::const_iterator	e = indices.end( );
//	for ( ; i != e; ++i )
//		m_triangle_indices.push_back	( n + *i );
//
//	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );
//}

void editor_renderer::add_visual			( u32 id, xray::render::visual_ptr v, xray::math::float4x4 const& transform, bool selected, bool system_object )
{
	m_world.commands_editor( ).push_command	( E_NEW( command_add_visual ) ( m_world, id, v, transform, selected, system_object, true ) );
}

void editor_renderer::remove_visual			( u32 id ) 
{
	m_world.commands_editor( ).push_command	( E_NEW( command_remove_visual ) ( m_world, id ) );
}

void editor_renderer::update_visual			( u32 id, xray::math::float4x4 const& transform, bool selected ) 
{
	m_world.commands_editor( ).push_command	( E_NEW( command_update_visual ) ( m_world, id, transform, selected  ) );
}

void editor_renderer::render_visuals			(  ) 
{
	m_world.commands_editor( ).push_command	( E_NEW( command_render_visuals ) ( m_world ) );
}

void editor_renderer::update_visual_vertex_buffer	( xray::render::visual_ptr const& object, vectora<buffer_fragment> const& fragments )
{
	m_world.commands_editor( ).push_command	( E_NEW( command_update_visual_vertex_buffer ) ( m_world, object, fragments, *xray::render::editor::g_allocator ) );
}

void editor_renderer::change_visual_shader		( xray::render::visual_ptr const& object, char const* shader, char const* texture )
{
	m_world.commands_editor( ).push_command	( E_NEW( command_change_visual_shader ) ( m_world, object, shader, texture ) );
}

math::float4x4	editor_renderer::get_view_matrix			( )
{
	return m_platform.get_view_matrix();
}

void	editor_renderer::set_view_matrix			( math::float4x4 view_matrix )
{
	m_view_matrix = view_matrix;
	m_world.commands_editor( ).push_command	( E_NEW( command_view_matrix ) ( m_platform, m_view_matrix ) );
}

math::float4x4	editor_renderer::get_projection_matrix	( )
{
	return m_platform.get_projection_matrix();
}

void	editor_renderer::set_projection_matrix	( math::float4x4 proj_matrix )
{
	m_projection_matrix = proj_matrix;
	m_world.commands_editor( ).push_command	( E_NEW( command_projection_matrix ) ( m_platform, m_projection_matrix ) );
}

void editor_renderer::set_command_push_thread_id	( )
{
	m_world.commands_editor( ).set_push_thread_id ( );
}

void editor_renderer::set_command_processor_frame_id	( u32 frame_id )
{
	m_world.commands_editor( ).set_frame_id( frame_id );
}

void editor_renderer::draw_frame		( )
{
	m_world.commands_editor().push_command	( E_NEW( command_delegate ) ( boost::bind( &xray::render::base_world::draw_frame_editor, &m_world  ) ) );
}

void editor_renderer::purge_orders		( )
{
	m_world.commands_editor().purge_orders	( );
}

debug::debug_renderer& editor_renderer::debug	( )
{
	ASSERT	( m_debug );
	return	*m_debug;
}

void editor_renderer::initialize_command_queue	( engine::command* null_command )
{
	m_world.commands_editor().initialize_queue	( null_command );
}

void editor_renderer::add_light				( u32 id, light_props const& props)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_add_light ) ( m_world, id, props, true) );
}

void editor_renderer::update_light				( u32 id, light_props const& props)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_update_light ) ( m_world, id, props, true) );
}

void editor_renderer::remove_light				( u32 id)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_remove_light ) ( m_world, id, true) );
}

void editor_renderer::terrain_add_cell			( visual_ptr v )
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_add_cell ) ( m_world, v, true) );
}

void editor_renderer::terrain_remove_cell		( visual_ptr v)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_remove_cell ) ( m_world, v, true) );
}

void editor_renderer::terrain_update_cell_buffer	( visual_ptr v, xray::vectora<buffer_fragment_NEW> & fragments, float4x4 const& transform, command_finished_callback const& finish_callback, u32 finish_callback_argument)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_cell_update_buffer ) ( m_world, v, fragments, transform, finish_callback, finish_callback_argument) );
}

void editor_renderer::terrain_add_cell_texture	( visual_ptr v, texture_string const & texture, u32 user_tex_id)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_cell_add_texture ) ( m_world, v, texture, user_tex_id ) );
}

void editor_renderer::terrain_remove_cell_texture	( visual_ptr v, u32 user_tex_id)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_cell_remove_texture ) ( m_world, v, user_tex_id ) );
}

void editor_renderer::terrain_exchange_texture	( texture_string const & old_texture, texture_string const & new_texture)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_terrain_exchange_texture ) ( m_world, old_texture, new_texture ) );
}

void editor_renderer::set_editor_render_mode	( bool draw_editor, bool draw_game)
{
	m_world.commands_editor( ).push_command	( E_NEW( command_delegate ) ( boost::bind(&xray::render::base_world::set_editor_render_mode, &m_world, draw_editor, draw_game)) );
}

} // namespace editor
} // namespace render
} // namespace xray
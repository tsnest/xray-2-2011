////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/render/facade/debug_renderer.h>
#include <xray/render/engine/world.h>
#include "debug_draw_lines_command.h"
#include "debug_draw_triangles_command.h"
#include <xray/render/world.h>
#include <xray/geometry_primitives.h>

using xray::render::vertex_colored;
using xray::render::debug::renderer;
using xray::render::engine::world;

using xray::float3;
using xray::float4x4;
using xray::math::color;
using xray::render::scene_ptr;

static u32 const max_debug_vertices_count	= 64*1024;

renderer::renderer			( xray::render::one_way_render_channel& channel, xray::memory::base_allocator& allocator, xray::render::engine::world& engine_world ) :
	m_render_engine_world		( engine_world ),
	m_channel					( channel ),
	m_allocator					( allocator )
{
}

void renderer::draw_line	( scene_ptr const& scene, float3 const& start_point, float3 const& end_point, xray::math::color const& color, bool const use_depth )
{
	vertex_colored const vertices[2]	= { vertex_colored( start_point, color ), vertex_colored( end_point, color ) };
	u16 const indices[2]		= { 0, 1 };
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( scene, m_render_engine_world, m_allocator, vertices, indices, use_depth ) );
}

void  renderer::draw_origin( scene_ptr const& scene, float4x4 const& matrix, float const half_size, bool const use_depth )
{
	//math::aabb bb = (math::aabb().identity()*half_size).modify(matrix);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	math::color const clr_red	( 0xff, 0x00, 0x00, 0xff );
	math::color const clr_green ( 0x00, 0xff, 0x00, 0xff );
	math::color const clr_blue	( 0x00, 0x00, 0x0ff, 0xff );
	math::color const clr_white ( 0xff, 0xff, 0xff, 0xff );

	vertex_colored const vertices[6]= 
	{ 
		vertex_colored( float3(-half_size, 0, 0)* matrix, clr_white ), vertex_colored( float3(half_size, 0, 0)* matrix, clr_red ), // x- axis
		vertex_colored( float3(0, -half_size, 0)* matrix, clr_white ), vertex_colored( float3(0, half_size, 0)* matrix, clr_green ), // y- axis
		vertex_colored( float3(0, 0, -half_size)* matrix, clr_white ), vertex_colored( float3(0, 0, half_size)* matrix, clr_blue ), // z- axis
	};
	u16 const indices[]			= { 0, 1, 2, 3, 4, 5 };
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( scene, m_render_engine_world, m_allocator, vertices, indices, use_depth ) );
}

void  renderer::draw_cross( scene_ptr const& scene, float3 const& p, float const half_size, xray::math::color const& color, bool const use_depth )
{
	//math::aabb bb = (math::aabb().identity()*2*half_size).move(p);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	vertex_colored const vertices[6]= 
	{ 
		vertex_colored( p + float3( -half_size, 0, 0 ), color ),	vertex_colored( p + float3( half_size, 0, 0 ), color ),
		vertex_colored( p + float3( 0, -half_size, 0 ), color ),	vertex_colored( p + float3( 0, half_size, 0 ), color ),
		vertex_colored( p + float3( 0, 0, -half_size ), color ),	vertex_colored( p + float3( 0, 0, half_size ), color ),
	};
	u16 const indices[]		= { 0, 1, 2, 3, 4, 5 };
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( scene, m_render_engine_world, m_allocator, vertices, indices, use_depth ) );
}

void renderer::draw_triangle	( scene_ptr const& scene, float3 const& point_0, float3 const& point_1, float3 const& point_2, xray::math::color const& color, bool const use_depth )
{
	vertex_colored const vertices [ 3 ]	=
	{
		vertex_colored( point_0, color ),
		vertex_colored( point_1, color ),
		vertex_colored( point_2, color )
	};
	draw_triangle				( scene, vertices, use_depth );
}

void renderer::draw_triangle	( scene_ptr const& scene, vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2, bool const use_depth )
{
	vertex_colored const vertices [ 3 ]	=
	{
		vertex_0,
		vertex_1,
		vertex_2
	};
	draw_triangle				( scene, vertices, use_depth );
}

void renderer::draw_triangle	( scene_ptr const& scene, vertex_colored const ( &vertices )[ 3 ], bool const use_depth )
{
	u16 const indices[3]		= { 0, 1, 2 };
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( scene, m_render_engine_world, m_allocator, vertices, indices, use_depth ) );
}

void renderer::draw_cube			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	//math::aabb bb = (math::aabb().identity()*size).modify(matrix);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::obb::vertices,
		geometry_utils::obb::vertex_count,
		(u16*)geometry_utils::obb::pairs,
		geometry_utils::obb::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_frustum	( 
		scene_ptr const& scene,
		float const fov_in_radians,
		float const near_plane_distance,
		float const far_plane_distance,
		float const aspect_ratio,
		float3 const& position,
		float3 const& direction,
		float3 up_vector,
		color const& color,
		bool use_depth
	)
{
	float y_fov					= fov_in_radians;
	float x_fov					= fov_in_radians * aspect_ratio;

	float window_right_coord	= math::tan( x_fov / 2.f );
	float window_left_coord		= -window_right_coord;
	float window_top_coord		= math::tan( y_fov / 2.f );
	float window_bottom_coord	= -window_top_coord;

	CURE_ASSERT					( math::is_similar( direction.length(), 1.f, math::epsilon_5 ), return, "given direction vector isn't normalized" );
	
	float3 const& right_vector	= normalize( up_vector ^ direction );
	up_vector					= normalize( direction ^ right_vector );
	float3 const& offset		= position + direction;
	
	float3 const corner_vertices[] =
	{
		offset + right_vector * window_right_coord + up_vector * window_top_coord,
		offset + right_vector * window_right_coord + up_vector * window_bottom_coord,
		offset + right_vector * window_left_coord  + up_vector * window_top_coord,
		offset + right_vector * window_left_coord  + up_vector * window_bottom_coord
	};

	float3 const projectors[]		=
	{
		normalize( corner_vertices[0] - position ),
		normalize( corner_vertices[1] - position ),
		normalize( corner_vertices[2] - position ),
		normalize( corner_vertices[3] - position )
	};

	float const distance_to_vertex	= far_plane_distance / ( direction | normalize( projectors[0] ) );
	float3 const far_plane_points[] =
	{
		position + projectors[0] * distance_to_vertex,
		position + projectors[1] * distance_to_vertex,
		position + projectors[2] * distance_to_vertex,
		position + projectors[3] * distance_to_vertex
	};

	if ( near_plane_distance == 0.f )
	{
		draw_line				( scene, position, far_plane_points[0], color, use_depth );
		draw_line				( scene, position, far_plane_points[1], color, use_depth );
		draw_line				( scene, position, far_plane_points[2], color, use_depth );
		draw_line				( scene, position, far_plane_points[3], color, use_depth );
	}
	else
	{
		float const distance_to_vertex	= near_plane_distance / ( direction | normalize( projectors[0] ) );
		float3 const near_plane_points[] =
		{
			position + projectors[0] * distance_to_vertex,
			position + projectors[1] * distance_to_vertex,
			position + projectors[2] * distance_to_vertex,
			position + projectors[3] * distance_to_vertex
		};
		draw_line				( scene, near_plane_points[0], near_plane_points[1], color, use_depth );
		draw_line				( scene, near_plane_points[1], near_plane_points[3], color, use_depth );
		draw_line				( scene, near_plane_points[3], near_plane_points[2], color, use_depth );
		draw_line				( scene, near_plane_points[2], near_plane_points[0], color, use_depth );

		draw_line				( scene, near_plane_points[0], far_plane_points[0], color, use_depth );
		draw_line				( scene, near_plane_points[1], far_plane_points[1], color, use_depth );
		draw_line				( scene, near_plane_points[2], far_plane_points[2], color, use_depth );
		draw_line				( scene, near_plane_points[3], far_plane_points[3], color, use_depth );
	}
	
	draw_line					( scene, far_plane_points[0], far_plane_points[1], color, use_depth );
	draw_line					( scene, far_plane_points[1], far_plane_points[3], color, use_depth );
	draw_line					( scene, far_plane_points[3], far_plane_points[2], color, use_depth );
	draw_line					( scene, far_plane_points[2], far_plane_points[0], color, use_depth );
}

void renderer::draw_frustum_solid	( 
		scene_ptr const& scene,
		float const fov_in_radians,
		float const near_plane_distance,
		float const far_plane_distance,
		float const aspect_ratio,
		float3 const& position,
		float3 const& direction,
		float3 up_vector,
		color const& color,
		bool use_depth
	)
{
	float y_fov					= fov_in_radians;
	float x_fov					= fov_in_radians * aspect_ratio;

	float window_right_coord	= math::tan( x_fov / 2.f );
	float window_left_coord		= -window_right_coord;
	float window_top_coord		= math::tan( y_fov / 2.f );
	float window_bottom_coord	= -window_top_coord;

	CURE_ASSERT					( math::is_similar( direction.length(), 1.f, math::epsilon_5 ), return, "given direction vector isn't normalized" );
	
	float3 const& right_vector	= normalize( up_vector ^ direction );
	up_vector					= normalize( direction ^ right_vector );
	float3 const& offset		= position + direction;
	
	float3 const corner_vertices[] =
	{
		offset + right_vector * window_right_coord + up_vector * window_top_coord,
		offset + right_vector * window_right_coord + up_vector * window_bottom_coord,
		offset + right_vector * window_left_coord  + up_vector * window_top_coord,
		offset + right_vector * window_left_coord  + up_vector * window_bottom_coord
	};

	float3 const projectors[]		=
	{
		normalize( corner_vertices[0] - position ),
		normalize( corner_vertices[1] - position ),
		normalize( corner_vertices[2] - position ),
		normalize( corner_vertices[3] - position )
	};

	float const distance_to_vertex	= far_plane_distance / ( direction | normalize( projectors[0] ) );
	float3 const far_plane_points[] =
	{
		position + projectors[0] * distance_to_vertex,
		position + projectors[1] * distance_to_vertex,
		position + projectors[2] * distance_to_vertex,
		position + projectors[3] * distance_to_vertex
	};

	if ( near_plane_distance == 0.f )
	{
		// right triangles
		draw_triangle			( scene, position, far_plane_points[0], far_plane_points[1], color, use_depth );
		draw_triangle			( scene, position, far_plane_points[1], far_plane_points[0], color, use_depth );
		// left triangles
		draw_triangle			( scene, position, far_plane_points[2], far_plane_points[3], color, use_depth );
		draw_triangle			( scene, position, far_plane_points[3], far_plane_points[2], color, use_depth );
		// top triangles
		draw_triangle			( scene, position, far_plane_points[0], far_plane_points[2], color, use_depth );
		draw_triangle			( scene, position, far_plane_points[2], far_plane_points[0], color, use_depth );
		// bottom triangles
		draw_triangle			( scene, position, far_plane_points[1], far_plane_points[3], color, use_depth );
		draw_triangle			( scene, position, far_plane_points[3], far_plane_points[1], color, use_depth );
	}
	else
	{
		float const distance_to_vertex	= near_plane_distance / ( direction | normalize( projectors[0] ) );
		float3 const near_plane_points[] =
		{
			position + projectors[0] * distance_to_vertex,
			position + projectors[1] * distance_to_vertex,
			position + projectors[2] * distance_to_vertex,
			position + projectors[3] * distance_to_vertex
		};

		// right triangles
		draw_triangle			( scene, near_plane_points[0], far_plane_points[0], far_plane_points[1], color, use_depth );
		draw_triangle			( scene, near_plane_points[0], far_plane_points[1], far_plane_points[0], color, use_depth );
		draw_triangle			( scene, far_plane_points[1], near_plane_points[0], near_plane_points[1], color, use_depth );
		draw_triangle			( scene, far_plane_points[1], near_plane_points[1], near_plane_points[0], color, use_depth );
		// left triangles
		draw_triangle			( scene, near_plane_points[2], far_plane_points[2], far_plane_points[3], color, use_depth );
		draw_triangle			( scene, near_plane_points[2], far_plane_points[3], far_plane_points[2], color, use_depth );
		draw_triangle			( scene, far_plane_points[3], near_plane_points[2], near_plane_points[3], color, use_depth );
		draw_triangle			( scene, far_plane_points[3], near_plane_points[3], near_plane_points[2], color, use_depth );
		// top triangles
		draw_triangle			( scene, near_plane_points[0], far_plane_points[0], far_plane_points[2], color, use_depth );
		draw_triangle			( scene, near_plane_points[0], far_plane_points[2], far_plane_points[0], color, use_depth );
		draw_triangle			( scene, far_plane_points[2], near_plane_points[0], near_plane_points[2], color, use_depth );
		draw_triangle			( scene, far_plane_points[2], near_plane_points[2], near_plane_points[0], color, use_depth );
		// bottom triangles
		draw_triangle			( scene, near_plane_points[1], far_plane_points[1], far_plane_points[3], color, use_depth );
		draw_triangle			( scene, near_plane_points[1], far_plane_points[3], far_plane_points[1], color, use_depth );
		draw_triangle			( scene, far_plane_points[3], near_plane_points[1], near_plane_points[3], color, use_depth );
		draw_triangle			( scene, far_plane_points[3], near_plane_points[3], near_plane_points[1], color, use_depth );
		// near plane triangles
		draw_triangle			( scene, near_plane_points[0], near_plane_points[1], near_plane_points[2], color, use_depth );
		draw_triangle			( scene, near_plane_points[0], near_plane_points[2], near_plane_points[1], color, use_depth );
		draw_triangle			( scene, near_plane_points[3], near_plane_points[1], near_plane_points[2], color, use_depth );
		draw_triangle			( scene, near_plane_points[3], near_plane_points[2], near_plane_points[1], color, use_depth );
	}
	
	// far plane triangles
	draw_triangle				( scene, far_plane_points[0], far_plane_points[1], far_plane_points[2], color, use_depth );
	draw_triangle				( scene, far_plane_points[0], far_plane_points[2], far_plane_points[1], color, use_depth );
	draw_triangle				( scene, far_plane_points[3], far_plane_points[1], far_plane_points[2], color, use_depth );
	draw_triangle				( scene, far_plane_points[3], far_plane_points[2], far_plane_points[1], color, use_depth );
}

void renderer::draw_rectangle ( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, color const& color, bool const use_depth )
{
	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::rectangle::vertices,
		geometry_utils::rectangle::vertex_count,
		geometry_utils::rectangle::pairs,
		geometry_utils::rectangle::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_aabb		( scene_ptr const& scene, float3 const& center, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_cube					(
		scene,
		math::create_translation(
			center
		),	
		size,
		color,
		use_depth
	);
}

void renderer::draw_ellipse	( scene_ptr const& scene, float4x4 const& matrix, xray::math::color const& color, bool const use_depth )
{
	draw_lines					(
		scene,
		matrix,
		geometry_utils::ellipse::vertices,
		geometry_utils::ellipse::vertex_count,
		geometry_utils::ellipse::pairs,
		geometry_utils::ellipse::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_ellipsoid	( scene_ptr const& scene, float4x4 const& matrix, xray::math::color const& color, bool const use_depth )
{
	draw_lines					(
		scene,
		matrix,
		geometry_utils::ellipsoid::vertices,
		geometry_utils::ellipsoid::vertex_count,
		geometry_utils::ellipsoid::pairs,
		geometry_utils::ellipsoid::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_ellipsoid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::ellipsoid::vertices,
		geometry_utils::ellipsoid::vertex_count,
		geometry_utils::ellipsoid::pairs,
		geometry_utils::ellipsoid::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_circle	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::circle::vertices,
		geometry_utils::circle::vertex_count,
		geometry_utils::circle::pairs,
		geometry_utils::circle::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_cylinder	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	//math::aabb bb = (math::aabb().identity()*size).modify(matrix);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::cylinder::vertices,
		geometry_utils::cylinder::vertex_count,
		geometry_utils::cylinder::pairs,
		geometry_utils::cylinder::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_sphere		( scene_ptr const& scene, float3 const& center, const float &radius, xray::math::color const& color, bool const use_depth )
{
	math::sphere sp(center, radius);
	if(frustum.test(sp)==math::intersection_outside)
		return;

	draw_ellipsoid				(
		scene,
		math::create_translation( center ),
		float3( radius, radius, radius ),
		color,
		use_depth
	);
}

void renderer::draw_cone			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	math::sphere sp(matrix.c.xyz(), math::max(size.x, size.y) );
	if(frustum.test(sp)==math::intersection_outside)
		return;

	draw_lines					(
		scene,
		matrix,
		size,
		geometry_utils::cone::vertices,
		geometry_utils::cone::vertex_count,
		geometry_utils::cone::pairs,
		geometry_utils::cone::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_cone_solid		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	math::sphere sp(matrix.c.xyz(), math::max(size.x, size.y) );
	if(frustum.test(sp)==math::intersection_outside)
		return;

	draw_primitive_solid		( 
		scene,
		// this is temporary, coordinates of the primitive need to be updated.
		math::create_scale( float3(0.5, 1, 0.5))*matrix, 
		size, 
		geometry_utils::cone_solid::vertices,  
		geometry_utils::cone_solid::vertex_count, 
		geometry_utils::cone_solid::faces, 
		geometry_utils::cone_solid::index_count, 
		color,
		use_depth
	);
}

void renderer::draw_rectangle_solid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_primitive_solid		( 
		scene,
		matrix, 
		size, 
		geometry_utils::rectangle_solid::vertices,  
		geometry_utils::rectangle_solid::vertex_count, 
		geometry_utils::rectangle_solid::faces, 
		geometry_utils::rectangle_solid::index_count, 
		color,
		use_depth
	);
}
void renderer::draw_cube_solid		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	//math::aabb bb = (math::aabb().identity()*size).modify(matrix);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	draw_primitive_solid		( 
		scene,
		matrix, 
		size, 
		geometry_utils::cube_solid::vertices,  
		geometry_utils::cube_solid::vertex_count, 
		geometry_utils::cube_solid::faces, 
		geometry_utils::cube_solid::index_count, 
		color,
		use_depth
	);
}

void renderer::draw_cylinder_solid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_primitive_solid		( 
		scene,
		// this is temporary, coordinates of the primitive need to be updated.
		math::create_scale( float3(0.5, 1, 0.5))*matrix, 
		//matrix,
		size, 
		geometry_utils::cylinder_solid::vertices,  
		geometry_utils::cylinder_solid::vertex_count, 
		geometry_utils::cylinder_solid::faces, 
		geometry_utils::cylinder_solid::index_count, 
		color,
		use_depth
	);
}

void renderer::draw_sphere_solid		( scene_ptr const& scene, float3 const& center, const float& radius, color const& color, bool const use_depth )
{
	math::sphere sp(center, radius);
	if(frustum.test(sp)==math::intersection_outside)
		return;

	draw_ellipsoid_solid		(
		scene,
		math::create_translation( center ),
		float3( radius, radius, radius ),
		color,
		use_depth
	);
}

void renderer::draw_ellipsoid_solid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, color const& color, bool const use_depth )
{
 	draw_primitive_solid		( 
		scene,
 		matrix, 
		size, 
		geometry_utils::ellipsoid_solid::vertices,  
		geometry_utils::ellipsoid_solid::vertex_count, 
		geometry_utils::ellipsoid_solid::faces, 
		geometry_utils::ellipsoid_solid::index_count, 
		color,
		use_depth
	);
}

void renderer::draw_primitive_solid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, float const* const vertices,  u32 vertex_count, u16 const* const faces, u32 const index_count, xray::math::color const& color, bool const use_depth )
{
	float4x4 trnsform = create_scale(size)*matrix;

	debug_vertices_type temp_vertices(m_allocator);
	temp_vertices.resize		( vertex_count);

	vertex_colored temp_vertex;
	temp_vertex.color = color;

	for( u32 i = 0, j = 0; i< vertex_count; ++i, j+=3) 
	{
		temp_vertex.position.x = vertices[j];
		temp_vertex.position.y = vertices[j+1];
		temp_vertex.position.z = vertices[j+2];

		temp_vertex.position = trnsform.transform_position( temp_vertex.position );
		temp_vertices[i] = temp_vertex;
	}

	debug_indices_type temp_indices(m_allocator);
	temp_indices.resize	( index_count);

	for( u32 i = 0; i< index_count; ++i) 
		temp_indices[i] = faces[i];

	draw_triangles	( scene, temp_vertices, temp_indices, use_depth );
}

void renderer::draw_arrow		( scene_ptr const& scene, float3 const& start_point, float3 const& end_point, xray::math::color const& line_color, xray::math::color const& cone_color, bool const use_depth )
{
	float3 direction			= end_point - start_point;
	if ( math::is_zero( direction.squared_length(), math::epsilon_3 ) )
		return;

	//math::aabb bb = create_aabb_min_max(start_point, start_point);
	//bb.modify(end_point);
	//if(frustum.test_inexact(bb)==math::intersection_outside)
	//	return;

	draw_line					( scene, start_point, end_point, line_color, use_depth );
	
	float						length = ( start_point - end_point ).length( );
	float						size = length / 20.f;
	float3						sizes = float3( .5f * size, size, .5f * size );
	
	direction					/= length;

	float3 const up_vector		( 0.f, 1.f, 0.f );
	float4x4 matrix;
	if ( !math::is_zero( math::length( up_vector ^ direction ) ) )
		matrix					= math::create_rotation_x( -math::pi_d2 ) * math::create_rotation( direction, up_vector );
	else
	{
		if ( ( up_vector | direction ) > 0.f )
			matrix				= math::create_rotation_y( math::pi_d2 );
		else
			matrix				= math::create_rotation_x( -math::pi ) * math::create_rotation_y( -math::pi_d2 );
	}

	matrix.c.xyz()				= end_point;
	draw_cone					( scene, matrix, sizes, cone_color, use_depth );
}

void renderer::draw_arrow		( scene_ptr const& scene, float3 const& start_point, float3 const& end_point, xray::math::color const& color, bool const use_depth )
{
	draw_arrow					(
		scene,
		start_point,
		end_point,
		color,
		color,
		use_depth
	);
}

void renderer::draw_line_ellipsoid( scene_ptr const& scene, float4x4 const& matrix, xray::math::color const& color, bool const use_depth )
{
	draw_lines(
		scene,
		matrix,
		geometry_utils::line_ellipsoid::vertices_xy,
		geometry_utils::line_ellipsoid::vertex_count,
		geometry_utils::line_ellipsoid::pairs,
		geometry_utils::line_ellipsoid::pair_count,
		color,
		use_depth
	);

	draw_lines(
		scene,
		matrix,
		geometry_utils::line_ellipsoid::vertices_yz,
		geometry_utils::line_ellipsoid::vertex_count,
		geometry_utils::line_ellipsoid::pairs,
		geometry_utils::line_ellipsoid::pair_count,
		color,
		use_depth
	);

	draw_lines(
		scene,
		matrix,
		geometry_utils::line_ellipsoid::vertices_xz,
		geometry_utils::line_ellipsoid::vertex_count,
		geometry_utils::line_ellipsoid::pairs,
		geometry_utils::line_ellipsoid::pair_count,
		color,
		use_depth
	);
}

void renderer::draw_line_hemisphere( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, bool bdraw_base, xray::math::color const& color, bool const use_depth )
{
	// base is XY plane
	draw_lines(
		scene,
		matrix,
		size,
		geometry_utils::line_arc::vertices_xy,
		geometry_utils::line_arc::vertex_count,
		geometry_utils::line_arc::pairs,
		geometry_utils::line_arc::pair_count,
		color,
		use_depth
	);

	draw_lines(
		scene,
		math::create_rotation_y(math::pi_d2)* matrix,
		size,
		geometry_utils::line_arc::vertices_xy,
		geometry_utils::line_arc::vertex_count,
		geometry_utils::line_arc::pairs,
		geometry_utils::line_arc::pair_count,
		color,
		use_depth
	);

	if(bdraw_base)
	{
		draw_lines(
			scene,
			matrix,
			size,
			geometry_utils::line_ellipsoid::vertices_xz,
			geometry_utils::line_ellipsoid::vertex_count,
			geometry_utils::line_ellipsoid::pairs,
			geometry_utils::line_ellipsoid::pair_count,
			color,
			use_depth
		);
	}
}

void renderer::draw_line_capsule( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_cylinder		( scene, matrix, size, color, use_depth );

	draw_line_hemisphere( scene, math::create_translation(float3(0, size.y, 0)) * matrix, 
							float3(size.x, size.x, size.z), 
							false, 
							color,
							use_depth);

	draw_line_hemisphere( scene, math::create_translation(float3(0, -size.y, 0)) * matrix, 
							float3(size.x, -size.x, size.z), 
							false, 
							color,
							use_depth);

}

void renderer::draw_solid_capsule( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, xray::math::color const& color, bool const use_depth )
{
	draw_cylinder_solid	( scene, matrix, size, color, use_depth );

	draw_sphere_solid	( scene, matrix.c.xyz( ) + matrix.j.xyz( ) * size.y, 
							size.x, 
							color,
							use_depth);

	draw_sphere_solid	( scene, matrix.c.xyz( ) - matrix.j.xyz( ) * size.y, 
							size.x, 
							color,
							use_depth);

}

void renderer::draw_lines( scene_ptr const& scene, xray::vectora< vertex_colored > const& vertices, xray::vectora< u16 > const& indices, bool const use_depth )
{
	R_ASSERT_CMP				( vertices.size(), <=, max_debug_vertices_count );
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( m_allocator, scene, m_render_engine_world, vertices, indices, use_depth ) );
}

void renderer::draw_lines(
		scene_ptr const& scene,
		xray::vectora< vertex_colored >::const_iterator start_point,
		xray::vectora< vertex_colored >::const_iterator end_point,
		xray::vectora< u16 > const& indices,
		bool use_depth
	)
{
	R_ASSERT_CMP				( end_point - start_point, <=, max_debug_vertices_count );
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( scene, m_render_engine_world, m_allocator, start_point, end_point, indices, use_depth ) );
}

void renderer::draw_lines		(
		scene_ptr const& scene,
		float4x4 const& matrix,
		float const* const vertices,
		u32 const vertex_count,
		u16 const* const pairs,
		u32 const pair_count,
		xray::math::color const& color,
		bool use_depth
	)
{
	R_ASSERT_CMP				( vertex_count, <=, max_debug_vertices_count );

	typedef xray::buffer_vector< u16 >	TempIndices;
	TempIndices temp_indices	( ALLOCA( 2 * pair_count * sizeof( u16 ) ), 2 * pair_count, pairs, pairs + 2*pair_count );
	
	typedef xray::buffer_vector< vertex_colored >	TempVertices;
	TempVertices temp_vertices	( ALLOCA( vertex_count * sizeof( vertex_colored ) ), vertex_count );
	temp_vertices.resize		( vertex_count );

	TempVertices::iterator		i = temp_vertices.begin( );
	TempVertices::iterator		e = temp_vertices.end( );
	for ( u32 j = 0; i != e; ++i, j += 3 ) {
		( *i )					= 
			vertex_colored(
				float3(
					vertices[ j ],
					vertices[ j + 1 ],
					vertices[ j + 2 ]
				) * matrix,
				color
			);
	}

	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( scene, m_render_engine_world, m_allocator, temp_vertices, temp_indices, use_depth ) );
}

void renderer::draw_triangles	( scene_ptr const& scene, xray::render::debug_vertices_type const &vertices, bool const use_depth )
{
	R_ASSERT_CMP				( vertices.size(), <=, max_debug_vertices_count );

	typedef xray::buffer_vector< vertex_colored >	TempVertices;
	TempVertices temp_vertices	( ALLOCA( vertices.size( ) * sizeof( vertex_colored ) ), vertices.size( ), vertices.begin( ), vertices.end( ) );

	typedef xray::buffer_vector< u16 >	TempIndices;
	TempIndices temp_indices	( ALLOCA( vertices.size( ) * sizeof( u16 ) ), vertices.size( ) );
	temp_indices.resize			( vertices.size( ) );
	
	TempIndices::iterator		i = temp_indices.begin( );
	TempIndices::iterator		e = temp_indices.end( );
	for ( u16 j = 0; i != e; ++i, ++j )
		*i						= j;
	
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( scene, m_render_engine_world, m_allocator, temp_vertices, temp_indices, use_depth ) );
}

void renderer::draw_triangles	( scene_ptr const& scene, xray::render::debug_vertices_type const &vertices, xray::render::debug_indices_type const& indices, bool const use_depth )
{
	R_ASSERT_CMP				( vertices.size(), <=, max_debug_vertices_count );
	m_channel.owner_push_back	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( scene, m_render_engine_world, vertices, indices, use_depth ) );
}

void renderer::draw_lines		(
		scene_ptr const& scene,
		float4x4 const& matrix,
		float3 const& size,
		float const* const vertices,
		u32 const vertex_count,
		u16 const* const pairs,
		u32 const pair_count,
		xray::math::color const& color,
		bool use_depth
	)
{
	R_ASSERT_CMP				( vertex_count, <=, max_debug_vertices_count );
	draw_lines					(
		scene,
		math::create_scale( size ) * matrix,
		vertices,
		vertex_count,
		pairs,
		pair_count,
		color,
		use_depth
	);
}
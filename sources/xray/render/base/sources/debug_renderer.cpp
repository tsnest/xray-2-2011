////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "debug_renderer.h"
#include <xray/render/base/debug_renderer.h>
#include <xray/render/base/platform.h>
#include "command_processor.h"
#include "debug_draw_lines_command.h"
#include "debug_draw_triangles_command.h"
#include <xray/render/base/world.h>
#include <xray/geometry_primitives.h>

using xray::render::vertex_colored;
using xray::render::debug::debug_renderer;
using xray::render::platform;
using xray::render::command_processor;
using xray::float3;
using xray::float4x4;

using namespace xray::geometry_utils;

static const u32 s_max_vertex_count	= 32*1024;

debug_renderer::debug_renderer		( command_processor& processor, xray::memory::base_allocator& allocator, platform& platform ) :
	m_platform					( platform ),
	m_processor					( processor ),
	m_allocator					( allocator )
{
}

inline vertex_colored create_vertex	( xray::float3 const& position, u32 const color )
{
	vertex_colored const result			= { position, color };
	return						( result );
}

void debug_renderer::draw_line		( float3 const& start_point, float3 const& end_point, u32 const color )
{
	vertex_colored const vertices[2]	= { { start_point, color }, { end_point, color } };
	u16 const indices[2]		= { 0, 1 };
	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( *this, vertices, indices ) );
}

void debug_renderer::draw_line		( float3 const& start_point, float3 const& end_point, color const color )
{
	draw_line					( start_point, end_point, color.get_d3dcolor() );
}

void debug_renderer::draw_triangle	( float3 const& point_0, float3 const& point_1, float3 const& point_2, u32 const color )
{
	vertex_colored const vertices [ 3 ]	=
	{
		{ point_0, color },
		{ point_1, color },
		{ point_2, color }
	};
	draw_triangle				( vertices );
}

void debug_renderer::draw_triangle	( float3 const& point_0, float3 const& point_1, float3 const& point_2, color const color )
{
	draw_triangle				( point_0, point_1, point_2, color.get_d3dcolor() );
}

void debug_renderer::draw_triangle	( vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2 )
{
	vertex_colored const vertices [ 3 ]	=
	{
		vertex_0,
		vertex_1,
		vertex_2
	};
	draw_triangle				( vertices );
}

void debug_renderer::draw_triangle	( vertex_colored const ( &vertices )[ 3 ] )
{
	u16 const indices[3]		= { 0, 1, 2 };
	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( *this, vertices, indices ) );
}

void debug_renderer::draw_obb			( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_lines					(
		matrix,
		size,
		(float*)obb::vertices,
		obb::vertex_count,
		(u16*)obb::pairs,
		obb::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_rectangle ( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_lines					(
		matrix,
		size,
		( float* )rectangle::vertices,
		rectangle::vertex_count,
		( u16* )rectangle::pairs,
		rectangle::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_aabb		( float3 const& center, float3 const& size, color const color )
{
	draw_obb					(
		math::create_translation(
			center
		),	
		size,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_ellipse		( float4x4 const& matrix, color const color )
{
	draw_lines					(
		matrix,
		( float* )ellipse::vertices,
		ellipse::vertex_count,
		( u16* )ellipse::pairs,
		ellipse::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_ellipsoid	( float4x4 const& matrix, color const color )
{
	draw_lines					(
		matrix,
		( float* )ellipsoid::vertices,
		ellipsoid::vertex_count,
		( u16* )ellipsoid::pairs,
		ellipsoid::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_ellipsoid	( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_lines					(
		matrix,
		size,
		(float*)ellipsoid::vertices,
		ellipsoid::vertex_count,
		(u16*)ellipsoid::pairs,
		ellipsoid::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_sphere		( float3 const& center, const float &radius, color const color )
{
	draw_ellipsoid				(
		math::create_translation( center ),
		float3( radius, radius, radius ),
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_cone ( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_lines					(
		matrix,
		size,
		( float* )cone::vertices,
		cone::vertex_count,
		( u16* )cone::pairs,
		cone::pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::draw_cone_solid		( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_primitive_solid		( 
		// this is temporary, coordinates of the primitive need to be updated.
		math::create_scale( float3(0.5, 1, 0.5))*matrix, 
		size, 
		(float*) cone_solid::vertices,  
		cone_solid::vertex_count, 
		(u16*) cone_solid::faces, 
		cone_solid::index_count, 
		color
	);
}

void debug_renderer::draw_rectangle_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_primitive_solid		( 
		matrix, 
		size, 
		(float*)rectangle_solid::vertices,  
		rectangle_solid::vertex_count, 
		(u16*) rectangle_solid::faces, 
		rectangle_solid::index_count, 
		color
	);
}
void debug_renderer::draw_cube_solid		( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_primitive_solid		( 
		matrix, 
		size, 
		(float*) cube_solid::vertices,  
		cube_solid::vertex_count, 
		(u16*) cube_solid::faces, 
		cube_solid::index_count, 
		color
	);
}

void debug_renderer::draw_cylinder_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
	draw_primitive_solid		( 
		// this is temporary, coordinates of the primitive need to be updated.
		math::create_scale( float3(0.5, 1, 0.5))*matrix, 
		size, 
		(float*) cylinder_solid::vertices,  
		cylinder_solid::vertex_count, 
		(u16*) cylinder_solid::faces, 
		cylinder_solid::index_count, 
		color
	);
}

void debug_renderer::draw_ellipsoid_solid	( float4x4 const& matrix, float3 const& size, color const color )
{
 	draw_primitive_solid		( 
 		matrix, 
		size, 
		(float*) ellipsoid_solid::vertices,  
		ellipsoid_solid::vertex_count, 
		(u16*) ellipsoid_solid::faces, 
		ellipsoid_solid::index_count, 
		color
	);
}

void debug_renderer::draw_primitive_solid	( float4x4 const& matrix, float3 const& size, float *vertices,  u32 vertex_count, u16* faces, u32 index_count, color const color )
{
	float4x4 trnsform = create_scale(size)*matrix;

	debug_vertices_type temp_vertices(m_allocator);
	temp_vertices.resize		( vertex_count);

	vertex_colored temp_vertex;
	temp_vertex.color = color.get_d3dcolor();

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

	draw_triangles	( temp_vertices, temp_indices);
}

void debug_renderer::draw_arrow		( float3 const& start_point, float3 const& end_point, color line_color, color cone_color )
{

	float3 direction			= end_point - start_point;
	if ( math::is_zero( direction.square_magnitude( ), math::epsilon_3 ) )
		return;

	draw_line					( start_point, end_point, line_color );
	
	float						distance = ( start_point - end_point ).magnitude( );
	float						size = distance/20.f;
	float3						sizes = float3( .5f*size, size, .5f*size );
	
	direction.normalize			( );

	float3 const up_vector		( 0.f, 1.f, 0.f );
	float4x4 matrix;
	if ( !math::is_zero( magnitude( up_vector ^ direction ) ) )
		matrix					= math::create_rotation_x( math::pi_d2 ) * math::create_rotation ( direction, float3( 0.f, 1.f, 0.f ) );
	else {
		if ( (up_vector | direction) > 0.f )
			matrix				= math::create_rotation_x( math::pi_d2 );
		else
			matrix				= math::create_rotation_x(  -math::pi_d2 );
	}

	matrix.c.xyz()				= start_point + direction * ( distance - size );
	draw_cone					( matrix, sizes, cone_color );
}

void debug_renderer::draw_arrow		( float3 const& start_point, float3 const& end_point, color const color )
{
	draw_arrow					(
		start_point,
		end_point,
		color,
		color
	);
}

void debug_renderer::draw_lines		( xray::vectora< vertex_colored > const& vertices, xray::vectora< u16 > const& indices )
{
	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( *this, vertices, indices ) );
}

void debug_renderer::draw_lines		(
		float4x4 const& matrix,
		float* const vertices,
		u32 const vertex_count,
		u16* const pairs,
		u32 const pair_count,
		color const color
	)
{
	typedef xray::buffer_vector< u16 >	TempIndices;
	TempIndices temp_indices	( ALLOCA( 2 * pair_count * sizeof( u16 ) ), 2 * pair_count, pairs, pairs + 2*pair_count );
	
	typedef xray::buffer_vector< vertex_colored >	TempVertices;
	TempVertices temp_vertices	( ALLOCA( vertex_count * sizeof( vertex_colored ) ), vertex_count );
	temp_vertices.resize		( vertex_count );

	u32 const d3d_color			= color.get_d3dcolor( );

	TempVertices::iterator		i = temp_vertices.begin( );
	TempVertices::iterator		e = temp_vertices.end( );
	for ( u32 j = 0; i != e; ++i, j += 3 ) {
		( *i )					= 
			create_vertex(
				float3(
					vertices[ j ],
					vertices[ j + 1 ],
					vertices[ j + 2 ]
				) * matrix,
				d3d_color
			);
	}

	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_lines_command ) ( *this, temp_vertices, temp_indices ) );
}

void debug_renderer::draw_triangles	( debug_vertices_type const &vertices )
{
	typedef xray::buffer_vector< vertex_colored >	TempVertices;
	TempVertices temp_vertices	( ALLOCA( vertices.size( ) * sizeof( vertex_colored ) ), vertices.size( ), vertices.begin( ), vertices.end( ) );

	typedef xray::buffer_vector< u16 >	TempIndices;
	TempIndices temp_indices	( ALLOCA( vertices.size( ) * sizeof( u16 ) ), vertices.size( ) );
	temp_indices.resize			( vertices.size( ) );
	
	TempIndices::iterator		i = temp_indices.begin( );
	TempIndices::iterator		e = temp_indices.end( );
	for ( u16 j = 0; i != e; ++i, ++j )
		*i						= j;
	
	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( *this, temp_vertices, temp_indices ) );
}

void debug_renderer::draw_triangles	( debug_vertices_type const &vertices, debug_indices_type const& indices )
{
	m_processor.push_command	( XRAY_NEW_IMPL( m_allocator, debug::draw_triangles_command ) ( *this, vertices, indices ) );
}

void debug_renderer::update_lines		( u32 add_count )
{
	if ( m_line_indices.size( ) + add_count >= s_max_vertex_count )
		render_lines			( );

	ASSERT						( m_line_indices.size() + add_count < s_max_vertex_count );
}

void debug_renderer::render_lines		( )
{
	if ( m_line_vertices.empty( ) ) {
		ASSERT					( m_line_indices.empty( ), "lines are empty, but not the pairs" );
		return;
	}

	ASSERT						( !m_line_indices.empty( ), "lines aren't empty, but not the pairs" );

	m_platform.draw_debug_lines		( m_line_vertices, m_line_indices );
	
	m_line_vertices.resize		( 0 );
	m_line_indices.resize		( 0 );
}

void debug_renderer::update_triangles	( u32 const add_count )
{
	if ( m_triangle_vertices.size( ) + add_count >= s_max_vertex_count )
		render_triangles		( );

	ASSERT						( m_triangle_vertices.size( ) + add_count < s_max_vertex_count );
}

void debug_renderer::render_triangles	( )
{
	if ( m_triangle_vertices.empty( ) )
		return;

	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );

	m_platform.draw_debug_triangles	( m_triangle_vertices, m_triangle_indices );
	m_triangle_vertices.resize	( 0 );
	m_triangle_indices.resize	( 0 );
}

void debug_renderer::draw_lines		(
		float4x4 const& matrix,
		float3 const& size,
		float* const vertices,
		u32 const vertex_count,
		u16* const pairs,
		u32 const pair_count,
		color const color
	)
{
	draw_lines					(
		math::create_scale( size ) * matrix,
		vertices,
		vertex_count,
		pairs,
		pair_count,
		color.get_d3dcolor()
	);
}

void debug_renderer::tick				( )
{
	render_lines				( );
	render_triangles			( );
}

void debug_renderer::draw_lines_impl		( debug_vertices_type const &vertices, debug_indices_type const& indices )
{
	ASSERT						( indices.size( ) % 2 == 0 );
	update_lines				( indices.size( ) / 2 );

	u16	const n					= ( u16 ) m_line_vertices.size( );
	m_line_vertices.insert		( m_line_vertices.end(), vertices.begin( ), vertices.end( ) );

	debug_indices_type::const_iterator	i = indices.begin( ); 
	debug_indices_type::const_iterator	e = indices.end( );
	for ( ; i != e; ++i )
        m_line_indices.push_back		( n + *i );
}

void debug_renderer::draw_triangles_impl	( debug_vertices_type const &vertices, debug_indices_type const& indices )
{
	update_triangles			( indices.size( ) );

	u16	const n					= ( u16 ) m_triangle_vertices.size( );
	m_triangle_vertices.insert	( m_triangle_vertices.end(), vertices.begin( ), vertices.end( ) );

	debug_indices_type::const_iterator	i = indices.begin( ); 
	debug_indices_type::const_iterator	e = indices.end( );
	for ( ; i != e; ++i )
        m_triangle_indices.push_back	( n + *i );

	ASSERT						( ( m_triangle_indices.size( ) % 3 ) == 0, "triangle indices count isn't divisible by 3" );
}
////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef DEBUG_RENDERER_H_INCLUDED
#define DEBUG_RENDERER_H_INCLUDED

#include <xray/render/base/debug_renderer.h>

namespace xray {
namespace render {

struct platform;
class command_processor;
class base_world;

namespace debug {

class debug_renderer :
	public xray::render::debug::renderer,
	private boost::noncopyable
{
public:
	typedef vectora< vertex_colored >	debug_vertices_type;
	typedef vectora< u16 >				debug_indices_type;

public:
					debug_renderer		( command_processor& processor, memory::base_allocator& allocator, platform& platform );
	virtual	void 	tick				( );

	virtual	void 	draw_line			( float3 const& start_point, float3 const& end_point, color color );
	virtual	void	draw_line			( float3 const& start_point, float3 const& end_point, u32 color );
	virtual	void 	draw_obb			( float4x4 const& matrix, float3 const& size, color color );
	virtual	void	draw_rectangle		( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_aabb			( float3 const& center, float3 const& size, color color );
	virtual	void 	draw_ellipse		( float4x4 const& matrix, color color );
	virtual	void 	draw_ellipsoid		( float4x4 const& matrix, color color );
	virtual	void 	draw_ellipsoid		( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_sphere			( float3 const& center, const float &radius, color color );
	virtual	void 	draw_cone			( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_arrow			( float3 const& start_point, float3 const& end_point, color arrow_color, color cone_color );
	virtual	void 	draw_arrow			( float3 const& start_point, float3 const& end_point, color color );
	virtual	void 	draw_lines			( xray::vectora< vertex_colored > const& vertices, xray::vectora< u16 > const& indices );

	virtual	void 	draw_triangle		( float3 const& point_0, float3 const& point_1, float3 const& point_2, color color );
	virtual	void	draw_triangle		( float3 const& point_0, float3 const& point_1, float3 const& point_2, u32 color );
	virtual	void 	draw_triangle		( vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2 );
	virtual	void 	draw_triangle		( vertex_colored const ( &vertices )[ 3 ] );
	virtual	void 	draw_triangles		( debug_vertices_type const &vertices );
	virtual	void 	draw_triangles		( debug_vertices_type const &vertices, debug_indices_type const& indices );

	virtual	void 	draw_rectangle_solid( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_cube_solid		( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_cone_solid		( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_cylinder_solid	( float4x4 const& matrix, float3 const& size, color color );
	virtual	void 	draw_ellipsoid_solid( float4x4 const& matrix, float3 const& size, color color );

	memory::base_allocator&	get_allocator() {return m_allocator;}

public:
			void 	draw_lines_impl		( debug_vertices_type const &vertices, debug_indices_type const& indices );
			void 	draw_triangles_impl	( debug_vertices_type const &vertices, debug_indices_type const& indices );
	
private:
	inline	void 	update_lines		( u32 add_count );
			void 	render_lines		( );
	inline	void 	update_triangles	( u32 add_count );
			void 	render_triangles	( );
			void 	draw_lines			( float4x4 const& matrix, float *vertices, u32 vertex_count, u16* pairs,  u32 pair_count, color color );
			void 	draw_lines			( float4x4 const& matrix, float3 const& size, float *vertices,  u32 vertex_count, u16* pairs, u32 pair_count, color color );

			void 	draw_primitive_solid( float4x4 const& matrix, float3 const& size, float *vertices,  u32 vertex_count, u16* faces, u32 index_count, color color );

			void	create_primitive	( xray::vectora< float3 >& avertices, 
											xray::vectora< u16 >& aindices, 
											math::float4x4 transform, 
											float const* vertices,  
											u32 vertex_count, 
											u16 const* faces, 
											u32 index_count );

private:
	typedef vector< vertex_colored >	vertices_type;
	typedef vector< u16 >				indices_type;

private:
	vertices_type				m_line_vertices;
	indices_type					m_line_indices;
	vertices_type				m_triangle_vertices;
	indices_type					m_triangle_indices;
	platform&				m_platform;
	command_processor&		m_processor;
	memory::base_allocator&	m_allocator;
}; // class world

} // namespace debug
} // namespace render
} // namespace xray

#endif // #ifndef DEBUG_RENDERER_H_INCLUDED
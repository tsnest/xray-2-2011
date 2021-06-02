////////////////////////////////////////////////////////////////////////////
//	Created 	: 13.11.2008
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RENDER_FACADE_DEBUG_RENDERER_H_INCLUDED
#define XRAY_RENDER_FACADE_DEBUG_RENDERER_H_INCLUDED

#include <xray/render/api.h>
#include <xray/render/engine/base_classes.h>
#include <xray/render/engine/vertex_colored.h>

namespace xray {
namespace render {

class one_way_render_channel;

namespace engine {
	class world;
} // namespace engine

typedef vectora< vertex_colored >		debug_vertices_type;
typedef vectora< u16 >					debug_indices_type;

namespace editor {
	class renderer;
} // namespace editor

namespace game {
	class renderer;
} // namespace game

namespace debug {

class XRAY_RENDER_API renderer : private core::noncopyable {
private:
	friend class editor::renderer;
	friend class game::renderer;
			renderer			( one_way_render_channel& channel, memory::base_allocator& allocator, engine::world& engine_world );

public:

	void 	draw_line			( scene_ptr const& scene , float3 const& start_point, float3 const& end_point, math::color const& color, bool use_depth = true );
	void 	draw_cube			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_frustum		(
				scene_ptr const& scene,
				float fov_in_radians,
				float near_plane_distance,
				float far_plane_distance,
				float aspect_ratio,
				float3 const& position,
				float3 const& direction,
				float3 up_direction,
				math::color const& color,
				bool use_depth = true
			);
	void	draw_rectangle		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_aabb			( scene_ptr const& scene, float3 const& center, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_ellipse		( scene_ptr const& scene, float4x4 const& matrix, math::color const& color, bool use_depth = true );
	void 	draw_ellipsoid		( scene_ptr const& scene, float4x4 const& matrix, math::color const& color, bool use_depth = true );
	void 	draw_ellipsoid		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_circle			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_cylinder		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_sphere			( scene_ptr const& scene, float3 const& center, const float &radius, math::color const& color, bool use_depth = true );
	void 	draw_cone			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_arrow			( scene_ptr const& scene, float3 const& start_point, float3 const& end_point, math::color const& arrow_color, math::color const& cone_color, bool use_depth = true );
	void 	draw_arrow			( scene_ptr const& scene, float3 const& start_point, float3 const& end_point, math::color const& color, bool use_depth = true );
	void 	draw_lines			( scene_ptr const& scene, vectora< vertex_colored > const& vertices, vectora< u16 > const& indices, bool use_depth = true );
	void 	draw_lines			(	scene_ptr const& scene,
									vectora< vertex_colored >::const_iterator start_point,
									vectora< vertex_colored >::const_iterator end_point,
									vectora< u16 > const& indices,
									bool use_depth = true
								);
	void 	draw_lines			( scene_ptr const& scene, float4x4 const& matrix, float const* vertices, u32 vertex_count, u16 const* pairs,  u32 pair_count, math::color const& color, bool use_depth = true );
	void 	draw_origin			( scene_ptr const& scene, float4x4 const& matrix, float const half_size, bool use_depth = true );
	void	draw_cross			( scene_ptr const& scene, float3 const& p, float const half_size, math::color const& c, bool use_depth = true );

	void 	draw_triangle		( scene_ptr const& scene, float3 const& point_0, float3 const& point_1, float3 const& point_2, math::color const& color, bool use_depth = true );
	void 	draw_triangle		( scene_ptr const& scene, vertex_colored const& vertex_0, vertex_colored const& vertex_1, vertex_colored const& vertex_2, bool use_depth = true );
	void 	draw_triangle		( scene_ptr const& scene, vertex_colored const ( &vertices )[ 3 ], bool use_depth = true );
	void 	draw_triangles		( scene_ptr const& scene, debug_vertices_type const &vertices, bool use_depth = true );
	void 	draw_triangles		( scene_ptr const& scene, debug_vertices_type const &vertices, debug_indices_type const& indices, bool use_depth = true );

	void	draw_frustum_solid	( 
				scene_ptr const& scene,
				float const fov_in_radians,
				float const near_plane_distance,
				float const far_plane_distance,
				float const aspect_ratio,
				float3 const& position,
				float3 const& direction,
				float3 up_vector,
				math::color const& color,
				bool use_depth
			);
	void 	draw_rectangle_solid( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_cube_solid		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_cone_solid		( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void 	draw_cylinder_solid	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void	draw_sphere_solid	( scene_ptr const& scene, float3 const& center, const float& radius, math::color const& color, bool use_depth = true );
	void 	draw_ellipsoid_solid( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void	draw_line_ellipsoid	( scene_ptr const& scene, float4x4 const& matrix, math::color const& color, bool use_depth = true );
	void	draw_line_hemisphere( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, bool bdraw_base, math::color const& color, bool use_depth = true );
	void	draw_line_capsule	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );
	void	draw_solid_capsule	( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, math::color const& color, bool use_depth = true );

	memory::base_allocator&	get_allocator() {return m_allocator;}

private:
	void 	draw_lines			( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, float const* vertices,  u32 vertex_count, u16 const* pairs, u32 pair_count, math::color const& color, bool use_depth = true );

	void 	draw_primitive_solid( scene_ptr const& scene, float4x4 const& matrix, float3 const& size, float const* vertices,  u32 vertex_count, u16 const* faces, u32 index_count, math::color const& color, bool use_depth = true );

	void	create_primitive	(
				scene_ptr const& scene,
				vectora< float3 >& avertices, 
				vectora< u16 >& aindices, 
				math::float4x4 transform, 
				float const* vertices,  
				u32 vertex_count, 
				u16 const* faces, 
				u32 index_count
			);

public:
	math::frustum			frustum;

private:
	engine::world&			m_render_engine_world;
	one_way_render_channel&	m_channel;
	memory::base_allocator&	m_allocator;
}; // class world

} // namespace debug
} // namespace render
} // namespace xray

#endif // #ifndef XRAY_RENDER_FACADE_DEBUG_RENDERER_H_INCLUDED
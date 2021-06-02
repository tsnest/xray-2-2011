////////////////////////////////////////////////////////////////////////////
//	Created		: 14.06.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef BENCHMARK_RENDERER_H_INCLUDED
#define BENCHMARK_RENDERER_H_INCLUDED


namespace xray {
namespace graphics_benchmark {

class gpu_timer;

class benchmark_renderer: public boost::noncopyable
{
public:
	enum enum_vertex_size_id
	{
		enum_vertex_size_24bytes,
		enum_vertex_size_32bytes,
		enum_vertex_size_40bytes,
		enum_vertex_size_48bytes,
		enum_vertex_size_64bytes,
		enum_vertex_size_96bytes,
		enum_vertex_size_128bytes,
		enum_vertex_size_COUNT,
	};
	struct colored_vertex
	{
		math::float3	position;
		math::float3	normal;
		u32				color;
		math::float2	uv;
		u32				colors[7];
	};
	struct low_colored_vertex
	{
		math::float3	position;
		u32				color;
		math::float2	uv;
	};
	template<u32 num_uv_channels=1> struct colored_vertex_sized
	{
		math::float3	position;
		u32				color;
		math::float2	uv[num_uv_channels];
	}; 
	struct mesh
	{
		u32						ioffset;
		u32						voffset;
		u32						index_count;
	};
	typedef xray::render::vector< colored_vertex >	colored_vertices_type;
	typedef xray::render::vector< low_colored_vertex >	low_colored_vertices_type;
	
	typedef xray::render::vector< u16 >				colored_indices_type;
	


	benchmark_renderer			( );
	~benchmark_renderer			( );
	void	initialize			( );
	
	
	void render_fullscreen_quad	( gpu_timer& gtimer);
	void render_low_fullscreen_quad	( gpu_timer& gtimer);
	
	void prepare_quad();
	u32  cook_mesh				( colored_vertices_type const& vertices, colored_indices_type  const& indices);
	u32  cook_mesh				( void* vertices, u32 const vertex_size, u32 const vertex_count, colored_indices_type  const& indices);
	u32  cook_geom				( void* vertices, u32 const vertex_size, u32 const vertex_count, colored_indices_type  const& indices);
	void render_cooked_mesh		( u32 mesh_index, xray::math::float4x4 const& wvp_matrix = xray::math::float4x4().identity());
	void render_3d_sphere		( gpu_timer& gtimer, xray::math::float4x4 const& wvp_matrix);
	
private:
	u32								m_quad_mesh_idx;
	u32								m_low_quad_mesh_idx;
	u32								m_3d_sphere_mesh_idx;

	std::vector<mesh>				m_meshes;

	xray::render::ref_geometry		m_geom;
	xray::render::ref_geometry		m_low_geom;

	xray::render::ref_geometry		m_geom_sized[ enum_vertex_size_COUNT ];

	xray::render::constant_host*	m_WVP;
}; // class benchmark_renderer

} // namespace graphics_benchmark
} // namespace xray

#endif // BENCHMARK_RENDERER_H_INCLUDED
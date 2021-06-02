////////////////////////////////////////////////////////////////////////////
//	Created		: 09.12.2009
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2009
////////////////////////////////////////////////////////////////////////////

#ifndef GEOMETRY_UTILS_H_INCLUDED
#define GEOMETRY_UTILS_H_INCLUDED

namespace xray {
namespace geometry_utils{

typedef xray::vectora< math::float3 >	geom_vertices_type;
typedef xray::vectora< u16 >			geom_indices_type;

XRAY_CORE_API bool		create_torus	( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, 
												float outer_raduius,  float inner_raduius, u16 outer_segments, u16 inner_segments );

XRAY_CORE_API bool		create_cone		( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, math::float3 size );

XRAY_CORE_API bool		create_cylinder	( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, math::float3 size );

XRAY_CORE_API bool		create_cube		( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, math::float3 size );

XRAY_CORE_API bool		create_ellipsoid( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, math::float3 size );

XRAY_CORE_API bool		create_octahedron( geom_vertices_type& vertices, geom_indices_type& indices, math::float4x4 transform, math::float3 size );

XRAY_CORE_API bool		create_ring		( geom_vertices_type& vertices, geom_indices_type& indices, float inner_radius, float width, u16 segments_count );

#pragma warning(push)
#pragma warning(disable:4251)
struct XRAY_CORE_API geometry_collector
{
						geometry_collector	( memory::base_allocator& a);

	void					add_vertex			( math::float3 const& p );
	void					add_triangle		( math::float3 const& p0, math::float3 const& p1, math::float3 const& p2 );
	bool					write_obj_file		( pcstr fn, float const scale );

protected:	
	typedef vectora<math::float3>	vertices_type;
	typedef vectora<u32>			indices_type;

	vertices_type			m_vertices;
	indices_type			m_indices;

	int						find_vertex				( math::float3 const& pos ) const;
	struct lookup_item
	{
		float		square_magnitude;
		u32			vertex_id;

		inline bool operator < (lookup_item const& other)	const	{ return square_magnitude<other.square_magnitude; }
		inline bool operator < (float const& sqm)			const	{ return square_magnitude<sqm; }
	};

	typedef vectora<lookup_item>	lookup_table;
	lookup_table					m_lookup;
};
#pragma warning(pop)

} // namespace geometry_utils
} // namespace xray

#endif // GEOMETRY_UTILS_H_INCLUDED

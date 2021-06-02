////////////////////////////////////////////////////////////////////////////
//	Created		: 11.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DELAUNAY_TRIANGULATOR_H_INCLUDED
#define DELAUNAY_TRIANGULATOR_H_INCLUDED

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

class XRAY_AI_NAVIGATION_API delaunay_triangulator : private core::noncopyable {
public:
	typedef xray::buffer_vector< float3 >			vertices_type;
	typedef xray::buffer_vector< u32 >				indices_type;
	typedef debug::vector< u32 >					input_indices_type;
	typedef debug::vector< u32 >					error_vertices_indices_type;

public:
						template < typename VerticesContainer >
						delaunay_triangulator			(
							VerticesContainer& vertices,
							indices_type& indices,
							float3 const& normal,
							error_vertices_indices_type* error_vertices_indices = 0,
							u32 max_delaunay_operation_id = u32(-1),
							bool keep_bounding_triangle = false
						);
						template < typename VerticesContainer, typename InputIndicesContainer >
						delaunay_triangulator			(
							VerticesContainer& vertices,
							InputIndicesContainer& input_indices,
							indices_type& output_indices,
							float3 const& normal,
							error_vertices_indices_type* error_vertices_indices = 0
						);

	static inline u32	get_additional_vertex_count		( ) { return 3; }
	static inline u32	get_maximum_triangle_count_in_triangulation ( u32 const vertex_count ) 
	{
		return 9*vertex_count + 1;
	}
private:
	typedef xray::buffer_vector< bool >	markers_type;

private:
			void		split_triangle					(
							u32 const (&coordinate_indices)[2],
							u32 vertex_index,
							u32 triangle_id,
							u32 edge_id
						);
			bool		split_triangles_are_correct	(
							u32 const (&coordinate_indices)[2],
							u32 vertex_index,
							u32 triangle_id,
							u32 edge_id
						) const;
			bool		split_triangles_are_correct	(
							u32 const (&coordinate_indices)[2],
							u32 vertex_index,
							u32 triangle_id,
							u32 edge_id,
							u32 neighbour_triangle_id,
							u32 neighbour_edge_id
						) const;
			u32			find_triangle (
							u32 const (&coordinate_indices)[2],
							float3 const& vertex,
							u32 start_index = 0,
							bool assert_on_failure = true
						);
			u32			find_neighbour_triangle_id	(
							u32 triangle_id,
							u32 vertex_id0,
							u32 vertex_id1,
							u32& vertex_id2
						);
			bool		is_edge_legal (
							u32 const (&coordinate_indices)[2],
							u32 triangle_id,
							u32 edge_id,
							u32 fourth_vertex_index
						);
			void		legalize_edge (
							u32 const (&coordinate_indices)[2],
							u32 triangle_id,
							u32 edge_id
						);
			void		add_vertex_inside_triangle_to_triangulation	(
							u32 const (&coordinate_indices)[2],
							u32 triangle_id,
							u32 vertex_id
						);
			bool		add_vertex_on_edge_to_triangulation	(
							u32 const (&coordinate_indices)[2],
							u32 vertex_index,
							u32 triangle_id,
							u32 edge_id
						);
			bool		add_vertex_to_triangulation ( u32 const (&coordinate_indices)[2],	u32 const vertex_id );
			
	template < typename ContainerType >
	inline	void		add_bounding_triangle(
							float3 const& normal,
							ContainerType& vertices,
							u32 (&coordinate_indices)[2]
						);
	template < typename VerticesContainerType >
	inline	void		add_bounding_triangle(
							VerticesContainerType& vertices,
							float3 const& normal,
							u32 (&coordinate_indices)[2],
							float3 const& min_position,
							float3 const& max_position
						);
	template < typename VerticesContainerType, typename IndicesContainerType >
	inline	void		add_bounding_triangle(
							float3 const& normal,
							VerticesContainerType& vertices,
							IndicesContainerType const& indices,
							u32 (&coordinate_indices)[2]
						);

	template < typename ContainerType >
			void		remove_bounding_triangle ( ContainerType& vertices );

	inline	bool		is_vertex_of_the_bounding_triangle	( u32 vertex_id ) const;
	static inline	bool		is_vertex_of_the_bounding_triangle	( u32 vertices_size, u32 vertex_id );

	inline	bool		is_correct_ordering			(
							u32 const (&coordinate_indices)[2],
							float3 v0,
							float3 v1,
							float3 v2
						) const;

private:
	float3 const*		m_vertices;
	indices_type&		m_indices;
	u32					m_vertices_size;
	float				m_ordering_sign;


private:
	class bounding_triangle_filter {
	public:
		inline bounding_triangle_filter	( u32 vertices_size, delaunay_triangulator::indices_type const& indices ) :
			m_vertices_size	( vertices_size ),
			m_indices		( &indices )
		{
		}

		inline bool operator ( )		( u32 const& index ) const
		{
			u32 const triangle_id	= (&index - m_indices->begin( )) / 3;
			return			
				is_vertex_of_the_bounding_triangle( m_vertices_size, (*m_indices)[ 3*triangle_id + 0 ] ) ||
				is_vertex_of_the_bounding_triangle( m_vertices_size, (*m_indices)[ 3*triangle_id + 1 ] ) ||
				is_vertex_of_the_bounding_triangle( m_vertices_size, (*m_indices)[ 3*triangle_id + 2 ] );
		}

	private:
		u32											m_vertices_size;
		delaunay_triangulator::indices_type const*	m_indices;
	}; // struct bounding_triangle_filter

}; // class delaunay_triangulator
void XRAY_AI_NAVIGATION_API fill_coordinate_indices	( float3 const& normal, u32 (&coordinate_indices)[2] );
u32 XRAY_AI_NAVIGATION_API segment_intersects_segment	(
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result0,
		float3& result1,
		bool& are_on_the_same_line
	);

} // namespace ai
} // namespace navigation
} // namespace xray

bool XRAY_AI_NAVIGATION_API is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float epsilon = xray::math::epsilon_6
	);

bool XRAY_AI_NAVIGATION_API is_similar ( 
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& v0,
		xray::math::float3 const& v1,
		float epsilon = xray::math::epsilon_6
	);

float XRAY_AI_NAVIGATION_API cross_product				( u32 const (&coordinate_indices)[2], xray::math::float3 const& vertex0, xray::math::float3 const& vertex1 );
float XRAY_AI_NAVIGATION_API dot_product				( u32 const (&coordinate_indices)[2], xray::math::float3 const& vertex0, xray::math::float3 const& vertex1 );
bool  XRAY_AI_NAVIGATION_API is_point_inside_triangle	( u32 const (&coordinate_indices)[2], xray::math::float3 p, xray::math::float3 a, xray::math::float3 b, xray::math::float3 c );

#include "delaunay_triangulator_inline.h"

#endif // #ifndef DELAUNAY_TRIANGULATOR_H_INCLUDED

////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "constrained_delaunay_triangulator.h"
#include <xray/strings_stream.h>

using xray::ai::navigation::delaunay_triangulator;
using xray::ai::navigation::constrained_delaunay_triangulator;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

u32 segment_intersects_triangle	(
		float3 const& normal,
		float3 const& segment0,
		float3 const& segment1,
		float3 const& vertex0,
		float3 const& vertex1,
		float3 const& vertex2,
		float3 (&intersections)[2]
	);

typedef xray::buffer_vector< bool > markers_type;

struct is_marked_in_buffer_predicate3 {
	inline			is_marked_in_buffer_predicate3	( delaunay_triangulator::indices_type const& indices, markers_type const& markers ) :
		m_indices	( &indices ),
		m_markers	( &markers )
	{
	}

	inline	bool	operator()			( u32 const& vertex_id ) const
	{
		size_t const vertex_id_index	= &vertex_id - m_indices->begin( );
		return							(*m_markers)[ vertex_id_index / 3 ];
	}

private:
	delaunay_triangulator::indices_type const* m_indices;
	markers_type const*	m_markers;
}; // struct is_marked_in_buffer_predicate


static inline bool is_right_side ( u32 const (&coordinate_indices)[2], float3 p, float3 b, float3 c, float3 n)
{
	u32 third_index = u32(-1);
	switch ( coordinate_indices[0] + coordinate_indices[1] ) {
	case 1: third_index = 2; break;
	case 2: third_index = 1; break;
	case 3: third_index = 0; break;
	default: NODEFAULT();
	}
	p[third_index] = 0;
	b[third_index] = 0;
	c[third_index] = 0;
	return ( (normalize(b - c) ^ normalize(p - c)) | n ) >= 0.f;
}

bool constrained_delaunay_triangulator::is_triangle_vertex(
		float3 const& testee,
		float3 const& v0,
		float3 const& v1,
		float3 const& v2
	)
{
	if ( testee.is_similar( v0 ) ) {
		R_ASSERT( testee.is_similar( v1 ) );
		R_ASSERT( testee.is_similar( v2 ) );
		return true;
	}

	if ( testee.is_similar( v1 ) ) {
		R_ASSERT( testee.is_similar( v2 ) );
		return true;
	}

	if ( testee.is_similar( v2 ) )
		return true;

	return false;
}

bool is_on_segment	(
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex	// v1
	);

u32 segment_intersects_segment	(
		u32 const (&coordinate_indices)[2],
		float3 const& v0,
		float3 const& v1,
		float3 const& u0,
		float3 const& u1,
		float3& result0,
		float3& result1,
		bool& are_on_the_same_line
	);

struct is_segment_vertex_predicate {
	is_segment_vertex_predicate( u32 const edge_vertex_index0, u32 edge_vertex_index1 ) :
		m_vertex_index0( edge_vertex_index0 ),
		m_vertex_index1( edge_vertex_index1 )
	{
	}

	bool operator() (u32 const vertex_index) {
		return (m_vertex_index0 == vertex_index) || (m_vertex_index1 == vertex_index);
	}

private:
	u32 m_vertex_index0;
	u32 m_vertex_index1;
};

struct less_by_projection_predicate {
	less_by_projection_predicate( 
			float3 const* vertices,  
			float3 const& segment_vertex0,
			float3 const& segment_vertex1
		) :
			m_vertices( vertices ),
			m_segment_vertex0 ( segment_vertex0 ),
			m_segment_vertex1 ( segment_vertex1 ),
			m_direction ( normalize(segment_vertex1 - segment_vertex0) )
	{
	}

	bool operator() ( u32 const left, u32 const right )
	{
		float projection0 = ( m_vertices[ left ] - m_segment_vertex0 ) | m_direction;
		float projection1 = ( m_vertices[ right ] - m_segment_vertex0 ) | m_direction;

		if ( projection0 < projection1 )
			return true;
		return false;
	}

private:
	float3 const*	m_vertices;
	float3			m_segment_vertex0;
	float3			m_segment_vertex1;
	float3			m_direction;
};

void constrained_delaunay_triangulator::add_pseudo_polygon_index(
			u32 const (&coordinate_indices)[2],
			u32 const vertex_index,
			polygon_indices_type& right_polygon_indices, 
			polygon_indices_type& left_polygon_indices,
			float3 const& v0,
			float3 const& v1
	)
{
	float3 vertex = m_vertices[ vertex_index ];
	if ( is_right_side( coordinate_indices, vertex, v0, v1, m_normal ) )
		right_polygon_indices.push_back( vertex_index );
	else
		left_polygon_indices.push_back( vertex_index );
}

bool constrained_delaunay_triangulator::remove_triangles ( 
			u32 const (&coordinate_indices)[2],
			polygon_indices_type& right_polygon_indices, 
			polygon_indices_type& left_polygon_indices, 
			u32 const segment_vertex_index0, 
			u32 const segment_vertex_index1
		)
{
	u32 third_index;

	switch ( coordinate_indices[0] + coordinate_indices[1] ) {
		case 1 : third_index = 2; break;
		case 2 : third_index = 1; break;
		case 3 : third_index = 0; break;
		default : NODEFAULT( third_index = u32(-1) );
	}

	u32 const	triangles_count	= m_indices.size() / 3;

	markers_type markers (
		ALLOCA( sizeof(bool) * triangles_count ),
		triangles_count,
		triangles_count,
		false
	);
	
	float3 const& segment_vertex0 = m_vertices[ segment_vertex_index0 ];
	float3 const& segment_vertex1 = m_vertices[ segment_vertex_index1 ];
	float3 projected_vertex0 = segment_vertex0;
	float3 projected_vertex1 = segment_vertex1;
	projected_vertex0[third_index] = 0;
	projected_vertex1[third_index] = 0;

	for ( u32 i = 0; i < triangles_count; ++i ) {
		
		u32 equal_vertex_count = 0;
		for ( u32 j = 0; j < 3; ++j ) {
			if ( segment_vertex_index0 == m_indices[3*i+j] )
				++equal_vertex_count;
			if ( segment_vertex_index1 == m_indices[3*i+j] )
				++equal_vertex_count;
		}

		if ( equal_vertex_count >= 2 ) {
			return false;
		}
	}

	triangles_info_type intersected_triangles;
	for ( u32 i = 0; i < triangles_count; ++i ) {
		/*
		u32 const intersections_count = segment_intersects_triangle( m_normal, segment_vertex0, segment_vertex1, m_vertices[m_indices[3*i+0]], m_vertices[m_indices[3*i+1]], m_vertices[m_indices[3*i+2]], intersections );
		*/
		
		intersections_type intersections;
		u32 const* indices = m_indices.begin() + i * 3;
		for ( u32 j = 0; j < 3; ++j ) {
			u32 const first_index	=	indices[ (j + 0) % 3 ];
			u32 const second_index	= indices[ (j + 1) % 3 ];

			float3 u0 = m_vertices[ first_index ];
			float3 u1 = m_vertices[ second_index ];
			u0[third_index] = 0;
			u1[third_index] = 0;

			bool are_on_same_line;
			float3 intersection[2];
			u32 const intersections_count = segment_intersects_segment(
				coordinate_indices,
				projected_vertex0,
				projected_vertex1,
				u0,
				u1,
				intersection[0],
				intersection[1],
				are_on_same_line
			);
			
			R_ASSERT_CMP( intersections_count, !=, 2 );
			if ( intersections_count ) {
				intersections.push_back( intersection_info( j, intersection[0] ) );
			}
		}
		
		for ( u32 k = 0; k < intersections.size(); ++k ) {
			for ( u32 l = k+1; l < intersections.size(); ++l ) {
				if ( is_similar( coordinate_indices, intersections[k].vertex, intersections[l].vertex ) ) {
					intersections.erase( intersections.begin() + l );
					--l;
				}
			}
		}

		u32 const intersections_count = intersections.size();
		R_ASSERT		( intersections_count <= 2 );
		if ( intersections_count > 1 ) {
			triangle_info info;
			info.triangle_id = i;

			float squared_length0 = squared_length( intersections[0].vertex - projected_vertex0 );
			float squared_length1 = squared_length( intersections[1].vertex - projected_vertex0 );

			if  ( squared_length1 < squared_length0 ) {
				info.intersection = intersections[1];
			} else {
				info.intersection = intersections[0];
			}
			intersected_triangles.push_back( info );
			markers[i] = true;
		}
	}
	
	std::sort (
		intersected_triangles.begin(),
		intersected_triangles.end(),
		less_by_intersection_distance( projected_vertex0 )
	);

	u32 const intersected_triangles_count = intersected_triangles.size();
	u32 u0;
	u32 u1;
	if( intersected_triangles_count > 1 ) {
		u32 edge_id	= intersected_triangles[1].intersection.edge_id;
		u32 const* indices = m_indices.begin() + 3*intersected_triangles[1].triangle_id;

		for ( u32 i = 0; i < 2; ++i ) {
			add_pseudo_polygon_index(
				coordinate_indices,
				indices[(edge_id+i)%3],
				right_polygon_indices,
				left_polygon_indices,
				segment_vertex0,
				segment_vertex1
			);
		}

		u0 = indices[(edge_id+0)%3];
		u1 = indices[(edge_id+1)%3];
	}

	for ( u32 i = 2; i < intersected_triangles_count; ++i ) {
		u32 const edge_id	= intersected_triangles[i].intersection.edge_id;
		u32 const* indices = m_indices.begin() + 3*intersected_triangles[i].triangle_id;
		if ( u1 == indices[(edge_id+1)%3] ) {
			add_pseudo_polygon_index(
				coordinate_indices,
				indices[(edge_id+0)%3],
				right_polygon_indices,
				left_polygon_indices,
				segment_vertex0,
				segment_vertex1
			);
		} else {
			R_ASSERT_CMP( u0, ==, indices[(edge_id+0)%3] );
			add_pseudo_polygon_index(
				coordinate_indices,
				indices[(edge_id+1)%3],
				right_polygon_indices,
				left_polygon_indices,
				segment_vertex0,
				segment_vertex1
			);
		}
		u0 = indices[(edge_id+0)%3];
		u1 = indices[(edge_id+1)%3];
	}

	m_indices.erase (
		std::remove_if (
			m_indices.begin(),
			m_indices.end(),
			is_marked_in_buffer_predicate3( m_indices, markers )
		),
		m_indices.end()
	);

	std::reverse(left_polygon_indices.begin(), left_polygon_indices.end());

	return true;
}

bool is_outside_circumcircle						(
		u32 const (&coordinate_indices)[2],
		float3 const& triangle_vertex0,
		float3 const& triangle_vertex1,
		float3 const& triangle_vertex2,
		float3 const& test_vertex
	);

inline bool constrained_delaunay_triangulator::is_correct_order(
		float3 const& v0,
		float3 const& v1,
		float3 const& v2
	)
{
	return ( ( normalize( v1 - v0 ) ^ normalize( v2 - v0 ) ) | m_normal) > 0;
}

void constrained_delaunay_triangulator::triangulate_pseudo_polygon ( 
		u32 const (&coordinate_indices)[2],
		polygon_indices_type::iterator const start, 
		polygon_indices_type::iterator const end, 
		u32 const edge_vertex_index0, 
		u32 const edge_vertex_index1 
	)
{
	u32 const vertices_count = u32(end - start);
	if ( vertices_count > 1 ) {
		polygon_indices_type::iterator third_vertex = start;
		polygon_indices_type::iterator i = start + 1;
		for ( ; i != end; ++i ) {
			if ( (*i) == edge_vertex_index0 || (*i) == edge_vertex_index1 ) {
				R_ASSERT( ((*i) == edge_vertex_index0) || ((*i) == edge_vertex_index1) );
				continue;
			}

			if	( !is_outside_circumcircle( 
						coordinate_indices,
						m_vertices[ edge_vertex_index0 ],
						m_vertices[ *third_vertex ],
						m_vertices[ edge_vertex_index1 ],
						m_vertices[ *i ]
					)
				) 
			{
				third_vertex = i;
			}
		}

		m_indices.push_back( edge_vertex_index0 );
		m_indices.push_back( *third_vertex );
		m_indices.push_back( edge_vertex_index1 );

		R_ASSERT( 
			is_correct_order(
				m_vertices[ m_indices[ m_indices.size() - 3 ] ],
				m_vertices[ m_indices[ m_indices.size() - 2 ] ],
				m_vertices[ m_indices[ m_indices.size() - 1 ] ]
			)
		);

		triangulate_pseudo_polygon( coordinate_indices, start, third_vertex, edge_vertex_index0, *third_vertex );
		triangulate_pseudo_polygon( coordinate_indices, third_vertex+1, end, *third_vertex, edge_vertex_index1 );
		return;
	}

	if ( vertices_count > 0 ) {
		m_indices.push_back( edge_vertex_index0 );
		m_indices.push_back( *start );
		m_indices.push_back( edge_vertex_index1 );

		R_ASSERT( 
			is_correct_order(
				m_vertices[ m_indices[ m_indices.size() - 3 ] ],
				m_vertices[ m_indices[ m_indices.size() - 2 ] ],
				m_vertices[ m_indices[ m_indices.size() - 1 ] ]
			)
		);
	}
}

void copy_config_to_clipboard ( xray::configs::lua_config_ptr& /*config*/ ) 
{

}

////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_ADJACENCY_BUILDER_H_INCLUDED
#define GRAPH_GENERATOR_ADJACENCY_BUILDER_H_INCLUDED

#include <xray/ai_navigation/api.h>

namespace xray {
namespace ai {
namespace navigation {

class XRAY_AI_NAVIGATION_API graph_generator_adjacency_builder {
	typedef buffer_vector< u32 > triangle_indices_type;

public:
	
	template < typename ContainerType0, typename ContainerType1>
	graph_generator_adjacency_builder( 
		ContainerType0 const& indices, 
		ContainerType1& neighbour_graph 
	);
	
	template < typename ContainerType0, typename ContainerType1, typename ContainerType2 >
	graph_generator_adjacency_builder( 
		ContainerType0 const& indices, 
		ContainerType1& adjacency_graph,
		ContainerType2 const& normals,
		float const normal_epsilon
	);

	template < typename ContainerType0, typename ContainerType1, typename TriangleIdsType >
	graph_generator_adjacency_builder( 
		ContainerType0 const& indices,
		TriangleIdsType const& triangles,
		ContainerType1& adjacency_graph
	);

	template < typename ContainerType0, typename ContainerType1, typename ContainerType2, typename TriangleIdsType >
	graph_generator_adjacency_builder( 
		ContainerType0 const& indices,
		TriangleIdsType const& triangles,
		ContainerType1& adjacency_graph,
		ContainerType2 const& normals,
		float const normal_epsilon
	);

private:
	void fill_and_sort_triangle_lists( triangle_indices_type& triangle_indices );
	void sort_triangle_lists( triangle_indices_type& triangle_indices );
	bool neighbour_check( 
		u32 const* indices0,
		u32 const* indices1,
		u32& edge_id0,
		u32& edge_id1
	);

private:
	u32 const*			m_indices;
	u32 				m_triangles_count;

private:
	struct less_triangle_predicate {
		less_triangle_predicate ( u32 const* indices, u32 const vertex_index0, u32 const vertex_index1, u32 const vertex_index2 ) :
			m_indices			( indices ),
			m_vertex_index0		( vertex_index0 ),
			m_vertex_index1		( vertex_index1 ),
			m_vertex_index2		( vertex_index2 )
		{
			R_ASSERT		( vertex_index0 < 3 );
			R_ASSERT		( vertex_index1 < 3 );
			R_ASSERT		( vertex_index2 < 3 );
		}

		inline	bool		operator () ( u32 const& left, u32 const& right )
		{
			u32	const*		indices0	= m_indices + 3 * left;
			u32 const*		indices1	= m_indices + 3 * right;

			if ( indices0[ m_vertex_index0 ] < indices1[ m_vertex_index0 ] )
				return true;

			if ( indices0[ m_vertex_index0 ] > indices1[ m_vertex_index0 ] )
				return false;

			if ( indices0[ m_vertex_index1 ] < indices1[ m_vertex_index1 ] )
				return true;

			if ( indices0[ m_vertex_index1 ] > indices1[ m_vertex_index1 ] )
				return false;

			return indices0[ m_vertex_index2 ] < indices1[ m_vertex_index2 ];
		}

		inline bool operator() (u32 const left, u32 const* indices1 ) 
		{
			u32	const*		indices0	= m_indices + 3 * left;

			if ( indices0[ m_vertex_index0 ] < indices1[ m_vertex_index0 ] )
				return true;

			if ( indices0[ m_vertex_index0 ] > indices1[ m_vertex_index0 ] )
				return false;

			if ( indices0[ m_vertex_index1 ] < indices1[ m_vertex_index1 ] )
				return true;

			if ( indices0[ m_vertex_index1 ] > indices1[ m_vertex_index1 ] )
				return false;

			return indices0[ m_vertex_index2 ] < indices1[ m_vertex_index2 ];
		}

	private:
		u32 const*					m_indices;
		u32							m_vertex_index0;
		u32							m_vertex_index1;
		u32							m_vertex_index2;
	}; // struct less_triangle_predicate

}; // class graph_generator_adjacency_builder

template < typename ContainerType0, typename ContainerType1, typename TriangleIdsType>
graph_generator_adjacency_builder::graph_generator_adjacency_builder( 
		ContainerType0 const& indices,
		TriangleIdsType const& triangles,
		ContainerType1& adjacency_graph 
	) :
		m_indices			( indices.begin() )
{
	
	R_ASSERT_CMP					( indices.size() % 3, ==, 0);
	m_triangles_count				= triangles.size();

	triangle_indices_type sorted_triangle_indices(
		ALLOCA( 3 * m_triangles_count * sizeof( triangle_indices_type::value_type ) ),
		m_triangles_count * 3,
		m_triangles_count * 3
	);
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() );
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() + m_triangles_count );
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() + m_triangles_count * 2);
	sort_triangle_lists	( sorted_triangle_indices );
	
	for ( u32 i = 0; i < m_triangles_count; ++i )
		if ( adjacency_graph[i].neighbours[0] != u32(-2) ) {
			for ( u32 j = 0; j < 3; ++j )
				adjacency_graph[i].neighbours[j] = u32(-1);
		}

	triangle_indices_type::iterator	i = sorted_triangle_indices.begin();
	triangle_indices_type::iterator const e = sorted_triangle_indices.begin() + m_triangles_count;
	for ( ; i != e; ++i ) {
		u32 success_count = 0;
		for ( u32 j = 0; j < 3; ++j ) {
			if ( adjacency_graph[ (*i) ].neighbours[j] != u32(-1) ) {
				++success_count;
			}
		}
		
		u32 edge_id0;
		u32 edge_id1;

		for ( u32 k = 0; k < 3; ++k ) {
			if ( adjacency_graph[ (*i) ].neighbours[k] != u32(-1) )
				continue;

			for ( u32 l = 0; l < 3; ++l ) {
				if ( success_count == 3 )
					break;

				u32 comp_indices[] = { 0, 0, 0 };
				comp_indices[ l ] = m_indices[ (*i) * 3 + k ];

				triangle_indices_type::iterator const first = sorted_triangle_indices.begin() + m_triangles_count * l;
				triangle_indices_type::iterator const last = sorted_triangle_indices.begin() + m_triangles_count * (l + 1);
				triangle_indices_type::iterator t = std::lower_bound(
					first,
					last,
					comp_indices,
					less_triangle_predicate( m_indices, (l+0)%3, (l+1)%3, (l+2)%3 )
				);

				for (; t != last; ++t ) {
					if ( m_indices[ (*t) * 3 + l] != comp_indices[l] )
						break;

					if ( (*i) == (*t) )
						continue;

					if ( neighbour_check ( m_indices + (*i) * 3, m_indices + (*t) * 3, edge_id0, edge_id1 ) ) {
						if ( adjacency_graph[ (*i) ].neighbours[ edge_id0 ] != u32(-1) )
							continue;

						adjacency_graph[ (*i) ].neighbours[ edge_id0 ] = (*t);
						adjacency_graph[ (*t) ].neighbours[ edge_id1 ] = (*i);
						++success_count;
					}
				}

			}
		}
	}
}

template < typename ContainerType0, typename ContainerType1, typename ContainerType2, typename TriangleIdsType>
graph_generator_adjacency_builder::graph_generator_adjacency_builder( 
		ContainerType0 const& indices,
		TriangleIdsType const& triangles,
		ContainerType1& adjacency_graph,
		ContainerType2 const& normals,
		float const normal_epsilon
	) :
		m_indices			( indices.begin() )
{
	
	R_ASSERT_CMP					( indices.size() % 3, ==, 0);
	m_triangles_count				= triangles.size();

	triangle_indices_type sorted_triangle_indices(
		ALLOCA( 3 * m_triangles_count * sizeof( triangle_indices_type::value_type ) ),
		m_triangles_count * 3,
		m_triangles_count * 3
	);
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() );
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() + m_triangles_count );
	std::copy( triangles.begin(), triangles.end(), sorted_triangle_indices.begin() + m_triangles_count * 2);
	sort_triangle_lists	( sorted_triangle_indices );

	typename TriangleIdsType::const_iterator const triangle_end = triangles.end();
	for ( typename TriangleIdsType::const_iterator it = triangles.begin(); it != triangle_end; ++it )
	{
		if ( adjacency_graph[ *it ].neighbours[ 0 ] != u32(-2) )
		{
			for ( u32 j = 0; j < 3; ++j )
				adjacency_graph[ *it ].neighbours[j] = u32(-1);
		}
	}

	triangle_indices_type::iterator	i = sorted_triangle_indices.begin();
	triangle_indices_type::iterator const e = sorted_triangle_indices.begin() + m_triangles_count;
	for ( ; i != e; ++i ) {

		u32 success_count = 0;
		for ( u32 j = 0; j < 3; ++j ) {
			if ( adjacency_graph[ (*i) ].neighbours[j] != u32(-1) ) {
				++success_count;
			}
		}
		
		u32 edge_id0;
		u32 edge_id1;

		for ( u32 k = 0; k < 3; ++k ) {
			if ( adjacency_graph[ (*i) ].neighbours[k] != u32(-1) )
				continue;

			for ( u32 l = 0; l < 3; ++l ) {
				if ( success_count == 3 )
					break;

				u32 comp_indices[] = { 0, 0, 0 };
				comp_indices[ l ] = m_indices[ (*i) * 3 + k ];

				triangle_indices_type::iterator const first = sorted_triangle_indices.begin() + m_triangles_count * l;
				triangle_indices_type::iterator const last = sorted_triangle_indices.begin() + m_triangles_count * (l + 1);
				triangle_indices_type::iterator t = std::lower_bound(
					first,
					last,
					comp_indices,
					less_triangle_predicate( m_indices, (l+0)%3, (l+1)%3, (l+2)%3 )
				);

				for (; t != last; ++t ) {
					if ( m_indices[ (*t) * 3 + l] != comp_indices[l] )
						break;

					if ( (*i) == (*t) )
						continue;

					if ( (normals[*i].plane.normal | normals[*t].plane.normal) < normal_epsilon )
						continue;
					
					if ( neighbour_check ( m_indices + (*i) * 3, m_indices + (*t) * 3, edge_id0, edge_id1 ) ) {
						if ( adjacency_graph[ (*i) ].neighbours[ edge_id0 ] != u32(-1) )
							continue;

						adjacency_graph[ (*i) ].neighbours[ edge_id0 ] = (*t);
						adjacency_graph[ (*t) ].neighbours[ edge_id1 ] = (*i);
						++success_count;
					}
				}

			}
		}
	}
}


} // namespace navigation
} // namespace ai
} // namespace xray

#include "adjacency_builder_inline.h"

#endif // #ifndef GRAPH_GENERATOR_ADJACENCY_BUILDER_H_INCLUDED

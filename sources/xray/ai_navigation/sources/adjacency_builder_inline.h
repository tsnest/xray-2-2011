////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef ADJACENCY_BUILDER_INLINE_H_INCLUDED
#define ADJACENCY_BUILDER_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

template < typename ContainerType0, typename ContainerType1>
graph_generator_adjacency_builder::graph_generator_adjacency_builder( 
		ContainerType0 const& indices, 
		ContainerType1& adjacency_graph 
	) :
		m_indices			( indices.begin() )
{
	
	R_ASSERT_CMP					( indices.size() % 3, ==, 0);
	m_triangles_count				= indices.size() / 3;

	triangle_indices_type sorted_triangle_indices(
		ALLOCA( 3 * m_triangles_count * sizeof( triangle_indices_type::value_type ) ),
		m_triangles_count * 3,
		m_triangles_count * 3
	);
	fill_and_sort_triangle_lists	( sorted_triangle_indices );
	
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

template < typename ContainerType0, typename ContainerType1, typename ContainerType2>
graph_generator_adjacency_builder::graph_generator_adjacency_builder( 
		ContainerType0 const& indices, 
		ContainerType1& adjacency_graph,
		ContainerType2 const& normals,
		float const normal_epsilon
	) :
		m_indices			( indices.begin() )
{
	
	R_ASSERT_CMP					( indices.size() % 3, ==, 0);
	m_triangles_count				= indices.size() / 3;

	triangle_indices_type sorted_triangle_indices(
		ALLOCA( 3 * m_triangles_count * sizeof( triangle_indices_type::value_type ) ),
		m_triangles_count * 3,
		m_triangles_count * 3
	);
	fill_and_sort_triangle_lists	( sorted_triangle_indices );
	
	{
	for ( u32 i = 0; i < m_triangles_count; ++i )
		if ( adjacency_graph[i].neighbours[0] != u32(-2) ) {
			for ( u32 j = 0; j < 3; ++j )
				adjacency_graph[i].neighbours[j] = u32(-1);
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

#endif // #ifndef ADJACENCY_BUILDER_INLINE_H_INCLUDED
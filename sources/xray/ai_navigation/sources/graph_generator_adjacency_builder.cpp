////////////////////////////////////////////////////////////////////////////
//	Created		: 18.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_adjacency_builder.h"

using xray::ai::navigation::graph_generator_adjacency_builder;

void graph_generator_adjacency_builder::fill_and_sort_triangle_lists( triangle_indices_type& triangle_indices )
{
	triangle_indices_type::iterator i = triangle_indices.begin();
	triangle_indices_type::iterator const e = triangle_indices.end();
	for ( u32 j = 0; i != e; ++i, ++j )
		(*i) = (j % m_triangles_count);

	this->sort_triangle_lists( triangle_indices );
}

void graph_generator_adjacency_builder::sort_triangle_lists( triangle_indices_type& triangle_indices )
{
	std::sort	( 
		triangle_indices.begin(),
		triangle_indices.begin() + m_triangles_count,
		less_triangle_predicate( m_indices, 0, 1, 2)
	);

	std::sort	( 
		triangle_indices.begin() + m_triangles_count,
		triangle_indices.begin() + m_triangles_count * 2,
		less_triangle_predicate( m_indices, 1, 2, 0)
	);

	std::sort ( 
		triangle_indices.begin() + m_triangles_count * 2,
		triangle_indices.begin() + m_triangles_count * 3,
		less_triangle_predicate( m_indices, 2, 0, 1)
	);
}


bool graph_generator_adjacency_builder::neighbour_check(
		u32 const* indices0, 
		u32 const* indices1,
		u32& edge_id0,
		u32& edge_id1
	)
{
	for ( u32 i = 0; i < 3; ++i ) {
		for ( u32 j = 0; j < 3; ++j ) {
			if ( indices0[ i ] == indices1[ (j+1) % 3 ]
				&& indices0[ (i+1)%3 ] == indices1[ j ] )
				{
					edge_id0 = i;
					edge_id1 = j;
					return true;
				}
		}
	}
	return false;
}

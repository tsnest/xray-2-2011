////////////////////////////////////////////////////////////////////////////
//	Created		: 23.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#if XRAY_DEBUG_ALLOCATOR

#include "graph_generator.h"

using xray::ai::navigation::graph_generator;

void graph_generator::merge_fill_indexed_vertices	( indexed_vertices_type& indexed_vertices )
{
	indexed_vertices.clear					( );
	m_before_merge_edges.clear				( );
	m_before_merge_edge_indices.clear		( );
	m_merged_convexes.clear					( );

	u32 const subdivided_triangles_count	= m_subdivided_triangles.size();
	indexed_vertices.reserve				( subdivided_triangles_count * 3 );
	m_before_merge_edges.reserve			( subdivided_triangles_count * 3 );
	m_before_merge_edge_indices.reserve		( subdivided_triangles_count * 3 );
	m_merged_convexes.reserve				( subdivided_triangles_count );

	{
		subdivided_triangles_type::const_iterator i			= m_subdivided_triangles.begin( );
		subdivided_triangles_type::const_iterator const e	= m_subdivided_triangles.end( );
		for ( ; i != e; ++i ) {
			u32 const count					= m_before_merge_edges.size();
			for ( u32 j = 0; j < 3; ++j ) {

				indexed_vertex const vertex	= { (*i).vertices[j], { count + j, count + (j+2)%3 }  };
				indexed_vertices.push_back	( vertex );

				edge const edge				= { { count + j, count + (j+1)%3 }, { m_merged_convexes.size(), u32(-1) }, count + j, (((*i).obstructed_edges & (1 << j)) == (1 << j)) };
				m_before_merge_edges.push_back	( edge );

				m_before_merge_edge_indices.push_back ( count + j );
			}

			m_merged_convexes.push_back		( std::make_pair( count, 3 ) );
		}
	}
}

void graph_generator::merge_remove_duplicated_vertices	( indexed_vertices_type& indexed_vertices )
{
	std::sort								( indexed_vertices.begin(), indexed_vertices.end() );

	indexed_vertices_type::const_iterator const b	= indexed_vertices.begin( );
	indexed_vertices_type::const_iterator e	= indexed_vertices.end( );
	indexed_vertices_type::iterator i		= indexed_vertices.begin( );

	R_ASSERT								( !indexed_vertices.empty() );
	indexed_vertices_type::const_iterator j	= b + 1;
	for ( ; j != e; ++j ) {
		if ( *i < *j )
			*++i	= *j;

		if ( i != j ) {
			for ( u32 k = 0; k < 2; ++k )
				m_before_merge_edges[ (*j).edge_indices[k] ].vertex_indices[ k ]	= u32(i - b);
		}
	}

	indexed_vertices.erase					(
		i + 1,
		indexed_vertices.end()
	);

	m_before_merge_vertices.resize			( indexed_vertices.size() );
	e										= indexed_vertices.end( );
	i										= indexed_vertices.begin( );
	for ( vertices_type::iterator j = m_before_merge_vertices.begin(); i != e; ++i, ++j )
		*j									= (*i).vertex;
}

void graph_generator::merge_remove_duplicated_edges	( )
{
	std::sort								( m_before_merge_edges.begin(), m_before_merge_edges.end() );

	edges_type::const_iterator const b		= m_before_merge_edges.begin( );
	edges_type::const_iterator e			= m_before_merge_edges.end( );
	edges_type::iterator i					= m_before_merge_edges.begin( );

	u32 const index_to_search_for			= (*i).original_edge_id;
	(*i).original_edge_id					= 0;
	convex_type& convex						= m_merged_convexes[ (*i).convex_ids[0] ];
	bool found								= false;
	for ( u32 k = 0, n = convex.second; k < n; ++k ) {
		if ( m_before_merge_edge_indices[ convex.first + k ] == index_to_search_for ) {
			found							= true;
			m_before_merge_edge_indices[ convex.first + k ] = 0;
			break;
		}
	}
	R_ASSERT								( found );

	R_ASSERT								( !m_before_merge_edges.empty() );
	edges_type::const_iterator j			= b + 1;
	for ( ; j != e; ++j ) {
		if ( *i < *j )
			*++i	= *j;
		else {
			R_ASSERT_CMP					( (*i).convex_ids[1], ==, u32(-1) );
			R_ASSERT_CMP					( (*j).convex_ids[0], !=, u32(-1) );
			(*i).convex_ids[1]				= (*j).convex_ids[0];
			(*i).is_obstructed				= (*i).is_obstructed | (*j).is_obstructed;
		}

		u32 const index_to_search_for		= (*j).original_edge_id;
		(*i).original_edge_id				= u32(i - b);
		convex_type& convex					= m_merged_convexes[ (*j).convex_ids[0] ];
		bool found							= false;
		for ( u32 k = 0, n = convex.second; k < n; ++k ) {
			if ( m_before_merge_edge_indices[ convex.first + k ] == index_to_search_for ) {
				found						= true;
				m_before_merge_edge_indices[ convex.first + k ] = u32(i - b);
				break;
			}
		}
		R_ASSERT							( found );
	}

	m_before_merge_edges.erase				(
		i + 1,
		m_before_merge_edges.end()
	);
}

void graph_generator::merge_triangles	( )
{
	if ( m_subdivided_triangles.empty() )
		return;

	{
		indexed_vertices_type				indexed_vertices;
		merge_fill_indexed_vertices			( indexed_vertices );
		merge_remove_duplicated_vertices	( indexed_vertices );
	}

	merge_remove_duplicated_edges			( );
}

#endif // #if XRAY_DEBUG_ALLOCATOR
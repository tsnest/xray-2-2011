////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTRAINED_DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED
#define CONSTRAINED_DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

template< typename VerticesContainer, typename InputIndicesContainer, typename ConstraintContainer >
constrained_delaunay_triangulator::constrained_delaunay_triangulator ( 
		VerticesContainer& vertices,
		InputIndicesContainer& input_indices,
		indices_type& output_indices,
		float3 const& normal,
		ConstraintContainer const& constraint_edges,
		delaunay_triangulator::error_vertices_indices_type* error_vertices_indices,
		u32 const max_delaunay_operation_id,
		bool
	)  :
		m_normal			( normal ),
		m_indices			( output_indices )
{
	void fill_coordinate_indices	( xray::math::float3 const& normal, u32 (&coordinate_indices)[2] );
	u32 coordinate_indices[2];
	fill_coordinate_indices ( m_normal, coordinate_indices );

	m_normal[coordinate_indices[0]] = 0;
	m_normal[coordinate_indices[1]] = 0;
	m_normal = normalize( m_normal );

	u32 const constraint_count = constraint_edges.size();
	for ( u32 i = 0; i < constraint_count; ++i ) {
		u32 vertex_index0 = constraint_edges[i].vertex_index0;
		u32 vertex_index1 = constraint_edges[i].vertex_index1;

		for ( u32 j = 0; j < input_indices.size(); ++j ) {
			if ( input_indices[j] == vertex_index0 || input_indices[j] == vertex_index1 )
				continue;

			R_ASSERT_U( !is_on_segment( coordinate_indices, vertices[ input_indices[j] ], vertices[ vertex_index0 ], vertices[ vertex_index1 ], math::epsilon_7 ), "Constraint edges can't cross vertices and other constraint edges except in constraint edge endpoints" );
		}
	}

	delaunay_triangulator	( vertices, input_indices, output_indices, m_normal, error_vertices_indices);

	if ( error_vertices_indices && !error_vertices_indices->empty() )
		return;

	if ( output_indices.empty() )
		return;

	m_vertices				= &*vertices.begin();

	polygon_indices_type	left_polygon_indices;
	polygon_indices_type	right_polygon_indices;

	u32 current_delaunay_operation_id = 0; 
	typename ConstraintContainer::const_iterator i			= constraint_edges.begin();
	typename ConstraintContainer::const_iterator const e	= constraint_edges.end();
	for ( ; i != e; ++i )  {
		
		if ( current_delaunay_operation_id++ >= max_delaunay_operation_id )
			break;

		left_polygon_indices.clear();
		right_polygon_indices.clear();

		float3 const& segment_vertex0 = m_vertices[ (*i).vertex_index0 ];
		float3 const& segment_vertex1 = m_vertices[ (*i).vertex_index1 ];

		if ( xray::math::is_similar( segment_vertex0[ coordinate_indices[0] ], segment_vertex1[ coordinate_indices[0] ] )
			&& xray::math::is_similar( segment_vertex0[ coordinate_indices[1] ], segment_vertex1[ coordinate_indices[1] ] ) )
		{
			continue;
		}

		if ( !remove_triangles ( coordinate_indices, right_polygon_indices, left_polygon_indices, (*i).vertex_index0, (*i).vertex_index1 ) ) {
			continue;
		}

		triangulate_pseudo_polygon		( coordinate_indices, right_polygon_indices.begin(), right_polygon_indices.end(), (*i).vertex_index0, (*i).vertex_index1 );
		triangulate_pseudo_polygon		( coordinate_indices, left_polygon_indices.begin(), left_polygon_indices.end(), (*i).vertex_index1, (*i).vertex_index0 );

	}

	R_ASSERT_CMP ( m_indices.size() % 3, ==, 0);
}

template< typename VerticesContainer, typename ConstraintContainer >
constrained_delaunay_triangulator::constrained_delaunay_triangulator ( 
		VerticesContainer& vertices,
		indices_type& output_indices,
		float3 const& normal,
		ConstraintContainer const& constraint_edges,
		delaunay_triangulator::error_vertices_indices_type* error_vertices_indices,
		u32 const max_delaunay_operation_id,
		bool keep_bounding_triangle
	)  :
		m_normal			( normal ),
		m_indices			( output_indices )
{
	void fill_coordinate_indices	( xray::math::float3 const& normal, u32 (&coordinate_indices)[2] );
	u32 coordinate_indices[2];
	fill_coordinate_indices ( m_normal, coordinate_indices );

	m_normal[coordinate_indices[0]] = 0;
	m_normal[coordinate_indices[1]] = 0;
	m_normal = normalize( m_normal );

	u32 const constraint_count = constraint_edges.size();
	for ( u32 i = 0; i < constraint_count; ++i ) {
		u32 vertex_index0 = constraint_edges[i].vertex_index0;
		u32 vertex_index1 = constraint_edges[i].vertex_index1;

		if ( vertex_index0 >= vertices.size() || vertex_index1 >= vertices.size() )
			continue;

		for ( u32 j = 0; j < vertices.size(); ++j ) {
			if ( j == vertex_index0 || j == vertex_index1 )
				continue;
	//		bool flag = is_on_segment( coordinate_indices, vertices[j], vertices[vertex_index0], vertices[vertex_index1] );
   //    		R_ASSERT( !flag );
		}
	}

	delaunay_triangulator	( vertices, output_indices, m_normal, error_vertices_indices, u32(-1), keep_bounding_triangle);

	if ( error_vertices_indices && !error_vertices_indices->empty() )
		return;

	if ( output_indices.empty() )
		return;

	m_vertices				= &*vertices.begin();

	polygon_indices_type	left_polygon_indices;
	polygon_indices_type	right_polygon_indices;

	u32 current_delaunay_operation_id = 0; 
	
	ConstraintContainer::const_iterator const	e		= constraint_edges.end();
	for ( ConstraintContainer::const_iterator	it	= constraint_edges.begin(); it != e; ++it )  {
		
		if ( current_delaunay_operation_id++ >= max_delaunay_operation_id )
			break;

		left_polygon_indices.clear();
		right_polygon_indices.clear();

		if ( !remove_triangles ( coordinate_indices, right_polygon_indices, left_polygon_indices, (*it).vertex_index0, (*it).vertex_index1 ) ) {
			continue;
		}

		triangulate_pseudo_polygon		( coordinate_indices, right_polygon_indices.begin(), right_polygon_indices.end(), (*it).vertex_index0, (*it).vertex_index1 );
		triangulate_pseudo_polygon		( coordinate_indices, left_polygon_indices.begin(), left_polygon_indices.end(), (*it).vertex_index1, (*it).vertex_index0 );
	}

	R_ASSERT_CMP ( m_indices.size() % 3, ==, 0);
}


} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef CONSTRAINED_DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED
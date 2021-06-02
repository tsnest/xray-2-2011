////////////////////////////////////////////////////////////////////////////
//	Created		: 19.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "graph_generator_tessellator.h"

using xray::ai::navigation::graph_generator_tessellator;
using xray::ai::navigation::delaunay_triangulator;
using xray::math::float3;

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

bool is_on_segment	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& testee,				// t
		xray::math::float3 const& first_segment_vertex,	// v0
		xray::math::float3 const& second_segment_vertex,// v1
		float epsilon
	);

bool is_point_inside_triangle	(
	u32 const (&coordinate_indices)[2],
	float3 p,
	float3 a,
	float3 b,
	float3 c
);

u32 select_third_edge_id (u32 const edge_id0, u32 const edge_id1 )
{
	R_ASSERT ( edge_id0 < 3 );
	R_ASSERT ( edge_id1 < 3 );
	R_ASSERT ( edge_id0 != edge_id1 );

	if ( edge_id0 == 0 ) {
		if ( edge_id1 == 1 )
			return 2;
		else
			return 1;
	}
	if ( edge_id0 == 1 ) {
		if ( edge_id1 == 0 )
			return 2;
		else
			return 0;
	}

	R_ASSERT ( edge_id0 == 2 );
	if ( edge_id1 == 0 )
		return 1;
	return 0;
}

inline bool graph_generator_tessellator::insert_edge ( edge_intersections_type& edge_intersections, float3 const vertex, u32 const edge_id )
{
	edge_intersections_type::iterator i = std::find_if(
		edge_intersections.begin(),
		edge_intersections.end(),
		compare_edge_intersections_predicate( vertex )
	);
	if ( i == edge_intersections.end() ) {
		edge_intersections.push_back( edge_intersection( vertex, edge_id ) );
		return true;
	}

	return false;
}

bool graph_generator_tessellator::is_edge_endpoint( float3 vertex, u32 triangle_id, u32 edge_id )
{
	u32 const* indices = m_triangles_mesh.indices.begin() + triangle_id * 3;
	if ( is_similar( vertex, m_triangles_mesh.vertices[ indices[ edge_id ] ] )
		|| is_similar( vertex, m_triangles_mesh.vertices[ indices[ (edge_id +1)%3] ] ) )
	{
		return true;
	}

	return false;
}

float distance_to_edge ( float3 const& vertex0, float3 const& vertex1, float3 const& point);

void graph_generator_tessellator::snap_to_edge ( float3& point, float3 const& vertex0, float3 const& vertex1 )
{
	float3 const& t0_t1_direction		= normalize( vertex1 - vertex0 );
	float3 const& t0_s0					= point - vertex0;
	point								= vertex0 + t0_t1_direction*(t0_t1_direction | t0_s0);
}

void graph_generator_tessellator::try_snap_to_closest_edge ( float3& vertex, u32 const triangle_id )
{
	u32 const* indices = m_triangles_mesh.indices.begin() + triangle_id * 3;
	float distance = distance_to_edge( m_triangles_mesh.vertices[ indices[0] ], m_triangles_mesh.vertices[ indices[1] ], vertex );
	u32 closest_id = 0;
	for ( u32 i = 1; i < 3; ++i ) {
		float new_distance = distance_to_edge( m_triangles_mesh.vertices[ indices[i] ], m_triangles_mesh.vertices[ indices[(i+1)%3] ], vertex );
		if ( new_distance < distance ) {
			closest_id = i;
			distance = new_distance;
		}
	}

	if ( distance < math::epsilon_3 ) 
		snap_to_edge ( vertex, m_triangles_mesh.vertices[ indices[closest_id] ], m_triangles_mesh.vertices[ indices[(closest_id+1)%3] ] );
}


bool is_same_side_segment( 
	float3 const& p1, 
	float3 const& p2, 
	float3 const& a, 
	float3 const& b 
);

bool graph_generator_tessellator::subtract_triangle (
		u32 const (&coordinate_indices)[2],
		u32 const triangle_id0, // Who
		u32 const triangle_id1, // From who
		vertices_type& intersection_vertices
	)
{
	bool was_intersected = false;
	u32 const indices0[] = {
		m_triangles_mesh.indices[ 3*triangle_id0 + 0 ],	
		m_triangles_mesh.indices[ 3*triangle_id0 + 1 ],	
		m_triangles_mesh.indices[ 3*triangle_id0 + 2 ]
	};

	float3 const v0	= m_triangles_mesh.vertices[ indices0[0] ];
	float3 const v1	= m_triangles_mesh.vertices[ indices0[1] ];
	float3 const v2	= m_triangles_mesh.vertices[ indices0[2] ];
	float3 const& median = ( v0 + v1 + v2 ) / 3.f;

	u32 const indices1[] = {
		m_triangles_mesh.indices[ 3*triangle_id1 + 0 ],	
		m_triangles_mesh.indices[ 3*triangle_id1 + 1 ],	
		m_triangles_mesh.indices[ 3*triangle_id1 + 2 ]
	};

	float3 const u0	= m_triangles_mesh.vertices[ indices1[0] ];
	float3 const u1	= m_triangles_mesh.vertices[ indices1[1] ];
	float3 const u2	= m_triangles_mesh.vertices[ indices1[2] ];
	u32 subdivide_success_count = 0;

	edge_intersections_type edges_info[3];
	for ( u32 i = 0; i < 3; ++i ) {

		float3 const& edge_vertex0 = m_triangles_mesh.vertices[ indices0[i] ];
		float3 const& edge_vertex1 = m_triangles_mesh.vertices[ indices0[(i+1)%3] ];

		float3 intersections[2];
		for ( u32 j = 0; j < 3; ++j ) {
			bool are_on_the_same_line;
			float3 const& u0 = m_triangles_mesh.vertices[ indices1[j] ];
			float3 const& u1 = m_triangles_mesh.vertices[ indices1[(j+1)%3] ];

			u32 result_count = segment_intersects_segment(
				coordinate_indices, 
				edge_vertex0, 
				edge_vertex1, 
				u0, 
				u1, 
				intersections[0],
				intersections[1],
				are_on_the_same_line
			);

			if ( result_count == 2 ) {
				edges_info[i].clear();
				break;
			}

			for ( u32 k = 0; k < result_count; ++k ) {
				
				try_snap_to_closest_edge( intersections[k], triangle_id1 );

				edge_intersections_type::iterator l = std::find_if(
					edges_info[i].begin(),
					edges_info[i].end(),
					compare_edge_intersections_predicate( intersections[k] )
				);
				
				if ( l == edges_info[i].end() ) {
					edges_info[i].push_back( edge_intersection( intersections[k], j ) );
					R_ASSERT_CMP ( edges_info[i].size(), <=, 2 );
					continue;
				}

				float3 const* testee0 = 0;
				if ( is_similar( u0, edge_vertex0 ) || is_similar( u0, edge_vertex1 ) ) {
					testee0 = &u1;
				} else {
					if ( is_similar( u1, edge_vertex0 ) || is_similar( u1, edge_vertex1 ) )
						testee0 = &u0;
					else {
						intersection_vertices.push_back( intersections[k] );
						return false;
					}
				}
				
				float3 const& testee1 = m_triangles_mesh.vertices[ indices0[(i+2)%3 ] ];
				if ( !is_same_side_segment( *testee0, testee1, edge_vertex0, edge_vertex1 ) ) {
					l->vertex = intersections[k];
					l->intersector_edge_id = j;
				}
			}
		}
	}

	if ( edges_info[0].size() == 1 && edges_info[1].size() == 1 && edges_info[2].size() == 1 ) {
		for ( u32 i = 0; i < 3; ++i ) {
			if ( is_similar( edges_info[i][0].vertex, m_triangles_mesh.vertices[ indices0[i]] )
				|| is_similar( edges_info[i][0].vertex, m_triangles_mesh.vertices[ indices0[(i+1)%3]] ) )
			{
				edges_info[i].clear();
				break;
			}
		}
	}

	u32 last_single_intersected_edge = u32(-1);
	for ( u32 i = 0; i < 3; i++ ) {
		if ( edges_info[i].size() == 2 ) {

			float3 const& vertex0 = m_triangles_mesh.vertices[ indices0[i] ];
			float3 const& vertex1 = m_triangles_mesh.vertices[ indices0[ (i+1) % 3 ] ];

			for ( u32 j = 0; j < 3; ++j ) {
				if ( is_similar( vertex0, m_triangles_mesh.vertices[ indices1[(j+1) % 3] ] )
					&& is_similar( vertex1, m_triangles_mesh.vertices[ indices1[j] ] ) )
				{
					return false;
				}
			}
		}
	}

	last_single_intersected_edge = u32(-1);
	for ( u32 i = 0; i < 3; ++i ) {

		u32 const intersections_count = edges_info[i].size();
		if ( intersections_count == 2 ) {
			was_intersected = true;

			intersection_vertices.push_back( edges_info[i][0].vertex );
			intersection_vertices.push_back( edges_info[i][1].vertex );
		
			/*
			for ( u32 j = 0; j < 2; ++j ) {
				float3 const& edge_vertex0 = m_triangles_mesh.vertices[ indices1[ edges_info[i][j].intersector_edge_id ] ];
				float3 const& edge_vertex1 = m_triangles_mesh.vertices[ indices1[ (edges_info[i][j].intersector_edge_id+1) % 3 ] ];
				if ( is_similar( edges_info[i][j].vertex, edge_vertex0 ) ||
					is_similar( edges_info[i][j].vertex, edge_vertex1 ) )
				{
					edges_info[i][j].intersector_edge_id = edges_info[i][(j+1)%2].intersector_edge_id;
				}
			}
			*/

			u32 non_intersected_edge_id;
			if ( edges_info[i][0].intersector_edge_id == edges_info[i][1].intersector_edge_id )
				non_intersected_edge_id = edges_info[i][0].intersector_edge_id;
			else
				non_intersected_edge_id = select_third_edge_id( edges_info[i][0].intersector_edge_id, edges_info[i][1].intersector_edge_id );

			float3 const& testee = m_triangles_mesh.vertices[ indices1[ (non_intersected_edge_id+2) % 3 ] ];
			float3 const& vertex0 = m_triangles_mesh.vertices[ indices0[i] ];
			float3 const& vertex1 = m_triangles_mesh.vertices[ indices0[(i+1) % 3] ];
			
			float3 intersections[] = {
				edges_info[i][0].vertex,
				edges_info[i][1].vertex,
			};

			float squared_length0 = squared_length ( intersections[0] - vertex0 );
			float squared_length1 = squared_length ( intersections[1] - vertex0 );
			if ( squared_length1 < squared_length0 )
				std::swap( intersections[0], intersections[1] );

			if ( is_same_side_segment ( median, testee, vertex0, vertex1 ) ) {
				float3 const u0 = m_triangles_mesh.vertices[ indices1[(non_intersected_edge_id+0) % 3] ];
				float3 const u1 = m_triangles_mesh.vertices[ indices1[(non_intersected_edge_id+1) % 3] ];
					
				if ( is_on_segment ( coordinate_indices, u0, vertex0, vertex1 ) && is_on_segment ( coordinate_indices, u1, vertex0, vertex1 ) ) {
					continue;
				}

				if ( is_same_side_segment( median, u0, vertex0, vertex1 ) && is_same_side_segment( median, u1, vertex0, vertex1 ) ) {
					continue;
				}

				subdivide_success_count += add_output_triangle ( triangle_id1, u0, intersections[1], intersections[0] ) ? 1 : 0;
				subdivide_success_count += add_output_triangle ( triangle_id1, u0, u1, intersections[1]  ) ? 1 : 0;

			} else {
				subdivide_success_count += add_output_triangle ( triangle_id1, testee, intersections[1], intersections[0] ) ? 1 : 0;
			}
		}

		if ( intersections_count == 1) {
			was_intersected = true;

			intersection_vertices.push_back ( edges_info[i][0].vertex );

			if ( last_single_intersected_edge == u32(-1) ) {
				last_single_intersected_edge = i;
				continue;
			}

			u32 intersector_edge0 = edges_info[i][0].intersector_edge_id;
			u32 intersector_edge1 = edges_info[last_single_intersected_edge][0].intersector_edge_id;
		
			float3 intersections[] = {
				edges_info[i][0].vertex,
				edges_info[last_single_intersected_edge][0].vertex,
			};

			if ( is_similar( intersections[0], intersections[1], math::epsilon_5 ) )
				continue;

			if ( is_edge_endpoint( intersections[0], triangle_id1, intersector_edge0 )
				&& is_edge_endpoint( intersections[1], triangle_id1, intersector_edge1 ) )
			{
				continue;
			}

			u32 const edge_inside_subdivided_triangle = select_third_edge_id(i, last_single_intersected_edge );
			float3 const testee = m_triangles_mesh.vertices[ indices0[(edge_inside_subdivided_triangle + 2) % 3] ];

			if ( intersector_edge0 == intersector_edge1 ) {

				float3 const vertex0 = m_triangles_mesh.vertices[ indices1[intersector_edge0] ];
				float3 const vertex1 = m_triangles_mesh.vertices[ indices1[(intersector_edge0+1) % 3] ];
				float3 const vertex2 = m_triangles_mesh.vertices[ indices1[(intersector_edge0+2) % 3] ];

				float squared_length0 = squared_length ( intersections[0] - vertex0 );
				float squared_length1 = squared_length ( intersections[1] - vertex0 );
				if ( squared_length1 < squared_length0 )
					std::swap( intersections[0], intersections[1] );

				if (is_similar( intersections[0], vertex0 ) && is_similar( intersections[1], vertex1 ) ) {
					continue;
				}

				if ( is_point_inside_triangle( coordinate_indices, testee, u0, u1, u2 ) ) {
					subdivide_success_count += add_output_triangle (triangle_id1, testee, intersections[1], vertex1 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, testee, vertex1, vertex2 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, testee, vertex2, vertex0 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, testee, vertex0, intersections[0] ) ? 1 : 0;
				} else {
					float3 const t0 = m_triangles_mesh.vertices[ indices0[(edge_inside_subdivided_triangle + 0) % 3] ];
					float3 const t1 = m_triangles_mesh.vertices[ indices0[(edge_inside_subdivided_triangle + 1) % 3] ];

					if ( is_similar( t0, intersections[0] ) && is_similar( t1, intersections[1] ) ) {
						continue;
					}

					subdivide_success_count += add_output_triangle (triangle_id1, intersections[1], vertex1, t0 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t0, vertex1, vertex2 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t0, vertex2, t1 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t1, vertex2, vertex0 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t1, vertex0, intersections[0] ) ? 1 : 0;
				}
			} else {
				u32 non_intersected_edge = select_third_edge_id( intersector_edge0, intersector_edge1 );
				
				float3 const vertex0 = m_triangles_mesh.vertices[ indices1[ non_intersected_edge ] ];
				float3 const vertex1 = m_triangles_mesh.vertices[ indices1[ (non_intersected_edge+1)%3 ] ];

				float squared_length0 = squared_length( intersections[0] - vertex0 );
				float squared_length1 = squared_length( intersections[1] - vertex0 );
				if ( squared_length1 < squared_length0 ) 
					std::swap( intersections[0], intersections[1] );

				if (is_similar( intersections[0], vertex0 ) && is_similar( intersections[1], vertex1 ) )
					continue;

				if ( is_point_inside_triangle( coordinate_indices, testee, u0, u1, u2) ) {
					subdivide_success_count += add_output_triangle (triangle_id1, testee, intersections[0], vertex0 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, testee, vertex0, vertex1 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, testee, vertex1, intersections[1] ) ? 1 : 0;
				} else {
					float3 const t0 = m_triangles_mesh.vertices[ indices0[(edge_inside_subdivided_triangle + 0) % 3] ];
					float3 const t1 = m_triangles_mesh.vertices[ indices0[(edge_inside_subdivided_triangle + 1) % 3] ];

					if ( is_similar( t1, intersections[0] ) && is_similar( t0, intersections[1] ) ) {
						continue;
					}

					subdivide_success_count += add_output_triangle (triangle_id1, t0, intersections[0], vertex0 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t0, vertex0, vertex1 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t0, vertex1, t1 ) ? 1 : 0;
					subdivide_success_count += add_output_triangle (triangle_id1, t1, vertex1, intersections[1] ) ? 1 : 0;

				}
			}
		}
	}
	return was_intersected;
}

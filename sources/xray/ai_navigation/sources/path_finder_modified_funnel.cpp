////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "path_finder_modified_funnel.h"
#include "navigation_mesh_functions.h"

using xray::ai::navigation::path_finder_modified_funnel;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float2;
using xray::math::float3;

float3 make_float3	( 
		u32 const (&coordinate_indices)[2],
		float2 const& value
	)
{
	float3 result;
	result[ coordinate_indices[0] ]	= value.x;
	result[ coordinate_indices[1] ]	= value.y;
	return							result;
}

std::pair< float3, float3 > compute_tangent	(
		float3 const& first_circle_center,
		float3 const& second_circle_center,
		u32 const (&coordinate_indices)[2],
		float const radius,	// common circle radius
		bool is_first_left,
		bool is_second_left
	)
{
	if ( is_first_left == is_second_left ) {
		float3 const& direction		= normalize( second_circle_center - first_circle_center );

		float3 orthogonal_direction	= float3( 0.f, 0.f, 0.f );
		orthogonal_direction[ coordinate_indices[0] ]	= -direction[ coordinate_indices[1] ];
		orthogonal_direction[ coordinate_indices[1] ]	= +direction[ coordinate_indices[0] ];

		return
			std::make_pair			(
				first_circle_center  + orthogonal_direction * radius * ( is_first_left ? 1.0f : -1.0f ),
				second_circle_center + orthogonal_direction * radius * ( is_first_left ? 1.0f : -1.0f )
			);
	}

	float2 const& a					= float2( first_circle_center[ coordinate_indices[0] ], first_circle_center[ coordinate_indices[1] ] );
	float2 const& b					= float2( second_circle_center[ coordinate_indices[0] ], second_circle_center[ coordinate_indices[1] ] );
	float2 const& middle_a_b_minus_a = (b + a)/2 - a;
	float const middle_a_b_minus_a_length	= middle_a_b_minus_a.length();
	float2 const& direction			= middle_a_b_minus_a/middle_a_b_minus_a_length;
	float2 const directions[] = {
		float2( direction.y, -direction.x ), // right
		float2( -direction.y, direction.x ), // left
	};
	float const projection_on_ab	= xray::math::sqr(radius)/middle_a_b_minus_a_length;
	R_ASSERT_CMP					( radius, >=, projection_on_ab, "it is impossible to construct valid tangents; path channel is non-passable!");
	float const projection2			= xray::math::sqrt( xray::math::sqr(radius) - xray::math::sqr(projection_on_ab) );
	return
		std::make_pair(
			make_float3( coordinate_indices, a + projection2*directions[is_first_left]  + projection_on_ab*direction ),
			make_float3( coordinate_indices, b + projection2*directions[is_second_left] - projection_on_ab*direction )
		);
}

std::pair< float3, float3 > compute_tangent	(
		float3 const& start_position,
		float3 const& circle_center,
		u32 const (&coordinate_indices)[2],
		float const radius,
		bool is_circle_left
	)
{
	float2 const& a					= float2( start_position[ coordinate_indices[0] ], start_position[ coordinate_indices[1] ] );
	float2 const& b					= float2( circle_center[ coordinate_indices[0] ], circle_center[ coordinate_indices[1] ] );
	float2 const& b_minus_a			= b - a;
	float const b_minus_a_length	= b_minus_a.length();
	float2 const& direction			= b_minus_a / b_minus_a_length;
	float2 const directions[] = {
		float2( direction.y, -direction.x ), // right
		float2( -direction.y, direction.x ), // left
	};
	float const projection_on_ab	= xray::math::sqr(radius)/b_minus_a_length;
	R_ASSERT_CMP					( radius, >=, projection_on_ab, "it is impossible to construct valid tangents; path channel is non-passable!");
	float const projection2			= xray::math::sqrt( xray::math::sqr(radius) - xray::math::sqr(projection_on_ab) );
	return
		std::make_pair(
			make_float3( coordinate_indices, a ),
			make_float3( coordinate_indices, b + projection2*directions[is_circle_left] - projection_on_ab*direction )
		);
}

void path_finder_modified_funnel::add_path_vertex( float3 const& vertex )
{
	m_path.push_back( vertex + float3( 0, 0.1f, 0) );
}

float3 path_finder_modified_funnel::extract_non_edge_vertex( u32 triangle_id0, u32 triangle_id1 )
{
	u32 edge_id = get_similar_edge( triangle_id0, triangle_id1 );
	R_ASSERT_CMP(edge_id, !=, u32(-1));
	u32 index = triangle_id0 * 3 + (( edge_id + 2 ) % 3);
	return m_triangles_mesh.vertices[ m_triangles_mesh.indices[ index ] ];
}

u32 path_finder_modified_funnel::get_similar_edge ( u32 triangle_id0, u32 triangle_id1 ) 
{
	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_triangles_mesh.data[ triangle_id0 ].neighbours[i] == triangle_id1 ) {
			return i;
		}
	}
	return u32(-1);
}

float cross_product	(
		u32 const (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1
	);


std::pair< float3, float3 > path_finder_modified_funnel::compute_wedge ( float3 const center0, enum_vertex_type type0, float3 const center1, enum_vertex_type type1 )
{
	if ( type0 == vertex_type_point && type1 == vertex_type_point )
		return std::make_pair( center0, center1 );

	u32 const coordinate_indices[2] = { 0, 2 };
	if ( type0 == vertex_type_point ) {
		return compute_tangent( center0, center1, coordinate_indices, m_radius, type1 == vertex_type_right );
	} else if ( type1 == vertex_type_point ) {
		std::pair< float3, float3 > result = compute_tangent( center1, center0, coordinate_indices, m_radius, type0 == vertex_type_left );
		std::swap( result.first, result.second );
		return result;
	}
	return compute_tangent ( center0, center1, coordinate_indices, m_radius, type0 == vertex_type_right, type1 == vertex_type_right );
}

void path_finder_modified_funnel::add ( float3 const& vertex, enum_vertex_type type )
{
	u32 const coordinate_indices[2] = { 0, 2 };

	if ( type == vertex_type_left ) {
		for ( ; ; ) {
			funnel_type::iterator left = m_funnel.begin();
			funnel_type::iterator right = m_funnel.end() - 1;

			float l0, l1;
			std::pair< float3, float3 > s0, s1;

			if ( left == right ) {
				m_funnel.push_front( funnel( vertex, type ) );
				return;
			} else if ( left->vertex == m_funnel_apex.vertex ) {
				s0 = compute_wedge ( left->vertex, left->type, (left+1)->vertex, (left+1)->type );
				l1 = length ( left->vertex - vertex );
			} else {
				s0 = compute_wedge ( (left+1)->vertex, (left+1)->type, left->vertex, left->type );
				l1 = length ( (left+1)->vertex - vertex );
			}
			
			l0 = length( left->vertex - (left+1)->vertex );
			s1 = compute_wedge ( (left+1)->vertex, (left+1)->type, vertex, type );
			
			if ( cross_product( coordinate_indices, (s0.second - s0.first), (s1.second - s1.first) ) >= 0 ) {
				m_funnel.push_front( funnel( vertex, type ) );
				return;
			}

			if ( left->vertex == m_funnel_apex.vertex && l1 < l0 ) {
				m_funnel.push_front( funnel( vertex, type ) );
				m_funnel_apex = m_funnel.front();
				m_intermediate_path.push_back( m_funnel_apex );
				return;
			} else if ( left->vertex == m_funnel_apex.vertex ) {
				m_funnel.pop_front();
				m_funnel_apex = m_funnel.front();
				m_intermediate_path.push_back( m_funnel_apex );
				continue;
			}
			m_funnel.pop_front();
		}
	} else if ( type == vertex_type_right ) {
		for ( ; ; ) {
			funnel_type::iterator left = m_funnel.begin();
			funnel_type::iterator right = m_funnel.end() - 1;

			float l0, l1;
			std::pair< float3, float3 > s0, s1;

			if ( right == left ) {
				m_funnel.push_back( funnel( vertex, type ) );
				return;
			} else if ( right->vertex == m_funnel_apex.vertex ) {
				s0 = compute_wedge ( (right)->vertex, right->type, (right-1)->vertex, (right-1)->type );
				l1 = length ( right->vertex - vertex );
			} else {
				s0 = compute_wedge ( (right-1)->vertex, (right-1)->type, (right)->vertex, right->type );
				l1 = length ( (right-1)->vertex - vertex );
			}
			
			l0 = length( right->vertex - (right-1)->vertex );
			s1 = compute_wedge ( (right-1)->vertex, (right-1)->type, vertex, type );
			
			if ( cross_product( coordinate_indices, (s0.second - s0.first), (s1.second - s1.first) ) <= 0 ) {
				m_funnel.push_back( funnel( vertex, type ) );
				return;
			}

			if ( right->vertex == m_funnel_apex.vertex && l1 < l0 ) {
				m_funnel.push_back( funnel( vertex, type ) );
				m_funnel_apex = m_funnel.back();
				m_intermediate_path.push_back( m_funnel_apex );
				return;
			} else if ( right->vertex == m_funnel_apex.vertex ) {
				m_funnel.pop_back();
				m_funnel_apex = m_funnel.back();
				m_intermediate_path.push_back( m_funnel_apex );
				continue;
			}
			m_funnel.pop_back();
		}
	} else if ( type == vertex_type_point ) {
		funnel_type::iterator i = m_funnel.begin();
		funnel_type::iterator left = m_funnel.begin();
		funnel_type::iterator right = m_funnel.end()-1;
		for ( ; i != right; ++i ) {
			if ( m_funnel_apex.vertex == (*i).vertex )
				break;
		}

		std::pair< float3, float3> wedge_left;
		std::pair< float3, float3> wedge_right;
		if ( i == left  || i == right ) {
			m_intermediate_path.push_back ( funnel( vertex, vertex_type_point ) );
			return;
		}
		wedge_left = compute_wedge( m_funnel_apex.vertex, m_funnel_apex.type, (i-1)->vertex, (i-1)->type  );
		wedge_right = compute_wedge( m_funnel_apex.vertex, m_funnel_apex.type, (i+1)->vertex, (i+1)->type  );

		std::pair< float3, float3> wedge_vertex = compute_wedge( m_funnel_apex.vertex, m_funnel_apex.type, vertex, vertex_type_point );

		if ( i != left && cross_product ( coordinate_indices, wedge_left.second - wedge_left.first, wedge_vertex.second - wedge_vertex.first ) >= 0 ) {
			while ( i != left ) {
				std::pair< float3, float3 > wedge = compute_wedge ( i->vertex, i->type, (i-1)->vertex, (i-1)->type );
				wedge_vertex = compute_wedge( i->vertex, i->type, vertex, vertex_type_point );
				float factor = cross_product ( coordinate_indices, wedge.second - wedge.first, wedge_vertex.second - wedge_vertex.first );
				if ( factor < 0 )
					break;
				if ( (i-1) == left ) {
					if ( squared_length( (i-1)->vertex - i->vertex ) > squared_length( vertex - i->vertex ) )
						break;
				}
				m_intermediate_path.push_back( *(i-1) );
				--i;
			}
		} else if ( i != right && cross_product( coordinate_indices, wedge_right.second - wedge_right.first, wedge_vertex.second - wedge_vertex.first ) <= 0 ) {
			while ( i != right ) {

				std::pair< float3, float3 > wedge = compute_wedge ( i->vertex, i->type, (i+1)->vertex, (i+1)->type );
				wedge_vertex = compute_wedge( i->vertex, i->type, vertex, vertex_type_point );
				float factor = cross_product ( coordinate_indices, wedge.second - wedge.first, wedge_vertex.second - wedge_vertex.first );
				if ( factor > 0 )
					break;
				if ( (i+1) == right ) {
					if ( squared_length( (i+1)->vertex - i->vertex ) > squared_length( vertex - i->vertex ) )
						break;
				}

				m_intermediate_path.push_back( *(i+1) );
				++i;
			}
		}

		m_intermediate_path.push_back ( funnel( vertex, vertex_type_point ) );
	}
}
	
void path_finder_modified_funnel::build_path( float height ) 
{
	funnel_type::iterator i = m_intermediate_path.begin() + 1;
	funnel_type::iterator const e = m_intermediate_path.end();
	for ( ; i != e; ++i ) {
		if ( (i-1)->type == vertex_type_point && i->type == vertex_type_point ) {
			add_path_vertex( (i-1)->vertex );
			add_path_vertex( i->vertex );
			continue;
		}
		std::pair< float3, float3 > wedge = compute_wedge ( (i-1)->vertex, (i-1)->type, i->vertex, i->type );
		// correct height
		wedge.first.y = (i-1)->vertex.y;
		wedge.second.y = i->vertex.y;
		add_path_vertex ( wedge.first );
		add_path_vertex ( wedge.second );
	}
}

path_finder_modified_funnel::path_finder_modified_funnel (
		triangles_mesh_type const&	triangles_mesh, 
		channel_type	const&	channel,
		float3			const&	start,
		float3			const&	goal,
		float			const	radius,
		path_type&				path
	) :
		m_triangles_mesh	( triangles_mesh ),
		m_funnel_apex		( funnel( start, vertex_type_point ) ),
		m_channel			( channel ),
		m_radius			( radius ),
		m_path				( path )
{
	m_path.clear();

	if ( m_channel.size() == 1 ) {
		u32 const coordinate_indices[] = { 0, 2 };
		u32 const node = m_channel[0];
		u32 const* indices = triangles_mesh.indices.begin() + node*3;
		for ( u32 i = 0; i < 3; ++i ) {
			float3 const& vertex = triangles_mesh.vertices[ indices[i] ];
			float distance = distance_to_segment( vertex, start, goal );
			if ( distance < m_radius ) {
				bool is_left = cross_product( coordinate_indices, goal - start, vertex - start ) < 0.0f;
				std::pair< float3, float3 > tangent = compute_tangent (start, vertex, coordinate_indices, m_radius, is_left);
				tangent.first.y = tangent.second.y = vertex.y;
				add_path_vertex( tangent.first );
				add_path_vertex( tangent.second );

				tangent = compute_tangent (goal, vertex, coordinate_indices, m_radius, !is_left);
				tangent.first.y = tangent.second.y = vertex.y;
				add_path_vertex( tangent.second );
				add_path_vertex( tangent.first );
				return;
			}
		}
		add_path_vertex( start );
		add_path_vertex( goal );
		return;
	}

	m_funnel.push_back( funnel( start, vertex_type_point ) );
	m_intermediate_path.push_back( m_funnel_apex );

	u32 similar_edge = get_similar_edge( m_channel[0], m_channel[1] );
	u32 const* indices = m_triangles_mesh.indices.begin() + m_channel[0] * 3;
	add( m_triangles_mesh.vertices[ indices[ similar_edge ] ], vertex_type_left );
	add( m_triangles_mesh.vertices[ indices[ (similar_edge + 1) % 3] ], vertex_type_right );

	u32 v0 = indices[ similar_edge ];
	u32 v1 = indices[ (similar_edge + 1) % 3];

	channel_type::const_iterator i = channel.begin() + 1; 
	channel_type::const_iterator e = channel.end() - 1;
	for ( ; i != e; ++i ) {
		u32 const* indices = m_triangles_mesh.indices.begin() + (*i) * 3;
		u32 edge_id = get_similar_edge( (*i), *(i+1));
		u32 u0 = indices[ edge_id ];
		u32 u1 = indices[ ( edge_id + 1 ) % 3 ];

		if ( v0 == u0 ) {
			// Means Right Side
			v1 = u1;
			add( m_triangles_mesh.vertices[ v1 ], vertex_type_right );
		} else {
			// Means Left side
			v0 = u0;
			add( m_triangles_mesh.vertices[ v0 ], vertex_type_left );
		}
	}
	add( goal, vertex_type_point );
	build_path( start.y );
}

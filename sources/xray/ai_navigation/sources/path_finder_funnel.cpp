////////////////////////////////////////////////////////////////////////////
//	Created		: 06.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "path_finder_funnel.h"

using xray::ai::navigation::path_finder_funnel;
using xray::ai::navigation::triangles_mesh_type;
using xray::math::float3;

float cross_product	(
		u32 const (&coordinate_indices)[2],
		float3 const& vertex0,
		float3 const& vertex1
	);

void path_finder_funnel::add_path_vertex( float3 const& vertex )
{
	m_path.push_back( vertex + float3( 0, 0.1f, 0) );
}

void path_finder_funnel::add( float3 vertex, enum_vertex_type type) 
{
	u32 coordinate_indices[2] = { 0, 2 };

	if ( type == vertex_type_left ) {
		for ( ; ; ) {
			funnel_type::iterator left = m_funnel.begin();
			funnel_type::iterator right = m_funnel.end() - 1;
			float factor;
			if ( left == right ) {
				m_funnel.push_front( vertex );
				return;
			} else if ( (*left) == m_funnel_apex ) {
				factor = cross_product( coordinate_indices, (*(left+1))-(*(left)), (*left) - vertex);
			} else {
				factor = cross_product( coordinate_indices, (*left)-(*(left+1)), (*left) - vertex);
			}

			if ( factor <= 0.0f ) {
				m_funnel.push_front( vertex );
				return;
			}
			if ( (*left) == m_funnel_apex ) {
				m_funnel.pop_front();
				m_funnel_apex = m_funnel.front();
				add_path_vertex( m_funnel.front() );
				continue;
			}
			m_funnel.pop_front();
		}
	} else if ( type == vertex_type_right ) {
		for ( ; ; ) {
			funnel_type::iterator left = m_funnel.begin();
			funnel_type::iterator right = m_funnel.end() - 1;
			float factor;
			if ( right == left ) {
				m_funnel.push_back( vertex );
				return;
			} else if ( (*right) == m_funnel_apex ) {
				factor = cross_product( coordinate_indices, (*(right-1))-(*(right)), vertex - (*right));
			} else {
				factor = cross_product( coordinate_indices, (*right)-(*(right-1)), vertex - (*right));
			}

			if ( factor <= 0.0f ) {
				m_funnel.push_back( vertex );
				return;
			}
			if ( (*right) == m_funnel_apex ) {
				m_funnel.pop_back();
				m_funnel_apex = m_funnel.back();
				add_path_vertex( m_funnel.back() );
				continue;
			}
			m_funnel.pop_back();
		}
	} else if ( type == vertex_type_point ) {
		funnel_type::iterator i = m_funnel.begin();
		funnel_type::iterator left = m_funnel.begin();
		funnel_type::iterator right = m_funnel.end()-1;
		for ( ; i != right; ++i ) {
			if ( m_funnel_apex == (*i) )
				break;
		}

		float3 const wedge_left = (*(i-1)) - m_funnel_apex;
		float3 const wedge_right = (*(i+1)) - m_funnel_apex;
		float3 const wedge_vertex = vertex - m_funnel_apex;

		if ( i != left && cross_product ( coordinate_indices, wedge_left, wedge_vertex ) >= 0 ) {
			while ( (*i) != (*left) ) {
				float factor = cross_product ( coordinate_indices, (*(i-1)) - (*i), vertex - (*i) );
				if ( factor < 0 )
					break;
				add_path_vertex ( *(i-1) );
				--i;
			}
		} else if ( i != right && cross_product( coordinate_indices, wedge_right, wedge_vertex ) <= 0 ) {
			while ( (*i) != (*right) ) {
				float factor = cross_product ( coordinate_indices, (*(i+1)) - (*i), vertex - (*i) );
				if ( factor > 0 )
					break;
				add_path_vertex ( *(i+1) );
				++i;
			}
		}
		add_path_vertex( vertex );
	}
}

float3 path_finder_funnel::extract_non_edge_vertex( u32 triangle_id0, u32 triangle_id1 )
{
	u32 edge_id = get_similar_edge( triangle_id0, triangle_id1 );
	R_ASSERT_CMP(edge_id, !=, u32(-1));
	u32 index = triangle_id0 * 3 + (( edge_id + 2 ) % 3);
	return m_triangles_mesh.vertices[ m_triangles_mesh.indices[ index ] ];
}

u32 path_finder_funnel::get_similar_edge ( u32 triangle_id0, u32 triangle_id1 ) 
{
	for ( u32 i = 0; i < 3; ++i ) {
		if ( m_triangles_mesh.data[ triangle_id0 ].neighbours[i] == triangle_id1 ) {
			return i;
		}
	}
	return u32(-1);
}

path_finder_funnel::path_finder_funnel ( 
		triangles_mesh_type const&	triangles_mesh, 
		channel_type	const&		channel,
		float3			const&		start,
		float3			const&		goal,
		path_type&					path
	) :
		m_funnel_apex		( start ),
		m_path				( path ),
		m_channel			( channel ),
		m_triangles_mesh	( triangles_mesh )
{

	m_path.clear();
	m_funnel_apex = start;
	m_funnel.push_back( start );
	add_path_vertex( start );
	
	if ( m_channel.size() == 1 ) {
		add_path_vertex( goal );
		return;
	}
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
}

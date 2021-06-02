////////////////////////////////////////////////////////////////////////////
//	Created		: 23.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED
#define DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED

#include <xray/math_randoms_generator.h>

namespace xray {
namespace ai {
namespace navigation {

inline bool delaunay_triangulator::is_vertex_of_the_bounding_triangle	( u32 const vertices_size, u32 const vertex_id )
{
	return						vertex_id >= (vertices_size - 3);
}

template < typename VerticesContainerType >
inline void delaunay_triangulator::add_bounding_triangle	(
		VerticesContainerType& vertices,
		float3 const& normal,
		u32 (&coordinate_indices)[2],
		float3 const& min_position,
		float3 const& max_position
	)
{
	fill_coordinate_indices			( normal, coordinate_indices );

	float3 const& center			= 0.5f * (min_position + max_position);
	float3 const& maximum			= 1.01f * math::max( xray::math::abs(min_position), xray::math::abs(max_position), float3( math::epsilon_3, math::epsilon_3, math::epsilon_3 ) );

	float3 vertex(0,0,0);

	vertex[ coordinate_indices[0] ]	= -3.f*maximum[ coordinate_indices[0] ] + center[ coordinate_indices[0] ];
	vertex[ coordinate_indices[1] ]	= -3.f*maximum[ coordinate_indices[1] ] + center[ coordinate_indices[1] ];
	vertices.push_back	( vertex );

	vertex[ coordinate_indices[0] ]	= 0.f + center[ coordinate_indices[0] ];
	vertex[ coordinate_indices[1] ]	= 3.f*maximum[ coordinate_indices[1] ] + center[ coordinate_indices[1] ];
	vertices.push_back	( vertex );

	vertex[ coordinate_indices[0] ]	= 3.f*maximum[ coordinate_indices[0] ] + center[ coordinate_indices[0] ];
	vertex[ coordinate_indices[1] ]	= 0.f + center[ coordinate_indices[1] ];
	vertices.push_back	( vertex );

	m_vertices_size		= vertices.size();
	m_indices.push_back	( m_vertices_size - 3 );
	m_indices.push_back	( m_vertices_size - 2 );
	m_indices.push_back	( m_vertices_size - 1 );

	float cross_product	(
		u32 const (&coordinate_indices)[2],
		xray::math::float3 const& vertex0,
		xray::math::float3 const& vertex1
	);

	m_ordering_sign		= math::sign(
		cross_product(
			coordinate_indices,
			vertices[ m_vertices_size - 2] - vertices[ m_vertices_size - 3],
			vertices[ m_vertices_size - 1] - vertices[ m_vertices_size - 3]
		)
	);
}

// http://groups.csail.mit.edu/graphics/classes/6.838/F01/lectures/Delaunay/Delaunay2D.ppt
template < typename VerticesContainerType >
inline void delaunay_triangulator::add_bounding_triangle (
		float3 const& normal,
		VerticesContainerType& vertices,
		u32 (&coordinate_indices)[2]
	)
{
	fill_coordinate_indices			( normal, coordinate_indices );

	float3 min_position				= float3(  math::infinity,  math::infinity,  math::infinity );
	float3 max_position				= float3( -math::infinity, -math::infinity, -math::infinity );
	VerticesContainerType::const_iterator const b	= vertices.begin( );
	VerticesContainerType::const_iterator const e	= vertices.end( );
	for (VerticesContainerType::const_iterator i = b; i != e; ++i ) {
		float3 const& vertex		= *i;
		min_position				= math::min( min_position, vertex );
		max_position				= math::max( max_position, vertex );
	}

	add_bounding_triangle			( vertices, normal, coordinate_indices, min_position, max_position );
}

template < typename VerticesContainerType, typename IndicesContainerType >
inline void delaunay_triangulator::add_bounding_triangle (
		float3 const& normal,
		VerticesContainerType& vertices,
		IndicesContainerType const& indices,
		u32 (&coordinate_indices)[2]
	)
{
	float3 min_position				= float3(  math::infinity,  math::infinity,  math::infinity );
	float3 max_position				= float3( -math::infinity, -math::infinity, -math::infinity );
	typedef typename IndicesContainerType::const_iterator const_iterator_type;
	const_iterator_type const b		= indices.begin( );
	const_iterator_type const e		= indices.end( );
	for (indices_type::const_iterator i = b; i != e; ++i ) {
		float3 const& vertex		= vertices[*i];
		min_position				= math::min( min_position, vertex );
		max_position				= math::max( max_position, vertex );
	}

	add_bounding_triangle			( vertices, normal, coordinate_indices, min_position, max_position );
}

template < typename ContainerType >
void delaunay_triangulator::remove_bounding_triangle ( ContainerType& vertices )
{
	m_indices.erase					(
		std::remove_if(
			m_indices.begin( ),
			m_indices.end( ),
			bounding_triangle_filter( vertices.size(), m_indices )
		),
		m_indices.end( )
	);
	vertices.resize					( vertices.size() - 3 );
}

template <typename VerticesContainer>
delaunay_triangulator::delaunay_triangulator(
		VerticesContainer& vertices,
		indices_type& indices,
		float3 const& normal,
		error_vertices_indices_type* error_vertices_indices,
		u32 max_delaunay_operation_id,
		bool keep_bounding_triangle
	) :
	m_indices						( indices )
{
	R_ASSERT_CMP					( vertices.size(), >=, 3 );

	u32 coordinate_indices[2];

	u32 vertices_pair[2] = { u32(-1), u32(-1) };
	float min_distance = math::infinity;
	u32 const vertices_count = vertices.size();
	for ( u32 i = 0; i < vertices_count; ++i ) {
		for ( u32 j = i+1; j < vertices_count; ++j ) {
			float distance = length(vertices[i] - vertices[j]);
			if ( vertices_pair[0] == u32(-1) || distance < min_distance ) {
				min_distance = distance;
				vertices_pair[0] = i;
				vertices_pair[1] = j;
			}
		}
	}

	for ( u32 i = 0; i < vertices_count; ++i ) {
		float distance0 = length( vertices[i] - vertices[ vertices_pair[0] ] ); 
		float distance1 = length( vertices[i] - vertices[ vertices_pair[1] ] );
		if ( distance1 < distance0 )
			distance0 = distance1;
		
		if ( xray::math::is_relatively_similar( distance0, distance0 + min_distance, math::epsilon_5 ) ) {
			if ( error_vertices_indices )
				error_vertices_indices->push_back(i);
			else {
				DEBUG_BREAK();
				return;
			}
		}
	}

	if ( error_vertices_indices && !error_vertices_indices->empty() )
		return;

	bool done = true;
	xray::math::random32 random		= xray::math::random32( 0 );
	for ( u32 shuffle_count = 0; shuffle_count < 1024; ++shuffle_count ) {
		done = true;
		indices.clear();
		buffer_vector<u32> input_indices( ALLOCA( vertices.size() * sizeof(u32) ), vertices.size() );
		for ( u32 i = 0; i < vertices.size(); ++i )
			input_indices.push_back( i );

		std::random_shuffle			( input_indices.begin(), input_indices.end(), random );
		add_bounding_triangle		( normal, vertices, input_indices, coordinate_indices );

		m_vertices					= &*vertices.begin();

		buffer_vector<u32>::const_iterator i		= input_indices.begin( );
		buffer_vector<u32>::const_iterator const e	= input_indices.end( );
		for ( u32 op = 0; i != e && op < max_delaunay_operation_id; ++i, ++op ) {
			if ( !add_vertex_to_triangulation ( coordinate_indices, *i ) ) {
				done = false;
				break;
			}
		}

		if ( !done || !keep_bounding_triangle )
			remove_bounding_triangle			( vertices );

		if ( done )
			break;
	}

	R_ASSERT								( done );

}

template < typename VerticesContainer, typename InputIndicesContainer >
delaunay_triangulator::delaunay_triangulator(
		VerticesContainer& vertices,
		InputIndicesContainer& input_indices,
		indices_type& output_indices,
		float3 const& normal,
		error_vertices_indices_type* error_vertices_indices
	) :
	m_indices						( output_indices )
{
	R_ASSERT_CMP					( vertices.size(), >=, 3 );
	R_ASSERT_CMP					( input_indices.size(), >=, 3 );

	u32 coordinate_indices[2];

	u32 vertices_pair[2] = { u32(-1), u32(-1) };
	float min_distance = -100500; // Means nothing but removes comiler warning
	typename InputIndicesContainer::iterator i = input_indices.begin();
	typename InputIndicesContainer::iterator e = input_indices.end();
	for ( ; i!=e ; ++i ) {
		typename InputIndicesContainer::iterator j = i + 1;
		for ( ; j != e; ++j ) {
			float distance = length(vertices[*i] - vertices[*j]);
			if ( vertices_pair[0] == u32(-1) || distance < min_distance ) {
				min_distance = distance;
				vertices_pair[0] = *i;
				vertices_pair[1] = *j;
			}
		}
	}

	for ( typename InputIndicesContainer::iterator i = input_indices.begin(); i != e; ++i ) {
		float distance0 = length( vertices[*i] - vertices[ vertices_pair[0] ] ); 
		float distance1 = length( vertices[*i] - vertices[ vertices_pair[1] ] );
		if ( distance1 < distance0 )
			distance0 = distance1;
		
		if ( xray::math::is_relatively_similar( distance0, distance0 + min_distance, math::epsilon_5 ) ) {
			if ( error_vertices_indices )
				error_vertices_indices->push_back( *i );
			else {
				DEBUG_BREAK(); // Relatively close vertices found
				return;
			}
		}
	}

	if ( error_vertices_indices && !error_vertices_indices->empty() ) {
		return;
	}

	bool done						= false;
	xray::math::random32 random		= xray::math::random32( 0 );
	for ( u32 shuffle_count = 0; shuffle_count < 1024; ++shuffle_count ) {
		done						= true;
		output_indices.clear		( );

		buffer_vector<u32> input_indices_copy( ALLOCA( input_indices.size() * sizeof(u32) ), input_indices.size(), input_indices.begin(), input_indices.end() );
		std::random_shuffle			( input_indices_copy.begin(), input_indices_copy.end(), random );
		add_bounding_triangle		( normal, vertices, input_indices, coordinate_indices );

		m_vertices					= &*vertices.begin();

		buffer_vector<u32>::const_iterator i		= input_indices_copy.begin( );
		buffer_vector<u32>::const_iterator const e	= input_indices_copy.end( );
		for ( ; i != e; ++i ) {
			if ( !add_vertex_to_triangulation ( coordinate_indices, *i ) ) {
				done = false;
				break;
			}
		}

		remove_bounding_triangle	( vertices );

		if ( done )
			break;

	}

	R_ASSERT						( done );
}

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef DELAUNAY_TRIANGULATOR_INLINE_H_INCLUDED
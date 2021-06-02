////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "bsp_tree_utilities.h"
#include "xray\math_randoms_generator.h"
namespace xray {
namespace model_editor {
#pragma message( XRAY_TODO( "The following code duplicates code from graph_generator::remove_duplicates" ) )

typedef xray::buffer_vector<u32> indices_type;

static inline float project_vertex (float3 const& v0, float3 const& v1, float3 const& p)
{
    return (p - v0) | normalize( v1 - v0 );
}
typedef vector<math::float3> vertices_type;

struct compare_indices_by_vertices_predicate : public std::binary_function<u32, u32, bool>
{
public:
    compare_indices_by_vertices_predicate( vertices_type const& vertices, float3 const& v0, float3 const& v1) :
        m_v0(v0),
        m_v1(v1),
        m_vertices( &vertices )
    {
    }

    inline bool operator() ( u32 const left, u32 const right) const {
        if ( project_vertex ( m_v0, m_v1, (*m_vertices)[ left ] ) < project_vertex ( m_v0, m_v1, (*m_vertices)[ right ] ) )
            return true;
        return false;
    }
private:
    vertices_type const*		m_vertices;
    float3                      m_v0;
    float3                      m_v1;
}; // struct compare_indices_by_vertices_predicate


struct remove_vertices_by_unique_indices_predicate : public std::unary_function<xray::math::float3 const, bool>
{
    remove_vertices_by_unique_indices_predicate (
            vertices_type const& vertices,
            indices_type const& unique_vertices_indices
        ) :
        m_vertices_begin            ( vertices.begin() ),
        m_unique_vertices_indices    ( &unique_vertices_indices )
    {
    }

    inline bool operator() ( xray::math::float3 const& vertex ) const
    {
        u32 const index = u32(&vertex - m_vertices_begin);
        return        (*m_unique_vertices_indices)[ index ] != index;
    }

private:
    vertices_type::const_iterator    m_vertices_begin;
    indices_type const*              m_unique_vertices_indices;
}; // struct remove_vertices_by_unique_indices_predicate 

bool is_between	( math::float3 const& test_vertex, math::float3 const& vertex0, math::float3 const& vertex1 )
{
	float const v0_to_v1_sqr_len						= ( vertex1 - vertex0 ).squared_length( );
	float const v0_to_test_sqr_len						= ( test_vertex - vertex0 ).squared_length( );
	float const test_to_v1_sqr_len						= ( vertex1 - test_vertex ).squared_length( );
	const float v0_to_test_len_mul_test_to_v1_len		= math::sqrt( v0_to_test_sqr_len * test_to_v1_sqr_len );
	const float v0_to_test_len_plus_test_to_v1_len_sqr	= v0_to_test_sqr_len + 2 * v0_to_test_len_mul_test_to_v1_len + test_to_v1_sqr_len;
	return math::is_relatively_similar( v0_to_v1_sqr_len, v0_to_test_len_plus_test_to_v1_len_sqr, math::epsilon_5 );
}

void make_point_between( math::float3 const& vertex0, math::float3 const& vertex1, math::float3 & point_to_change )
{
	R_ASSERT( !math::is_similar( vertex0, vertex1, math::epsilon_7 ) );
	R_ASSERT( !math::is_similar( vertex0, point_to_change, math::epsilon_7 ) );
	R_ASSERT( !math::is_similar( vertex1, point_to_change, math::epsilon_7 ) );

	math::float3 const& vertex0_to_vertex1		=	vertex1 - vertex0;
	math::float3 const& vertex0_to_vertex1_norm =	math::float3( vertex0_to_vertex1 ).normalize_safe( math::float3( 0.0f, 0.0f, 0.0f ) );
	math::float3 const& vertex0_to_point		=	point_to_change - vertex0;
	const float signed_projection				=	vertex0_to_vertex1_norm | vertex0_to_point;
	float rel_signed_projection					=	signed_projection / vertex0_to_vertex1.length( );
	math::clamp	( rel_signed_projection, 0.0f, 1.0f );
	point_to_change								=  vertex0 + vertex0_to_vertex1 * rel_signed_projection;
	R_ASSERT( is_between( point_to_change, vertex0, vertex1 ) );
}


} // namespace model_editor
} // namespace xray

////////////////////////////////////////////////////////////////////////////
//	Created		: 27.08.2010
//	Author		: Dmitriy Iassenev
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_TESSELLATOR_H_INCLUDED
#define GRAPH_GENERATOR_TESSELLATOR_H_INCLUDED

#include "navigation_mesh_types.h"
#include "delaunay_triangulator.h"

namespace xray {
namespace ai {
namespace navigation {

//typedef debug::vector< debug:: >	triangles_type;

class graph_generator_tessellator : private boost::noncopyable {
public:
					graph_generator_tessellator( triangles_mesh_type& triangles_mesh, u32 tessellation_max_operation_id );

private:
			typedef buffer_vector< u32 > buffer_indices_type;
			typedef vector< float3 > vertices_type;

	inline	u32		get_next_untessellated_triangle( u32 triangle_id );
			u32		untessellated_triangles_count(u32 const triangle_id );
			void	collect_untessellated_triangles ( u32 const triangle_id, buffer_indices_type& result );
			bool	try_tessellate_triangles( u32 const triangle_id0, u32 const triangle_id1 );
			u32		get_similar_edge ( 
						u32 const triangle_id0, 
						u32 const triangle_id1,
						u32* edge_id1 = 0
					) ;

			bool	tessellate_coplanar_triangles	(
						u32 const triangle_id0,
						u32 const triangle_id1
					);

			bool	process_degenerated_cases(
						u32 triangle_id,
						float3 const (&segment_vertices)[2]
					);

			bool	subtract_triangle (
						u32 const (&coordinate_indices)[2],
						u32 const triangle_id0,
						u32 const triangle_id1,
						vertices_type& intersection_vertices
					);
			bool	subdivide_triangle ( 
						u32 const triangle_id, 
						float3 (&segment_vertices)[2]
					);

			bool	tessellate_non_coplanar_triangles( 
						u32 const triangle_id0, 
						u32 const triangle_id1, 
						float3 (&segment_vertices)[2]
					);

			bool add_output_triangle( 
					u32 const triangle_id, 
					float3 const& triangle_vertex0,
					float3 const& triangle_vertex1,
					float3 const& triangle_vertex2,
					bool check_null_square = true
				);
			void get_colinear_edge( 
					u32 const (&coordinate_indices)[2],
					u32 const triangle_id0, 
					u32 const triangle_id1,
					u32& colinear_edge_id0,
					u32& colinear_edge_id1
				);
			bool process_colinear_edge_tests ( 
					u32 const (&coordinate_indices)[2],
					u32 const triangle_id0, 
					u32 const triangle_id1,
					vertices_type& vertices
				);

			bool	tessellate_triangle_impl		( u32 triangle_id, buffer_indices_type& triangles );
			bool	tessellate_triangle_impl2		( u32 triangle_id, buffer_indices_type& triangles );
			void	tessellate_triangle				( u32 triangle_id, buffer_indices_type& triangles );
			void	tessellate_triangle				( u32 triangle_id, triangles_type& triangles);
			void	tessellate_passable_triangles	( );
			bool	is_edge_endpoint				( float3 vertex, u32 triangle_id, u32 edge_id );
			bool is_null_square ( 
					float3 const& a,
					float3 const& b,
					float3 const& c
				);
			void	snap_to_edge ( float3& point, float3 const& vertex0, float3 const& vertex1 );
			void	try_snap_to_closest_edge ( float3& vertex, u32 const triangle_id );

			bool	collect_intersection_vertices( u32 const triangle_id, buffer_indices_type& intersected_triangles );

	struct intermediate_constraint_edge {
		intermediate_constraint_edge( float3 const& _v0, float3 const& _v1 ) :
			v0( _v0 ),
			v1( _v1 )
		{
		}

		float3 v0;
		float3 v1;
	};
	typedef vector< intermediate_constraint_edge > intermediate_edges_type;

			void	push_intermediate_edge			( intermediate_edges_type& edges, float3 const& v0, float3 const& v1 );
			void	collect_constrained_edges		( u32 const (&coordinate_indices)[2], intermediate_edges_type& intermediate_edges, u32 const triangle_id0, u32 const triangle_id1 );
			u32		find_vertex_index				( delaunay_triangulator::vertices_type& vertices, float3 const& vertex );

	inline	bool	advance_current_operation_id	( )
	{
		return (m_tessellation_current_operation_id++ >= m_tessellation_max_operation_id);
	}

private:
	
	struct triangle_info {
		triangle_info() :
			is_processed	(false),
			next_triangle	( u32(-1) ),
			parent			( u32(-1) )
		{
		}

		bool	is_processed;
		u32		next_triangle;
		u32		parent;
	};

	struct edge_intersection {
		edge_intersection( float3 const& intersection_vertex, u32 edge_id ) :
			vertex(intersection_vertex ),
			intersector_edge_id( edge_id )
		{
		}

		float3	vertex;
		u32		intersector_edge_id;
	}; // struct edge_intersection
	typedef fixed_vector< edge_intersection, 4 >	edge_intersections_type;

	struct compare_edge_intersections_predicate {
		compare_edge_intersections_predicate( float3 const& vertex ) :
			m_vertex( vertex )
		{
		}

		inline bool operator() ( edge_intersection const left )
		{
			if ( is_similar( left.vertex, m_vertex ) )
				return true;
			return false;
		}
	private:
		float3		m_vertex;
	}; // struct compare_edge_intersections_predicate

	static	inline	bool	insert_edge ( edge_intersections_type& edge_intersections, float3 const vertex, u32 const edge_id );

	triangles_mesh_type&			m_triangles_mesh;
	debug::vector< triangle_info >	m_triangle_info;
	u32 const						m_tessellation_max_operation_id;
	u32								m_tessellation_current_operation_id;

}; // class graph_generator_tessellator
} // namespace navigation
} // namespace ai
} // namespace xray


#endif // #ifndef GRAPH_GENERATOR_TESSELLATOR_H_INCLUDED
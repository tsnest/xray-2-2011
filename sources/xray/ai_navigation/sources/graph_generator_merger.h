////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_MERGER_H_INCLUDED
#define GRAPH_GENERATOR_MERGER_H_INCLUDED

#include "navigation_mesh_types.h"
#include "constrained_delaunay_triangulator.h"
#include <xray/collision/object.h>

namespace xray {
namespace ai {
namespace navigation {

class graph_generator_merger : private boost::noncopyable {
private:

	struct triangle_connection {
		triangle_connection() :
			visited	( false )
		{
			neighbours[0] = neighbours[1] = neighbours[2] = u32(-1);
		}
		bool	visited;
		u32		neighbours[3];
	}; // struct triangle_data
	typedef debug::vector<triangle_connection> triangle_connections_type;
	typedef buffer_vector< u32 > triangle_indices_type;
	typedef debug::vector< u32 > vertex_indices_type;
	typedef buffer_vector< u32 > edges_indices_type;
	typedef buffer_vector< u32 > buffer_indices_type;
	typedef buffer_vector< bool > markers_type;

public:
		typedef buffer_vector< bool > edge_markers_type;

public:
					graph_generator_merger ( 
						triangles_mesh_type& triangles_mesh, 
						u32 const merger_max_operation_id, 
						float const edge_merge_angle_epsilon, 
						float const edge_merge_height_epsilon, 
						u32 const delaunay_max_operation_id,
						edges_type& debug_edges
					);

private:
			bool try_remove_vertex (
					u32 const (&coordinate_indices)[2],
					u32& vertex_id0,
					u32 const vertex_id1,
					edges_indices_type const& sorted_edges,
					edge_markers_type& edge_markers
				);
			bool try_merge_edge ( 
					u32 const(&coordinate_indices)[2],
					u32& vertex_index0, 
					u32& vertex_index1,
					edges_indices_type const& sorted_edges,
					edge_markers_type& edge_markers
				);
				void	add_constraint_edge( 
						u32 const index0, 
						u32 const index1, 
						bool const is_region_edge,
						xray::math::float3 const& edge_direction,
						float obstruction_height
					);
			void	mark_outside_triangles ( 
						constrained_delaunay_triangulator::indices_type const& indices, 
						markers_type& markers 
					);
			u32		find_edge_id_by_first						( u32 const vertex_id );
			u32		find_edge_id_by_second						( u32 const vertex_id, edges_indices_type const& edge_indices );
			void	fill_and_sort_edges_indices					( edges_indices_type& edges_indices );
			bool	can_remove_vertex ( 
						u32 const (&coordinate_indices)[2],
						float3 const& testee, 
						float3 const& v0, 
						float3 const& v1
					);

			void	merge_constraint_edges						( float3 const& normal );
			u32		get_constraint_index_by_vertices_indices	( u32 vertex_index0, u32 vertex_index1 );
			void	mark_constraint_edges						( u32 const triangle_id );
			void	subdivide_constraint_edges					( float3 const& normal );
			void	collect_obstructed_constraint_edges			( u32 const triangle_id );
			void	collect_constraint_edges					( u32 const triangle_id, float3 const& region_normal );
			void	triangulate_region							( u32 const triangle_id );
			bool	can_traverse								( u32 const triangle_id, float3 const& region_normal );

// Constraint subdivider
			void try_subdivide_constraint_edge (
				u32 const (&coordinate_indices)[2],
				u32 const constraint_id,
				float3 const& v0,
				float3 const& v1,
				buffer_indices_type& constraint_edge_indices
			);
			bool subdivide_constraint_edge (
				u32 const (&coordinate_indices)[2],
				u32 const constraint_id,
				float3 const& v0,
				float3 const& v1,
				buffer_indices_type& constraint_edge_indices
			);
			void subdivide_constraint_edge  (
				u32 const (&coordinate_indices)[2],
				u32 const constraint_id, 
				collision::objects_type& objects,
				markers_type& markers
			);
			void remove_duplicated_edge_vertices ( u32 const old_vertices_count, float3 const& normal );


private:
	struct constraint_edge {
		constraint_edge ( u32 const index0, u32 const index1, bool const flag, float3 const& dir, float const height ) :
			vertex_index0		( index0 ),
			vertex_index1		( index1 ),
			obstruction_height	( height ),
			edge_direction		( dir ),
			is_region_edge		( flag ),
			start_index			( u32(-1) ),
			triangle_id			( u32(-1) )
		{
		}

		constraint_edge ( ) :
			start_index			( u32(-1) ),
			triangle_id			( u32(-1) )
		{
		}
		
		constraint_edge ( u32 const index0, u32 const index1 ) :
			vertex_index0		( index0 ),
			vertex_index1		( index1 ),
			start_index			( u32(-1) ),
			triangle_id			( u32(-1) )
		{
		}

		/* Debug */
		u32					triangle_id;
		u32					start_index;
		/**/

		u32					vertex_index0;
		u32					vertex_index1;
		float				obstruction_height;
		float3				edge_direction;
		bool				is_region_edge;

	}; // struct onstraint_edges_info_type 
	typedef debug::vector< constraint_edge > constraint_edges_type;

private:
	triangles_mesh_type&									m_triangles_mesh;
	constraint_edges_type									m_constraint_edges;
	triangle_connections_type								m_triangle_connections;
	edges_type&												m_debug_edges;
	u32 const												m_merge_max_operation_id;
	float const												m_edge_merge_height_epsilon;
	float const												m_edge_merge_angle_epsilon;
	u32														m_merge_current_operation_id;
	u32	const												m_delaunay_max_operation_id;
	u32														m_current_region_color;

/////////////////////////////
// Predicates
/////////////////////////////
private:
	struct is_marked_constraint_predicate {
		is_marked_constraint_predicate (
				constraint_edges_type const& constraint_edges,
				markers_type const& markers
			) :
				m_constraint_edges	( &constraint_edges ),
				m_markers			( &markers )
		{
		}
		
		inline bool operator() ( constraint_edge& edge )
		{
			u32 index = &edge - m_constraint_edges->begin();
			if ( index < m_markers->size() )
				return (*m_markers)[ &edge - m_constraint_edges->begin() ];
			else
				return false;
		}

	private:
		constraint_edges_type const*	m_constraint_edges;
		markers_type const*				m_markers;
	}; // struct is_marked_constraint_predicate

	struct remove_edge_if_marked_predicate {
		remove_edge_if_marked_predicate ( 
				constraint_edges_type const& constraint_edges,
				graph_generator_merger::edge_markers_type const& edge_markers
			) :
			m_constraint_edges	( &constraint_edges ),
			m_edge_markers		( &edge_markers )
		{
		}

		inline bool operator() ( constraint_edge const& constraint_edge ) {
			u32 const edge_id = &constraint_edge - m_constraint_edges->begin();
			return (*m_edge_markers)[edge_id];
		}
	private:
		constraint_edges_type const*						m_constraint_edges;
		graph_generator_merger::edge_markers_type const*	m_edge_markers;
	}; // remove_edge_if_marked_predicate

	struct equal_constraint_edges_predicate {

		inline bool operator() (constraint_edge const left, constraint_edge const right)
		{
			if ( left.vertex_index0 != right.vertex_index0 )
				return false;
			return left.vertex_index1 == right.vertex_index1;
		}
	}; // struct equal_constraint_edges_predicate

	struct compare_constraint_edges_predicate {
		inline bool operator() ( 
				constraint_edge const& left, 
				constraint_edge const& right
			)
		{
			if ( left.vertex_index0 < right.vertex_index0 )
				return true;

			if ( left.vertex_index0 > right.vertex_index0 )
				return false;
			
			return left.vertex_index1 < right.vertex_index1;
		}
	}; // atruct compare_constraint_edges_predicate

	struct compare_constraint_edges_indices_by_second_predicate {
		compare_constraint_edges_indices_by_second_predicate ( constraint_edges_type const& constraint_edges ) :
			m_constraint_edges ( &constraint_edges )
		{
		}

		inline bool operator() ( u32 const left, u32 const right )
		{
			if ( (*m_constraint_edges)[left].vertex_index1 < (*m_constraint_edges)[right].vertex_index1 )
				return true;

			if ( (*m_constraint_edges)[left].vertex_index1 > (*m_constraint_edges)[right].vertex_index1 )
				return false;

			return  (*m_constraint_edges)[left].vertex_index0 < (*m_constraint_edges)[right].vertex_index0;
		}

		inline bool operator() (u32 const left, constraint_edge const& right)
		{
			if ( (*m_constraint_edges)[left].vertex_index1 < right.vertex_index1 )
				return true;

			if ( (*m_constraint_edges)[left].vertex_index1 > right.vertex_index1 )
				return false;

			return (*m_constraint_edges)[left].vertex_index0 < right.vertex_index0;
		}

		inline bool operator() ( constraint_edge const& left, u32 const right )
		{
			if ( left.vertex_index1 < (*m_constraint_edges)[right].vertex_index1 )
				return true;

			if ( left.vertex_index1 > (*m_constraint_edges)[right].vertex_index1 )
				return false;

			return left.vertex_index0 < (*m_constraint_edges)[right].vertex_index0;
		}

	private:
		constraint_edges_type const*	m_constraint_edges;
	}; // struct compare_constraint_edges_indices_by_second_predicate


	struct less_triangle_predicate {
		less_triangle_predicate ( buffer_indices_type const& indices, u32 const vertex_index0, u32 const vertex_index1, u32 const vertex_index2 ) :
			m_indices			( &indices ),
			m_vertex_index0		( vertex_index0 ),
			m_vertex_index1		( vertex_index1 ),
			m_vertex_index2		( vertex_index2 )
		{
			R_ASSERT		( vertex_index0 < 3 );
			R_ASSERT		( vertex_index1 < 3 );
			R_ASSERT		( vertex_index2 < 3 );
		}

		inline	bool		operator () ( u32 const& left, u32 const& right )
		{
			u32	const*		indices0	= m_indices->begin() + 3 * left;
			u32 const*		indices1	= m_indices->begin() + 3 * right;

			if ( indices0[ m_vertex_index0 ] < indices1[ m_vertex_index0 ] )
				return true;

			if ( indices0[ m_vertex_index0 ] > indices1[ m_vertex_index0 ] )
				return false;

			if ( indices0[ m_vertex_index1 ] < indices1[ m_vertex_index1 ] )
				return true;

			if ( indices0[ m_vertex_index1 ] > indices1[ m_vertex_index1 ] )
				return false;

			return indices0[ m_vertex_index2 ] < indices1[ m_vertex_index2 ];
		}

	private:
		buffer_indices_type const*	m_indices;
		u32							m_vertex_index0;
		u32							m_vertex_index1;
		u32							m_vertex_index2;
	}; // struct less_triangle_predicate

}; // class graph_generator_merger

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_MERGER_H_INCLUDED
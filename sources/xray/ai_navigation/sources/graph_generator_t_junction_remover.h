////////////////////////////////////////////////////////////////////////////
//	Created		: 22.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_T_JUNCTION_REMOVER_H_INCLUDED
#define GRAPH_GENERATOR_T_JUNCTION_REMOVER_H_INCLUDED

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

	class graph_generator_t_junction_remover : boost::noncopyable {
	struct edge_type {
		edge_type( u32 const id0, u32 const id1 )
		{
			indices[0] = id0;
			indices[1] = id1;
		}

		u32 indices[2];
	};
	typedef buffer_vector< edge_type > edges_type;

public:
			graph_generator_t_junction_remover(triangles_mesh_type& triangles_mesh);

private:
			void try_subdivide_triangle (
				u32 const triangle_id,
				u32 const t_junction_vertex,
				u32 const edge_id,
				edges_type& sortde_edges_by_first_index,
				edges_type& sorted_edges_by_second_index
			);
			u32 find_t_junction (
				u32 const vertex_index0,
				u32 const vertex_index1,
				edges_type& sorted_edges,
				u32 const (&sort_order)[2]
			);
			bool remove_t_junctions ( 
				u32 const triangle_id,
				edges_type& sorted_edges_by_first_index,
				edges_type& sorted_edges_by_second_index
			);

private:
	triangles_mesh_type&		m_triangles_mesh;

	// Predicates
private:
	struct less_edges_by_vertices_indices_predicate {
		inline less_edges_by_vertices_indices_predicate( u32 const id0, u32 const id1 ) :
			index0 ( id0 ),
			index1 ( id1 )
		{
		}

		inline bool operator() ( edge_type const& left, edge_type const& right ) {
			if ( left.indices[index0] < right.indices[index0] )
				return true;

			if ( left.indices[index0] > right.indices[index0] )
				return false;

			return left.indices[index1] < right.indices[index1];
		}
	private:
		u32 index0;
		u32 index1;
	}; // struct sort_edges_by_vertices_indices_predicate

}; // class graph_generator_t_junction_remover

} // namespace ai
} // namespace navigation
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_T_JUNCTION_REMOVER_H_INCLUDED
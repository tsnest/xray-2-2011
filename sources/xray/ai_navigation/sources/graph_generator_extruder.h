////////////////////////////////////////////////////////////////////////////
//	Created		: 07.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_EXTRUDER_H_INCLUDED
#define GRAPH_GENERATOR_EXTRUDER_H_INCLUDED

#include "triangles_mesh.h"
#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

class graph_generator_extruder : private boost::noncopyable {
public:
	explicit	graph_generator_extruder	( triangles_mesh_type& triangles_mesh );

private:
		void push_triangle (
			u32 const v0,
			u32 const v1,
			u32 const v2,
			u32 const edge_vertex0,
			u32 const edge_vertex1
		);
		void	extrude_triangles			( );

private:
	struct edge {
		edge( u32 const id0, u32 const id1) :
			vertex_index0( id0 ),
			vertex_index1( id1 )
		{
		}

		u32 vertex_index0;
		u32 vertex_index1;
	}; // struct edge
	typedef buffer_vector< edge > edges_type;

private:
	triangles_mesh_type&	m_triangles_mesh;

private:
	struct less_by_edge_vertices_indices_predicate {
		inline bool operator() (edge const& left, edge const& right ) 
		{
			if ( left.vertex_index0 < right.vertex_index0 )
				return true;
			if ( left.vertex_index0 > right.vertex_index0 )
				return false;
			return left.vertex_index1 < right.vertex_index1;
		}
	}; // struct less_edge_by_vertices_indices_predicate
	struct equal_by_edge_vertices_indices_predicate {
		inline bool operator() ( edge const& left, edge const& right )
		{
			return (left.vertex_index0 == right.vertex_index0) && (left.vertex_index1 == right.vertex_index1);
		}
	}; // struct equal_by_edge_vertices_indices_predicate
}; // class graph_generator_extruder

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_EXTRUDER_H_INCLUDED
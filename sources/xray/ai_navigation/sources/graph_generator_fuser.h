////////////////////////////////////////////////////////////////////////////
//	Created		: 16.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_FUSER_H_INCLUDED
#define GRAPH_GENERATOR_FUSER_H_INCLUDED

#include "navigation_mesh_types.h"

namespace xray {
namespace ai {
namespace navigation {

class graph_generator_fuser : private boost::noncopyable {
public:
	graph_generator_fuser( triangles_mesh_type& triangles_mesh, u32 const fuser_max_operation_id );

private:
	typedef debug::vector< u32 >	vertices_indices_type;
	typedef fixed_vector< u32, 3 >	edge_ids_type;
	typedef buffer_vector< u32 >	triangle_indices_type;

private:
	inline void push_triangle ( 
					u32 const parent_triangle_id,
					u32 const v0,
					u32 const v1,
					u32 const v2
				);
	inline u32	create_vertex					( float3 const& vertex );

	inline void connect_triangles( 
				u32 const triangle_id0,
				u32 const triangle_id1,
				u32 const edge_id
			);
	void subdivide_triangle( 
			u32 const triangle_id,
			u32 const edge_id,
			u32 const connection_triangle_id,
			u32 const connection_vertex_index0,
			u32 const connection_vertex_index1
		);

	u32			calculate_childs_count			( u32 triangle_id );
	u32			get_next_unprocessed_triangle	( u32 triangle_id );
	void		collect_child_triangles			( u32 triangle_id, triangle_indices_type& triangle_indices );

	bool		try_fuse_edge (
					u32 const triangle_id0, 
					u32 const triangle_id1, 
					u32 const edge_id // Edge from triangle_id0
				);
	void		fuse_triangle_edges ( 
					u32 const triangle_id, 
					triangles_type& intersected_triangles
				);
	void		get_edges_for_fuse (
					u32 const triangle_id, 
					edge_ids_type& edge_ids 
				);
private:
	struct triangle_info {
		triangle_info() : 
			next_triangle	( u32(-1) )
		{
		}

		u32 next_triangle;
	}; // struct triangle_info
	typedef debug::vector< triangle_info > triangles_info_type;

	triangles_info_type			m_triangles_info;
	triangles_mesh_type&		m_triangles_mesh;
	u32							m_fuser_current_operation_id;
	u32	const					m_fuser_max_operation_id;
}; // class graph_generator_fuser

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_FUSER_H_INCLUDED
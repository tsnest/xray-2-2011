////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef CONSTRAINED_DELAUNAY_TRIANGULATOR_H_INCLUDED
#define CONSTRAINED_DELAUNAY_TRIANGULATOR_H_INCLUDED

#include "navigation_mesh_types.h"
#include "delaunay_triangulator.h"
#include "delaunay_debug_functions.h"

namespace xray {
namespace ai {
namespace navigation {

class XRAY_AI_NAVIGATION_API constrained_delaunay_triangulator : private core::noncopyable {
public:

	struct constraint_edge {
		constraint_edge () {}
		constraint_edge ( u32 const index0, u32 const index1 ) :
			vertex_index0 (index0),
			vertex_index1 (index1)
		{
		}
		#if defined(_MSC_VER)
		#	pragma warning(push)
		#	pragma warning(disable:4201)
		#endif // #if defined(_MSC_VER)
			union
			{
				struct {
				u32		vertex_index0;
				u32		vertex_index1;
				};
				u32		indices[2];
			};
		#if defined(_MSC_VER)
		#	pragma warning(pop)
		#endif // #if defined(_MSC_VER)

		bool operator == ( constraint_edge const& rhs ) const
		{
			return vertex_index0 == rhs.vertex_index0 && vertex_index1 == rhs.vertex_index1;
		}
	}; // struct constraint_edge
	typedef debug::vector< constraint_edge >	constraint_edges_type;
	typedef delaunay_triangulator::indices_type indices_type;
	typedef delaunay_triangulator::input_indices_type input_indices_type;

private:
	struct intersection_info {
		intersection_info() { }
		intersection_info( u32 id, float3 v ) :
			edge_id ( id ),
			vertex( v )
		{
		}

		u32 edge_id;
		float3 vertex;
	};

	struct triangle_info {
		u32 triangle_id;
		intersection_info intersection;
	};
	
	typedef debug::vector< u32 > polygon_indices_type;
	typedef debug::vector<triangle_info> triangles_info_type;
	typedef debug::vector<intersection_info> intersections_type;

public:
		template< typename VerticesContainer, typename InputIndicesContainer, typename ConstraintContainer >
		constrained_delaunay_triangulator ( 
			VerticesContainer& vertices,
			InputIndicesContainer& input_indices,
			indices_type& output_indices,
			float3 const& normal,
			ConstraintContainer const& constraint_edges,
			delaunay_triangulator::error_vertices_indices_type* error_vertices_indices = 0,
			u32 const max_delaunay_operation_id = u32(-1),
			bool keep_bounding_triangle = false
		);

		template< typename VerticesContainer, typename ConstraintContainer >
		constrained_delaunay_triangulator ( 
			VerticesContainer& vertices,
			indices_type& output_indices,
			float3 const& normal,
			ConstraintContainer const& constraint_edges,
			delaunay_triangulator::error_vertices_indices_type* error_vertices_indices = 0,
			u32 const max_delaunay_operation_id = u32(-1),
			bool keep_bounding_triangle = false
		);
private:
		void				triangulate_pseudo_polygon ( 
				u32 const (&coordinate_indices)[2],
				polygon_indices_type::iterator const start,
				polygon_indices_type::iterator const end,
				u32 const edge_vertex_index0,
				u32 const edge_vertex_index1 
			);
			bool			is_triangle_vertex(
				float3 const& testee,
				float3 const& v0,
				float3 const& v1,
				float3 const& v2
			);
			bool			remove_triangles ( 
				u32 const (&coordinate_indices)[2],
				polygon_indices_type& right_polygon_indices, 
				polygon_indices_type& left_polygon_indices, 
				u32 const segment_vertex_index0, 
				u32 const segment_vertex_index1
			);
			void add_pseudo_polygon_index(
				u32 const (&coordinate_indices)[2],
				u32 const vertex_index,
				polygon_indices_type& right_polygon_indices, 
				polygon_indices_type& left_polygon_indices,
				float3 const& v0,
				float3 const& v1
			);
	inline	bool			is_correct_order		(
			float3 const& v0,
			float3 const& v1,
			float3 const& v2
		);
	
private:
	float3									m_normal;
	float3	const*							m_vertices;
	indices_type&							m_indices;

private:
	struct less_by_intersection_distance {
		less_by_intersection_distance( float3 vertex ) :
			m_vertex( vertex )
		{
		}
			
		bool operator() ( triangle_info const& left, triangle_info const& right ) 
		{
			float const squared_length0 = squared_length( left.intersection.vertex - m_vertex );
			float const squared_length1 = squared_length( right.intersection.vertex - m_vertex );
			if ( squared_length0 < squared_length1 )
				return true;
			return false;
		}

	private:
		float3 m_vertex;

	};
}; // class constraint_delaunay_triangulator

} // namespace navigation
} // namespace ai
} // namespace xray

#include "constrained_delaunay_triangulator_inline.h"

#endif // #ifndef CONSTRAINED_DELAUNAY_TRIANGULATOR_H_INCLUDED
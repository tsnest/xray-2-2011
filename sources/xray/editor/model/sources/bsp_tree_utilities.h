////////////////////////////////////////////////////////////////////////////
//	Created		: 27.07.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BSP_TREE_UTILITIES_H_INCLUDED
#define BSP_TREE_UTILITIES_H_INCLUDED
#include "bsp_tree_triangle.h"

namespace xray {
namespace model_editor {
//void remove_duplicate_vertices_from_indexed_geometry( vector<math::float3>& vertices, vector<u32>& indices );
bool is_between	( math::float3 const& test_vertex, math::float3 const& vertex0,	math::float3 const& vertex1 );
void make_point_between( math::float3 const& vertex0, math::float3 const& vertex1, math::float3 & point_to_change );
inline float triangle_square(  math::float3 const& v0, math::float3 const& v1, math::float3 const& v2  )
{
	return ( ( v1 - v0 ) ^ ( v2 - v0 ) ).length() * 0.5f;
}


} // namespace model_editor
} // namespace xray

#endif // #ifndef BSP_TREE_UTILITIES_H_INCLUDED
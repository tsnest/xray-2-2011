////////////////////////////////////////////////////////////////////////////
//	Created		: 04.08.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef BSP_TYPEDEFS_H_INCLUDED
#define BSP_TYPEDEFS_H_INCLUDED
#include <xray\ai_navigation\sources\constrained_delaunay_triangulator.h>

namespace xray {
namespace model_editor {

typedef xray::ai::navigation::constrained_delaunay_triangulator::constraint_edge edge;
typedef xray::ai::navigation::constrained_delaunay_triangulator::constraint_edges_type edges_type;
typedef buffer_vector<edge> edges_buffer_type;
typedef	xray::ai::navigation::constrained_delaunay_triangulator::input_indices_type input_indices_type;
typedef	xray::ai::navigation::constrained_delaunay_triangulator::indices_type indices_buffer_type;
typedef u32 coord_indices_pair [2];
} // namespace model_editor
} // namespace xray

#endif // #ifndef BSP_TYPEDEFS_H_INCLUDED
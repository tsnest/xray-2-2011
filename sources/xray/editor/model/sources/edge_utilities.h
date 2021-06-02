////////////////////////////////////////////////////////////////////////////
//	Created		: 05.10.2011
//	Author		: Alexander Stupakov
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef EDGE_UTILITIES_H_INCLUDED
#define EDGE_UTILITIES_H_INCLUDED
#include "bsp_typedefs.h"

namespace xray {
namespace model_editor {

struct edge_equals : public std::binary_function<edge, edge, bool>
{
	bool operator() ( edge const& first, edge const& second ) const
	{
		u32 const e_0_min = std::min( first.vertex_index0, first.vertex_index1 );
		u32 const e_0_max = std::max( first.vertex_index0, first.vertex_index1 );
		u32 const e_1_min = std::min( second.vertex_index0, second.vertex_index1 );
		u32 const e_1_max = std::max( second.vertex_index0, second.vertex_index1 );
		return e_0_min == e_1_min && e_0_max == e_1_max;
	}
};

struct edge_less : public std::binary_function<edge, edge, bool>
{
	bool operator() ( edge const& first, edge const& second ) const
	{
		u32 const e_0_min = std::min( first.vertex_index0, first.vertex_index1 );
		u32 const e_0_max = std::max( first.vertex_index0, first.vertex_index1 );
		u32 const e_1_min = std::min( second.vertex_index0, second.vertex_index1 );
		u32 const e_1_max = std::max( second.vertex_index0, second.vertex_index1 );
		return e_0_min < e_1_min || e_0_min == e_1_min && e_0_max < e_1_max;
	}
};

struct edge_strictly_less : public std::binary_function<edge, edge, bool>
{
	bool operator() ( edge const& first, edge const& second ) const
	{
		return first.vertex_index0 < second.vertex_index0 || 
			first.vertex_index0 == second.vertex_index0 && first.vertex_index1 < second.vertex_index1;
	}
};

struct edge_less_by_index : public std::binary_function<edge, edge, bool>
{
	edge_less_by_index( u32 index ):
m_index( index )
{
}
bool operator() ( edge const& first, edge const& second ) const
{
	return first.indices[ m_index ] < second.indices[ m_index ];
}
private:
	u32 m_index;
};

typedef associative_vector<edge, u32, debug::vector, edge_less > edge_to_u32_type;

} // namespace model_editor
} // namespace xray

#endif // #ifndef EDGE_UTILITIES_H_INCLUDED
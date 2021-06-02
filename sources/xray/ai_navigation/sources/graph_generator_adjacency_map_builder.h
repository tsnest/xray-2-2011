////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPH_GENERATOR_ADJACENCY_MAP_BUILDER_H_INCLUDED
#define GRAPH_GENERATOR_ADJACENCY_MAP_BUILDER_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

class graph_generator_adjacency_map_builder {
public:
	typedef buffer_vector< u32 > buffer_indices_type;

public:
			graph_Generator_adjacency_map_builder	( 
				buffer_indices_type const& triangle_indices,
				triangle_connections_type& triangle_connections
			);

private:
	
}; // class graph_generator_adjacency_map_builder

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef GRAPH_GENERATOR_ADJACENCY_MAP_BUILDER_H_INCLUDED
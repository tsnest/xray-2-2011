////////////////////////////////////////////////////////////////////////////
//	Created		: 07.10.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef DELAUNAY_DEBUG_FUNCTIONS_H_INCLUDED
#define DELAUNAY_DEBUG_FUNCTIONS_H_INCLUDED

namespace xray {
namespace ai {
namespace navigation {

template<typename VerticesContainer, typename InputIndicesContainer, typename ConstraintContainer>
void dump_constrained_triangulation_input (
		VerticesContainer& vertices, 
		InputIndicesContainer const& indices,
		ConstraintContainer& edges,
		math::float3 const& normal 
	 )
{
	configs::lua_config_ptr config			= configs::create_lua_config();
	configs::lua_config_value vertices_cfg	= (*config)[ "vertices" ];
	u32 new_index = 0;
	typedef associative_vector<u32, u32, vector> old_to_new_index_type;
	old_to_new_index_type old_to_new_index;
	typename InputIndicesContainer::const_iterator const indices_end = indices.end();
	for ( typename InputIndicesContainer::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
	{
		math::float3 const& v = vertices[ *index_it ];
		vertices_cfg[ new_index ] = v;
		old_to_new_index.insert( std::make_pair( *index_it, new_index++ ) );
	}

	u32 edge_index = 0;
	configs::lua_config_value edges_cfg	= (*config)[ "edges" ];
	typename ConstraintContainer::const_iterator const edges_end = edges.end();
	for ( typename ConstraintContainer::const_iterator edge_it = edges.begin(); edge_it != edges_end; ++edge_it )
	{
		R_ASSERT( old_to_new_index.count( edge_it->vertex_index0 ) );
		R_ASSERT( old_to_new_index.count( edge_it->vertex_index1 ) );
		configs::lua_config_value current_edge_cfg	= edges_cfg[ edge_index++ ];
		current_edge_cfg[ 0 ] = old_to_new_index[ edge_it->vertex_index0 ];
		current_edge_cfg[ 1 ] = old_to_new_index[ edge_it->vertex_index1 ];
	}

	(*config)[ "normal" ] = normal;
	//config->save_as( "D:/triangulation_input.lua", xray::configs::target_sources );
}

template<typename VerticesContainer, typename ConstraintContainer>
void dump_constrained_triangulation_input (
		VerticesContainer& vertices, 
		ConstraintContainer& edges,
		math::float3 const& normal 
	 )
{
	configs::lua_config_ptr config			= configs::create_lua_config();
	configs::lua_config_value vertices_cfg	= (*config)[ "vertices" ];
	u32 new_index = 0;
	InputIndicesContainer::const_iterator const indices_end = indices.end();
	for ( InputIndicesContainer::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
	{
		math::float3 const& v = vertices[ *index_it ];
		vertices_cfg[ new_index ] = v;
	}

	u32 edge_index = 0;
	configs::lua_config_value edges_cfg	= (*config)[ "edges" ];
	ConstraintContainer::const_iterator const edges_end = edges.end();
	for ( ConstraintContainer::const_iterator edge_it = edges.begin(); edge_it != edges_end; ++edge_it )
	{
		configs::lua_config_value current_edge_cfg	= edges_cfg[ edge_index++ ];
		current_edge_cfg[ 0 ] = edge_it->vertex_index0;
		current_edge_cfg[ 1 ] = edge_it->vertex_index1;
	}

	(*config)[ "normal" ] = normal;
	//config->save_as( "D:/triangulation_input.lua" );
}

template<typename VerticesContainer, typename InputIndicesContainer>
void dump_triangulation_input (
		VerticesContainer& vertices, 
		InputIndicesContainer const& indices,
		math::float3 const& normal 
	 )
{
	configs::lua_config_ptr config			= configs::create_lua_config();
	configs::lua_config_value vertices_cfg	= (*config)[ "vertices" ];
	u32 new_index = 0;
	InputIndicesContainer::const_iterator const indices_end = indices.end();
	for ( InputIndicesContainer::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
	{
		++new_index;
		vertices_cfg[ new_index ] = vertices[ *index_it ];
	}

	(*config)[ "normal" ] = normal;
	//config->save_as( "D:/triangulation_input.lua" );
}

template<typename VerticesContainer>
void dump_triangulation_input (
		VerticesContainer& vertices, 
		math::float3 const& normal 
	 )
{
	configs::lua_config_ptr config			= configs::create_lua_config();
	configs::lua_config_value vertices_cfg	= (*config)[ "vertices" ];
	u32 new_index = 0;
	InputIndicesContainer::const_iterator const indices_end = indices.end();
	for ( InputIndicesContainer::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
	{
		vertices_cfg[ new_index ] = *index_it;
	}

	(*config)[ "normal" ] = normal;
	//config->save_as( "D:/triangulation_input.lua" );
}


template< typename VerticesContainer, typename InputIndicesContainer, typename EdgesContainer>
bool is_correct_triangulation_input_data( 
		math::float3 const& normal, 
		VerticesContainer const& vertices, 
		InputIndicesContainer const& indices, 
		EdgesContainer const& edges 
	)
{
	InputIndicesContainer  sorted_indices( indices );
	std::sort( sorted_indices.begin(), sorted_indices.end() );
	u32 coordinate_indices[2];
	xray::ai::navigation::fill_coordinate_indices( normal, coordinate_indices );
	typename EdgesContainer::const_iterator const edges_end				= edges.end();
	typename InputIndicesContainer::const_iterator const indices_end	= indices.end();
	for ( typename EdgesContainer::const_iterator it = edges.begin(); it != edges_end; ++it )
	{
		if ( !std::binary_search( sorted_indices.begin(), sorted_indices.end(), it->vertex_index0 ) )
			return false;
		if ( !std::binary_search( sorted_indices.begin(), sorted_indices.end(), it->vertex_index1 ) )
			return false;
		
		for ( typename InputIndicesContainer::const_iterator index_it = indices.begin(); index_it != indices_end; ++index_it )
		{
			if ( it->vertex_index0 == *index_it || it->vertex_index1 == *index_it )
				continue;
			if ( is_on_segment( coordinate_indices, vertices[ *index_it ], vertices[ it->vertex_index0 ], vertices[ it->vertex_index1 ], math::epsilon_7 ) )
				return false;
		}

		for ( typename EdgesContainer::const_iterator inner_it = edges.begin(); inner_it != edges_end; ++inner_it )
		{
			if ( inner_it == it )
				continue;
			if ( inner_it->vertex_index0 == it->vertex_index0 || 
				 inner_it->vertex_index0 == it->vertex_index1 || 
				 inner_it->vertex_index1 == it->vertex_index0 || 
				 inner_it->vertex_index1 == it->vertex_index1 )
				 continue;
			math::float3 result0, result1;
			bool on_the_same_line = false;
			if ( xray::ai::navigation::segment_intersects_segment( 
				 coordinate_indices,
				 vertices[ inner_it->vertex_index0 ],
				 vertices[ inner_it->vertex_index1 ],
				 vertices[ it->vertex_index0 ],
				 vertices[ it->vertex_index1 ],
				 result0, result1, on_the_same_line	) )
				 return false;
		}
	}
	for ( typename InputIndicesContainer::const_iterator it = indices.begin(); it != indices_end; ++it )
	{
		for ( typename InputIndicesContainer::const_iterator inner_it = it + 1; inner_it != indices_end; ++inner_it )
			if ( is_similar( coordinate_indices, vertices[ *it ], vertices[ *inner_it ], math::epsilon_5 ) )
				return false;

	}
	return true;
}

} // namespace navigation
} // namespace ai
} // namespace xray

#endif // #ifndef DELAUNAY_DEBUG_FUNCTIONS_H_INCLUDED
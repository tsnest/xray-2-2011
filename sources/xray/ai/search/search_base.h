////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_SEARCH_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_SEARCH_BASE_H_INCLUDED

#include <xray/ai/search/world_state.h>
#include <xray/ai/search/vertex_allocator_fixed_count.h>
#include <xray/ai/search/vertex_manager_fixed_count_hash.h>
#include <xray/ai/search/priority_queue_binary_heap.h>
#include <xray/ai/search/graph_wrapper_planner_base.h>
#include <xray/ai/search/path_constructor_planner_base.h>
#include <xray/ai/search/a_star.h>
//#include <xray/ai/search/dijkstra.h>

namespace xray {
namespace ai {
namespace planning {

class search_base
{
public:
	typedef u32												distance_type;
	typedef world_state										vertex_id_type;
	typedef operator_id_type								edge_id_type;
	typedef vertex_allocator::fixed_count					vertex_allocator_type;
	typedef vertex_manager::fixed_count_hash				vertex_manager_type;
	typedef priority_queue::binary_heap						priority_queue_type;
	typedef graph_wrapper::propositional_planner_base		graph_wrapper_base_type;
	typedef path_constructor::propositional_planner_base	path_constructor_base_type;
	typedef a_star											algorithm_type;
	//typedef dijkstra										algorithm_type;
	typedef vector< search_base::edge_id_type >				path_type;

	struct vertex_type : 
		public algorithm_type::vertex_helper< distance_type >::vertex_impl,
		public graph_wrapper_base_type::vertex_impl,
		public path_constructor_base_type::vertex_impl< vertex_type >,
		public priority_queue_type::vertex_impl,
		public vertex_manager_type::vertex_helper< vertex_id_type >::vertex_impl,
		public vertex_allocator_type::vertex_impl
	{};

	struct look_up_cell_type :
		public algorithm_type::look_up_cell_impl,
		public graph_wrapper_base_type::look_up_cell_impl,
		public path_constructor_base_type::look_up_cell_impl,
		public priority_queue_type::look_up_cell_impl,
		public vertex_manager_type::look_up_cell_helper< vertex_type >::look_up_cell_impl< look_up_cell_type >,
		public vertex_allocator_type::look_up_cell_impl
	{};

	struct vertex_allocator_impl_type : public vertex_allocator_type::impl< vertex_type >
	{
		typedef vertex_allocator_type::impl< vertex_type >	super;
		inline vertex_allocator_impl_type	( u32 const max_vertex_count ) :
			super							( max_vertex_count )
		{
		}
	};

	struct vertex_manager_impl_type : public 
		vertex_manager_type::impl<
			vertex_allocator_impl_type,
			look_up_cell_type
		>
	{
		typedef vertex_manager_type::impl< vertex_allocator_impl_type, look_up_cell_type >	super;
		inline vertex_manager_impl_type		(
			vertex_allocator_impl_type& allocator,
			u32 const hash_size,
			u32 const fix_size
		) : super							( allocator, hash_size, fix_size )
		{
		}
	};

	struct priority_queue_impl_type : public priority_queue_type::impl< vertex_manager_impl_type >
	{
		typedef priority_queue_type::impl< vertex_manager_impl_type >	super;
		inline priority_queue_impl_type	( vertex_manager_type& vertex_manager, u32 const vertex_count ) :
			super						( vertex_manager, vertex_count )
		{
		}
	};

public:
								search_base			( );
	static	u32					max_vertex_count	( );

private:
	vertex_allocator_impl_type	m_vertex_allocator;
	vertex_manager_impl_type	m_vertex_manager;

public:
	priority_queue_impl_type	m_priority_queue;
}; // class search_base

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_SEARCH_BASE_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_H_INCLUDED
#define XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_H_INCLUDED

namespace xray {
namespace ai {
namespace priority_queue {

struct binary_heap
{
	struct vertex_impl			{};
	struct look_up_cell_impl	{};

	template < typename vertex_manager >
	class impl : private boost::noncopyable
	{
	public:
		typedef vertex_manager								vertex_manager_type;
		typedef typename vertex_manager_type::vertex_type	vertex_type;
		typedef typename vertex_type::vertex_id_type		vertex_id_type;
		typedef typename vertex_type::distance_type			distance_type;

	private:
		struct predicate
		{
			inline bool operator()	( vertex_type* node1, vertex_type* node2 )
			{
				return				node1->f() > node2->f();
			}
		};

	public:
		inline					impl					( vertex_manager_type& , u32 const vertex_count );
		inline					~impl					( );
		
		inline void				on_before_search		( );
		inline void				on_after_search			( bool const success ) const;
		
		inline bool				opened_list_empty		( ) const;
		inline void				add_to_opened_list		( vertex_type& vertex );
		inline void				decrease_opened			( vertex_type& vertex, distance_type const& value );
		inline void				move_best_to_closed		( );
		inline vertex_type&		best					( ) const;

	public:
		inline bool				opened					( vertex_type const& vertex ) const;
		inline bool				visited					( vertex_id_type const& vertex_id ) const;
		inline vertex_type&		new_vertex				( vertex_id_type const& vertex_id ) const;
		inline vertex_type&		vertex					( vertex_id_type const& vertex_id ) const;
		inline u32				visited_vertex_count	( ) const;

	private:
		vertex_manager_type&	m_manager;
		vertex_type**			m_heap;
		vertex_type**			m_heap_head;
		vertex_type**			m_heap_tail;
	};
}; // struct binary_heap

} // namespace priority_queue
} // namespace ai
} // namespace xray

#include <xray/ai/search/priority_queue_binary_heap_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PRIORITY_QUEUE_BINARY_HEAP_H_INCLUDED
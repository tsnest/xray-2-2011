////////////////////////////////////////////////////////////////////////////
//	Created		: 12.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_VERTEX_ALLOCATOR_FIXED_COUNT_H_INCLUDED
#define XRAY_AI_SEARCH_VERTEX_ALLOCATOR_FIXED_COUNT_H_INCLUDED

namespace xray {
namespace ai {
namespace vertex_allocator {

struct fixed_count
{
	struct vertex_impl			{};
	struct look_up_cell_impl	{};

	template < typename VertexType >
	class impl
	{
	public:
		typedef	VertexType	vertex_type;

	public:
		inline				impl					( u32 const max_vertex_count );
		inline				~impl					( );

		inline void			on_before_search		( );
		inline void			on_after_search			( bool const success );
		inline vertex_type&	new_vertex				( );

	public:
		inline u32			visited_vertex_count	( ) const;

	private:
		vertex_type*		m_vertices;
		vertex_type*		m_vertices_end;
		vertex_type*		m_vertex_current;
	};

}; // struct fixed_count

} // namespace ai
} // namespace xray
} // namespace vertex_allocator

#include <xray/ai/search/vertex_allocator_fixed_count_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_VERTEX_ALLOCATOR_FIXED_COUNT_H_INCLUDED
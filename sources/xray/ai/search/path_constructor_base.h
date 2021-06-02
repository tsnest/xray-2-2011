////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_constructor {

struct base
{
	template < typename final_type >
	struct vertex_impl
	{
		final_type*					m_back;
		inline final_type*&			back	( )			{ return m_back; }
		inline final_type const*	back	( )	const	{ return m_back; }
	};

	struct look_up_cell_impl{};

	template < typename vertex_type >
	class impl
	{
	public:
		inline void			on_before_search	( );
		inline void			on_after_search		( bool const success ) const;

		inline void			assign_parent		( vertex_type& neighbour, vertex_type* parent );
		template < typename T >
		inline void			assign_parent		( vertex_type& neighbour, vertex_type* parent, T const& );
		inline void			update_successors	( vertex_type& neighbour );

	protected:
		inline static u32	length				( vertex_type const* vertex );
	};
}; // struct base

} // namespace path_constructor
} // namespace ai
} // namespace xray

#include <xray/ai/search/path_constructor_base_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_BASE_H_INCLUDED
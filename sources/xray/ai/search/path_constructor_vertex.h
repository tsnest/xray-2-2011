////////////////////////////////////////////////////////////////////////////
//	Created		: 08.09.2011
//	Author		: Sergey Lozinski
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_H_INCLUDED

#include <xray/ai/search/path_constructor_base.h>

namespace xray {
namespace ai {
namespace path_constructor {

struct vertex : public base {
	typedef base		super;

	template < typename VertexType, typename PathType >
	class impl : public super::impl< VertexType > {
	private:
		typedef super::impl< VertexType >			super;

	public:
		typedef typename VertexType::vertex_id_type	vertex_id_type;
		typedef PathType							path_type;

	public:
		inline				impl				( PathType* path = 0 );
		inline void			on_before_search	( );
		inline void			construct_path		( VertexType const& best );
		inline PathType&	path				( );
	private:
		PathType*			m_path;
	};

}; // struct vertex

} // namespace path_constructor
} // namespace ai
} // namespace xray

#include <xray/ai/search/path_constructor_vertex_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_VERTEX_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_H_INCLUDED

#include <xray/ai/search/path_constructor_base.h>

namespace xray {
namespace ai {
namespace path_constructor {

struct edge : public base
{
	typedef base		super;

	template < typename EdgeIdType >
	struct helper
	{
		template < typename final_type >
		struct vertex_impl : public super::vertex_impl< final_type >
		{
			typedef EdgeIdType		edge_id_type;
			edge_id_type			m_edge_id;
			inline edge_id_type&	edge_id		( ) { return m_edge_id; }
		};
	};

	template < typename VertexType, typename PathType >
	class impl : public super::impl< VertexType >
	{
	private:
		typedef super::impl< VertexType >			super;

	public:
		typedef typename VertexType::edge_id_type	edge_id_type;
		typedef PathType							path_type;

	public:
		using super::assign_parent;

	public:
		inline				impl					( path_type* path = 0 );
		inline void			assign_parent			( VertexType& neighbour, VertexType* parent, edge_id_type const& edge_id );
		inline void			construct_path			( VertexType& best );

	protected:
		PathType*			m_path;
	};
}; // struct edge

} // namespace path_constructor
} // namespace ai
} // namespace xray

#include <xray/ai/search/path_constructor_edge_inline.h>

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_EDGE_H_INCLUDED
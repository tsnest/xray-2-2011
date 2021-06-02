////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BACKWARD_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BACKWARD_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace path_constructor {

#define TEMPLATE_SPECIALIZATION	template < typename VertexType, typename PathType >
#define	constructor				path_constructor::planner_backward::impl< VertexType, PathType >

TEMPLATE_SPECIALIZATION
inline constructor::impl		( PathType* path ) :
	super						( path  )
{
}

TEMPLATE_SPECIALIZATION
inline void constructor::construct_path	( VertexType& best )
{
	if ( !super::m_path )
		return;

	super::m_path->resize				( length( &best ) - 1 );

	VertexType* i						= &best;
	typename PathType::iterator iter		= super::m_path->begin();
	typename PathType::iterator iter_end	= super::m_path->end();
	for ( ; iter != iter_end; ++iter )
	{
		*iter							= i->edge_id();
		i								= i->back();
	}
}

#undef TEMPLATE_SPECIALIZATION
#undef constructor

} // namespace path_constructor
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PATH_CONSTRUCTOR_PLANNER_BACKWARD_INLINE_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline operator_impl::operator_impl	( pcstr id, operator_weight_type const cost ) :
	m_id							( id ),
	m_cost							( cost ),
	m_object						( 0 ),
	m_first_execute					( false )
{
}

inline pcstr operator_impl::id		( ) const
{
	return m_id.c_str				( );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_INLINE_H_INCLUDED
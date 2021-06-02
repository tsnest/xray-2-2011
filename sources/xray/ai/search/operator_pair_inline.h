////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_PAIR_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_PAIR_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

#define PAIR planning::operator_pair

inline PAIR::operator_pair	( operator_id_type const& id, operator_ptr_type _operator ) :
	m_id					( id ),
	m_operator				( _operator )
{
}

inline bool PAIR::operator<	( operator_id_type const& operator_id ) const
{
	return					m_id < operator_id;
}

inline PAIR::operator_ptr_type PAIR::get_operator	( ) const
{
	return					m_operator;
}

inline PAIR::operator_id_type const& PAIR::id		( ) const
{
	return					m_id;
}

inline void PAIR::clear_operator					( )
{
	m_operator				= 0;
}

#undef PAIR

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_PAIR_INLINE_H_INCLUDED
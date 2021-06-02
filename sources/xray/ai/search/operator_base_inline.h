////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_OPERATOR_BASE_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_OPERATOR_BASE_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

#define WORLD_OPERATOR planning::operator_base

inline WORLD_OPERATOR::operator_base	( )
{
	init					( );
}

inline void WORLD_OPERATOR::init		( )
{
	m_preconditions_hash	= preconditions().hash_value();
	m_effects_hash			= effects().hash_value();
	m_min_weight			= 0;
}

inline WORLD_OPERATOR::state_type& WORLD_OPERATOR::preconditions( )
{
	return					m_preconditions;
}

inline WORLD_OPERATOR::state_type& WORLD_OPERATOR::effects		( )
{
	return					m_effects;
}

#undef WORLD_OPERATOR

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_OPERATOR_BASE_INLINE_H_INCLUDED
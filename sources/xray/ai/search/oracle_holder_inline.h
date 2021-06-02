////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_ORACLE_HOLDER_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_ORACLE_HOLDER_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

#define HOLDER	planning::oracle_holder

inline HOLDER::oracle_holder( propositional_planner& total_order_planner ) :
	m_planner				( total_order_planner )
{
}

inline HOLDER::objects_type const& HOLDER::objects	( ) const
{
	return					m_objects;
}

inline void HOLDER::remove	( oracle_id_type const& oracle_id )
{
	remove_impl				( oracle_id, true );
}

#undef HOLDER

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_ORACLE_HOLDER_INLINE_H_INCLUDED
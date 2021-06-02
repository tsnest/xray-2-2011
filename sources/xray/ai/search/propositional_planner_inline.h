////////////////////////////////////////////////////////////////////////////
//	Created		: 13.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_INLINE_H_INCLUDED
#define XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline propositional_planner::state_type const& propositional_planner::current	( ) const
{
	return				m_current_state;
}

inline propositional_planner* propositional_planner::return_this				( )
{
	return				this;
}

inline propositional_planner::target_state_type const& propositional_planner::target	( ) const
{
	return				m_target_state;
}

inline u32 propositional_planner::get_offsets_count	( ) const
{
	return				m_target_state_offsets.size();
}

inline propositional_planner::state_type propositional_planner::target	( u32 const offset ) const
{
	u32 const offsets_count					= m_target_state_offsets.size();

	properties_const_iterator it_target		= ( offset == 0 || offset == u32(-1) ) ?
												m_target_state.properties().begin() :
												m_target_state.properties().begin() + m_target_state_offsets[offset - 1];
	properties_const_iterator it_target_end	= ( offset == u32(-1) || offset == offsets_count ) ?
												m_target_state.properties().end() :
												m_target_state.properties().begin() + m_target_state_offsets[offset];
	
	vertex_id_type								result;									
	for ( ; it_target != it_target_end; ++it_target )
		result.add								( *it_target );
	
	return										result;
}

inline void propositional_planner::target	( propositional_planner::target_state_type const& target )
{
	m_actual			= m_actual && ( m_target_state == target );
	m_target_state		= target;
}

inline bool propositional_planner::failed	( ) const
{
	return				m_failed;
}

inline pcstr propositional_planner::id		( ) const
{
	return				m_current_id.c_str();
}

inline void propositional_planner::forward_search	( bool const forward_search )
{
	m_forward_search	= forward_search;
}

inline bool propositional_planner::forward_search	( ) const
{
	return				m_forward_search;
}

inline void propositional_planner::add_target_offset( offset_type const offset )
{
	m_target_state_offsets.push_back				( offset );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef XRAY_AI_SEARCH_PROPOSITIONAL_PLANNER_INLINE_H_INCLUDED
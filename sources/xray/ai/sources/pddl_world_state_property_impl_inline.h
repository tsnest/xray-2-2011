////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED
#define PDDL_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline pddl_world_state_property_impl::pddl_world_state_property_impl	(
		pddl_predicate const* required_predicate,
		bool result
	) :
	m_predicate			( required_predicate ),
	m_result			( result )
{
}

inline u32 pddl_world_state_property_impl::get_index	( u32 const index ) const
{
	R_ASSERT			( index < m_indices.size() );
	return				m_indices[index];
}

inline void pddl_world_state_property_impl::set_index	( u32 const index, u32 const index_value )
{
	R_ASSERT			( index < m_indices.size() );
	m_indices[index]	= index_value;
}

inline void pddl_world_state_property_impl::add_index	( u32 const index )
{
	m_indices.push_back( index );
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef PDDL_WORLD_STATE_PROPERTY_INLINE_H_INCLUDED
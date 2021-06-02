////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef PDDL_WORLD_STATE_PROPERTY_IMPL_H_INCLUDED
#define PDDL_WORLD_STATE_PROPERTY_IMPL_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

class pddl_predicate;

class pddl_world_state_property_impl
{
public:
	inline		pddl_world_state_property_impl	( pddl_predicate const* required_predicate, bool result );

	inline	u32		get_indices_count			( ) const { return m_indices.size(); }
	inline	pddl_predicate const* get_predicate	( ) const { return m_predicate; }
	inline	u32		get_index					( u32 const index ) const;
	inline	void	set_index					( u32 const index, u32 const index_value );
	inline	void	add_index					( u32 const instance );
	inline	bool	get_result					( ) const { return m_result; }
	
public:
	typedef fixed_vector< u32, 4 >				indices_type;

private:	
	// placeholders for general actions,
	// real object indices for specified actions
	indices_type			m_indices;
	
	pddl_predicate const*	m_predicate;
	bool 					m_result;
}; // class pddl_world_state_property_impl

typedef vector< pddl_world_state_property_impl > world_state_type;
typedef fixed_vector< u32, 16 >					 world_state_offsets_type;	

} // namespace planning
} // namespace ai
} // namespace xray

#include "pddl_world_state_property_impl_inline.h"

#endif // #ifndef PDDL_WORLD_STATE_PROPERTY_IMPL_H_INCLUDED
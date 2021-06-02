////////////////////////////////////////////////////////////////////////////
//	Created		: 22.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#ifndef GENERALIZED_ACTION_INLINE_H_INCLUDED
#define GENERALIZED_ACTION_INLINE_H_INCLUDED

namespace xray {
namespace ai {
namespace planning {

inline generalized_action::generalized_action	( pddl_domain const& domain, action_type const type, pcstr name, u32 const cost ) :
	m_caption									( name ),	
	m_type										( type ),
	m_cost										( cost ),
	m_parent									( 0 ),
	m_domain									( domain ),
	next										( 0 )
{
}

inline void generalized_action::add_precondition( pddl_world_state_property_impl const& precondition_to_be_added )
{
	if ( precondition_to_be_added.get_indices_count()!= precondition_to_be_added.get_predicate()->get_parameters_count() )
	{
		LOG_ERROR				( "placeholders count doesn't match with predicate parameters count" );
		return;
	}
	m_preconditions.push_back	( precondition_to_be_added );	
}

inline void generalized_action::add_effect		( pddl_world_state_property_impl const& effect_to_be_added )
{
	if ( effect_to_be_added.get_indices_count() != effect_to_be_added.get_predicate()->get_parameters_count() )
	{
		LOG_ERROR				( "placeholders count doesn't match with predicate parameters count" );
		return;
	}		
	m_effects.push_back			( effect_to_be_added );	
}

inline pddl_world_state_property_impl const& generalized_action::get_precondition	( u32 const index ) const
{
	return						m_preconditions[index];
}

inline pddl_world_state_property_impl const& generalized_action::get_effect			( u32 const index ) const
{
	return						m_effects[index];
}

inline generalized_action const* generalized_action::get_clone	( u32 const index ) const
{
	return						m_clones[index];
}

inline void generalized_action::add_parameter_type				( parameter_type const type )
{
	m_parameter_types.push_back	( type );
}

inline parameter_type generalized_action::get_parameter_type	( u32 const index ) const
{
	return						m_parameter_types[index];
}

} // namespace planning
} // namespace ai
} // namespace xray

#endif // #ifndef GENERALIZED_ACTION_INLINE_H_INCLUDED
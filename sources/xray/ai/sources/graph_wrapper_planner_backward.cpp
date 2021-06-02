////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/graph_wrapper_planner_backward.h>

namespace xray {
namespace ai {
namespace graph_wrapper {

typedef planner_backward::impl			wrapper;

bool wrapper::applicable				(
		properties_type const& search_state,
		properties_type const& preconditions,
		properties_type const& effects
	)
{
	// this is the current search state
	const_iterator it_search			= search_state.begin();
	const_iterator it_end_search		= search_state.end();

	// this is our operator effects we're about to check
	const_iterator it_effects			= effects.begin();
	const_iterator it_end_effects		= effects.end();

	// TRICKY:
	// this is our operator preconditions, we need them to check
	// the following: if there is a precondition and there is no
	// corresponding effect and there is a corresponding property
	// in the current search state, then we need to check if they
	// are the same, otherwise operator is not applicable
	const_iterator it_preconditions		= preconditions.begin();
	const_iterator it_end_preconditions	= preconditions.end();
	
	for ( ; ( it_effects != it_end_effects ) && ( it_search != it_end_search ); )
	{
		if ( (*it_effects).id() < (*it_search).id() )
		{
			// property isn't represented in search state, skip it
			++it_effects;
			continue;
		}

		if ( (*it_effects).id() == (*it_search).id() )
		{
			// property is mentioned in search state, check it
			if ( (*it_effects).value() != (*it_search).value() )
				// property values don't match, operator isn't applicable
				return				false;

			// property values match, continue iterating
			++it_effects;
			++it_search;
			continue;
		}

		// property isn't presented in the effects, try to search
		// in the preconditions
		while ( ( it_preconditions != it_end_preconditions ) && ( (*it_preconditions).id() < (*it_search).id() ) )
			++it_preconditions;

		if ( it_preconditions == it_end_preconditions )
		{
			// we didn't find the property, continue iterating
			++it_search;
			continue;
		}

		if ( (*it_preconditions).id() != (*it_search).id() )
		{
			// we didn't find the property, continue iterating
			++it_search;
			continue;
		}

		if ( (*it_preconditions).value() != (*it_search).value() )
			// property values don't match, operator isn't applicable
			return					false;

		// property matches preconditions, continue iteration
		++it_preconditions;
		++it_search;
	}

	if ( it_search == it_end_search )
		// search state properties are over, operator is applicable
		return						true;

	// properties in the effects are over,
	// operator is still applicable, but there are still
	// some properties in preconditions which may not match
	// current search state properties
	for ( ; ( it_preconditions != it_end_preconditions ) && ( it_search != it_end_search ); )
	{
		if ( (*it_preconditions).id() < (*it_search).id() )
		{
			// property isn't mentioned in preconditions, skip it
			++it_preconditions;
			continue;
		}

		if ( (*it_preconditions).id() > (*it_search).id() )
		{
			// property isn't mentioned in the current search state, skip it
			++it_search;
			continue;
		}

		if ( (*it_preconditions).value() != (*it_search).value() )
			// property values don't match, operator is not applicable
			return					false;

		// property matches preconditions, continue iteration
		++it_preconditions;
		++it_search;
	}

	return							true;
}

bool wrapper::apply					(
		properties_type const& search_state,
		properties_type const& preconditions,
		properties_type const& effects
	)
{
	m_new_state.clear				( );

	// if operator doesn't change the world, we make it inapplicable
	// we can check it in applicable(), but this is an optimization,
	// since in case of applicability we make almost the same work twice
	bool changed					= false;

	// this is our operator preconditions we're about to apply
	const_iterator iter_preconditions		= preconditions.begin();
	const_iterator iter_end_preconditions	= preconditions.end();

	// this is the current search state
	const_iterator iter_search				= search_state.begin();
	const_iterator iter_end_search			= search_state.end();

	// TRICKY:
	// this is our operator effects, we need them to check if our operator applied 
	const_iterator iter_effects				= effects.begin();
	const_iterator iter_end_effects			= effects.end();
	
	for ( ; ( iter_search != iter_end_search ) && ( iter_preconditions != iter_end_preconditions ); )
	{
		if ( (*iter_search).id() > (*iter_preconditions).id() )
		{
			// property isn't mentioned in current search state,
			// but mentioned in preconditions, so add it to the
			// new world state
			m_new_state.add_back			( *iter_preconditions	);
			++iter_preconditions;
			continue;
		}

		if ( (*iter_search).id() == (*iter_preconditions).id() )
		{
			if ( (*iter_search).value() != (*iter_preconditions).value() )
				// property is mentioned in both current search state
				// and preconditions and values are different, so our
				// operator changes current search state, mark it as
				// applicable
				changed						= true;

			// property is mentioned in both current search state
			// and preconditions, so add it to the new world state
			m_new_state.add_back			( *iter_preconditions );
			++iter_search;
			++iter_preconditions;
			continue;
		}

		// property is not in the preconditions, try to search
		// in the effects
		while ( (iter_effects != iter_end_effects ) && ( (*iter_effects).id() < (*iter_search).id() ) )
			++iter_effects;

		if ( iter_effects == iter_end_effects )
		{
			// we didn't find the property,
			// so add it to the new world state
			m_new_state.add_back			( *iter_search );
			++iter_search;
			continue;
		}

		if ( (*iter_effects).id() != (*iter_search).id() )
		{
			// we didn't find the property,
			// so add it to the new world state
			m_new_state.add_back			( *iter_search );
			++iter_search;
			continue;
		}

		// we found the property and verified it matches
		// since otherwise applicable should return false
		// and we can't get to the apply
		R_ASSERT							( (*iter_effects).value() == (*iter_search).value(), "error in algorithm" );
		changed								= true;
		++iter_effects;
		++iter_search;
	}

	if ( iter_search == iter_end_search )
	{
		// current search state properties are over
		if ( !changed )
			// our operator doesn't change the world
			// return it is not applicable
			return							false;

		// add the rest of the properties in preconditions
		// to the new world state
		for ( ; ( iter_preconditions != iter_end_preconditions ); ++iter_preconditions )
			m_new_state.add_back			( *iter_preconditions );

		return								true;
	}

	// preconditions are over, try to search in effects
	for ( ; ( iter_effects != iter_end_effects ) && ( iter_search != iter_end_search ); )
	{
		if ( (*iter_effects).id() < (*iter_search).id() )
		{
			// property is not mentioned in the current search state,
			// but is mentioned in effects, skip it
			++iter_effects;
			continue;
		}

		if ( (*iter_effects).id() > (*iter_search).id() )
		{
			// property isn't mentioned in effects,
			// but mentioned in the current search state,
			// so add it to the new world state
			m_new_state.add_back			( *iter_search );
			++iter_search;
			continue;
		}

		// property isn't mentioned neither in effects nor at the current search state,
		// mask our operator as applicable and ASSERT if their values are equal,
		// since otherwise applicable should return false
		// and we can't get to the apply
		R_ASSERT							( (*iter_effects).value() == (*iter_search).value(), "error in algorithm" );
		changed								= true;
		++iter_effects;
		++iter_search;
	}

	if ( !changed )
		// our operator doesn't change the world
		// return that it's not applicable
		return								false;

	if ( iter_search == iter_end_search )
		// current search state properties are over
		return								true;

	// add the rest of the properties in the current search state
	// to the new world state
	for ( ; iter_search != iter_end_search; ++iter_search )
		m_new_state.add_back				( *iter_search );

	return									true;
}

} // namespace graph_wrapper
} // namespace ai
} // namespace xray
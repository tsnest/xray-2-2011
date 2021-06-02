////////////////////////////////////////////////////////////////////////////
//	Created		: 15.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/graph_wrapper_planner_forward.h>

namespace xray {
namespace ai {
namespace graph_wrapper {

typedef planner_forward::impl	wrapper;

bool wrapper::applicable		(
		properties_type const& search_state,
		properties_type const& preconditions,
		properties_type const& start_state
	)
{
	// this is the current search state
	const_iterator iter_search				= search_state.begin();
	const_iterator iter_end_search			= search_state.end();
	
	// this is our operator preconditions we're about to check
	const_iterator iter_preconditions		= preconditions.begin();
	const_iterator iter_end_preconditions	= preconditions.end();
	
	// TRICKY: (we need this to evaluate the properties we need during our search only)
	// this is our start world state, we need it, since some of the properties
	// in the current search state may not be presented and we will get them
	// from the start world state. if they are not in the start world state,
	// we'll ask graph() to evaluate them for us
	const_iterator iter_initial				= start_state.begin();
	const_iterator iter_end_initial			= start_state.end();
	
	for ( ; ( iter_search != iter_end_search ) && ( iter_preconditions != iter_end_preconditions ); )
	{
		if ( (*iter_search).id() < (*iter_preconditions).id() )
		{
			// property is not mentioned in preconditions, skip it
			++iter_search;
			continue;
		}

		if ( (*iter_search).id() == (*iter_preconditions).id() )
		{
			// property is mentioned in preconditions, check it
			if ( (*iter_search).value() != (*iter_preconditions).value() )
				// property doesn't match, operator is not applicable
				return				false;

			// property matches preconditions, continue iteration
			++iter_search;
			++iter_preconditions;
			continue;
		}

		// property is not in the current search state, try to search
		// in the start world state
		for ( ; ( iter_initial != iter_end_initial ) && ( (*iter_initial).id() < (*iter_preconditions).id() ); )
			// searching for property in the start world state
			++iter_initial;

		if ( ( iter_initial == iter_end_initial ) || ( (*iter_initial).id() > (*iter_preconditions).id() ) )
			// we didn't find the property
			// so, this is the first time we need it
			// during our search
			// we ask graph() to evaluate it
			graph().evaluate		( iter_initial, iter_end_initial, (*iter_preconditions).id() );

		if ( (*iter_initial).value() != (*iter_preconditions).value() )
			// property doesn't match, operator is not applicable
			return					false;

		// property matches preconditions, continue iteration
		++iter_initial;
		++iter_preconditions;
	}

	if ( iter_search != iter_end_search )
		// preconditions are over, operator is applicable
		return						true;

	// properties in the current search state are over,
	// operator is applicable still, but there are still
	// some preconditions which may not match start world
	// state properties
	iter_search						= iter_initial;
	iter_end_search					= iter_end_initial;
	for ( ; iter_preconditions != iter_end_preconditions; )
	{
		if ( ( iter_search == iter_end_search ) || ( (*iter_search).id() > (*iter_preconditions).id() ) )
			// we didn't find the property
			// so, this is the first time we need it
			// during our search
			// we ask graph() to evaluate it
			graph().evaluate		( iter_search,iter_end_search, (*iter_preconditions).id() );
		else
		{
			if ( (*iter_search).id() < (*iter_preconditions).id() )
			{
				// property is not mentioned in preconditions, skip it
				++iter_search;
				continue;
			}
		}

		R_ASSERT					( (*iter_search).id() == (*iter_preconditions).id(), "error in algorithm" );
		if ( (*iter_search).value() != (*iter_preconditions).value() )
			// property doesn't match, operator is not applicable
			return					false;

		// property matches preconditions, continue iteration
		++iter_search;
		++iter_preconditions;
	}
	
	return							true;
}

void wrapper::apply					(
		properties_type const& search_state,
		properties_type const& effects,
		properties_type const& start_state
	)
{
	m_new_state.clear				( );

	// this is the current search state
	const_iterator iter_search		= search_state.begin();
	const_iterator iter_end_search	= search_state.end();
	
	// this is our operator effects we're about to apply
	const_iterator iter_effects		= effects.begin();
	const_iterator iter_end_effects	= effects.end();
	
	// TRICKY: (we need this to evaluate the properties we need during our search only)
	// this is our start world state, we need it to check if our applied 
	// world state contains properties which have the same value as in start
	// world state. we remove these properties from the applied world state
	const_iterator iter_initial		= start_state.begin();
	const_iterator iter_end_initial	= start_state.end();
	
	for ( ; ( iter_search != iter_end_search ) && ( iter_effects != iter_end_effects ); )
	{
		if ( (*iter_search).id() < (*iter_effects).id() )
		{
			// property is not mentioned in effects, but mentioned in
			// current search state, therefore it differs from the
			// start world state, so add it to the new world state
			m_new_state.add_back	( *iter_search );
			++iter_search;
			continue;
		}

		if ( (*iter_search).id() == (*iter_effects).id() )
		{
			if ( (*iter_search).value() == (*iter_effects).value() )
			{
				// property is mentioned in effects, and has the same value as 
				// in the current search state, therefore it differs from the
				// start world state, so add it to the new world state
				m_new_state.add_back( *iter_effects );
			}

			// property is mentioned in effects and has different value
			// in the current search state, therefore it has the same value as in the 
			// start world state, so skip it
			++iter_search;
			++iter_effects;
			continue;
		}

		// property is not in the current search state, try to search
		// in the start world state
		for ( ; ( iter_initial != iter_end_initial ) && ( (*iter_initial).id() < (*iter_effects).id() ); )
			++iter_initial;

		if ( ( iter_initial == iter_end_initial ) || ( (*iter_initial).id() > (*iter_effects).id() ) )
			// we didn't find the property
			// so this is the first time we need it
			// during our search
			// we ask graph() to evaluate it
			graph().evaluate		( iter_initial, iter_end_initial, (*iter_effects).id() );

		if ( (*iter_initial).value() != (*iter_effects).value() )
			// property doesn't match, so add it to the new world state
			m_new_state.add_back	( *iter_effects );

		// property matches, skip it
		++iter_initial;
		++iter_effects;
	}

	if ( iter_search != iter_end_search )
	{
		// effects are over, add all the rest properties
		// from the current search state
		for ( ; iter_search != iter_end_search; ++iter_search )
			m_new_state.add_back	( *iter_search );

		return;
	}

	// properties in the current search state are over
	// continue new world state construction using effects
	// and start world state
	iter_search						= iter_initial;
	iter_end_search					= iter_end_initial;
	for ( ; iter_effects != iter_end_effects; )
	{
		if ( ( iter_search == iter_end_search ) || ( (*iter_search).id() > (*iter_effects).id() ) )
			// we didn't find the property
			// so this is the first time we need it
			// during our search
			// we ask graph() to evaluate it
			graph().evaluate		( iter_search, iter_end_search, (*iter_effects).id() );
		else
		{
			if ( (*iter_search).id() < (*iter_effects).id() )
			{
				// property is not mentioned in effects, skip it
				++iter_search;
				continue;
			}
		}

		R_ASSERT					( (*iter_search).id() == (*iter_effects).id(), "error in algorithm" );
		if ( (*iter_search).value() != (*iter_effects).value() )
			// property has different value, add it to the new world state
			m_new_state.add_back	( *iter_effects );

		// property has the same value, skip it
		++iter_search;
		++iter_effects;
	}

	return;
}

} // namespace graph_wrapper
} // namespace ai
} // namespace xray
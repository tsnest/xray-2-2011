////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/operator_base.h>

namespace xray {
namespace ai {
namespace planning {

operator_base::~operator_base			( )
{
}

void operator_base::compute_min_weight	( )
{
	if ( m_preconditions_hash == preconditions().hash_value() )
	{
		if ( m_effects_hash == effects().hash_value() )
			return;
	}

	m_min_weight		= 0;

	typedef state_type::properties_type::const_iterator	const_iterator;
	
	const_iterator iter_preconditions		= preconditions().properties().begin();
	const_iterator iter_end_preconditions	= preconditions().properties().end();
	const_iterator iter_effects				= effects().properties().begin();
	const_iterator iter_end_effects			= effects().properties().end();
	
	for ( ; ( iter_effects != iter_end_effects ) && ( iter_preconditions != iter_end_preconditions ); )
	{
		if ( (*iter_preconditions).id() < (*iter_effects).id() )
		{
			++iter_preconditions;
			continue;
		}

		if ( (*iter_preconditions).id() > (*iter_effects).id() )
		{
			++m_min_weight;
			++iter_effects;
			continue;
		}

		if ( (*iter_preconditions).value() != (*iter_effects).value() )
			++m_min_weight;

		++iter_preconditions;
		++iter_effects;
	}

	if ( iter_effects == iter_end_effects )
		return;

	m_min_weight		= m_min_weight + operator_weight_type( iter_end_effects - iter_effects );
}

operator_base::operator_weight_type const& operator_base::min_weight	( )
{
	compute_min_weight	( );
	return				m_min_weight;
}

} // namespace planning
} // namespace ai
} // namespace xray


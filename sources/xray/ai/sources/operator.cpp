////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/operator.h>
#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/plan_tracker.h>

namespace xray {
namespace ai {
namespace planning {

void operator_impl::on_after_addition	( propositional_planner* object )
{
	R_ASSERT							( object );
	m_object							= object;
}

void operator_impl::make_inactual		( )
{
	R_ASSERT							( m_object );
	m_object->make_inactual				( );
}
 
void operator_impl::initialize			( )
{
	m_first_execute						= true;

// 	if ( !m_object->tracker().verbose() )
// 		return;

	LOG_INFO							( "operator %s is initialized", id() );
}

void operator_impl::execute				( )
{
	if ( !m_first_execute )
		return;

	m_first_execute						= false;

// 	if ( !m_object->tracker().verbose() )
// 		return;

	LOG_INFO							( "operator %s is executed", id() );
}

void operator_impl::finalize			( )
{
// 	if ( !m_object->tracker().verbose() )
// 		return;

	LOG_INFO							( "operator %s is finalized", id() );
}

void operator_impl::dump				( pcstr offset )
{
	XRAY_UNREFERENCED_PARAMETER			( offset );
}

operator_impl::operator_weight_type operator_impl::weight	( state_type const& state0, state_type const& state1 )
{
	XRAY_UNREFERENCED_PARAMETERS		( &state0, &state1 );
	R_ASSERT							( m_cost >= min_weight() );
	return								m_cost;
}

} // namespace planning
} // namespace ai
} // namespace xray


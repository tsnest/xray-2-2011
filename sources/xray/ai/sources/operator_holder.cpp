////////////////////////////////////////////////////////////////////////////
//	Created		: 14.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/operator_holder.h>
#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/operator.h>
#include <xray/ai/search/plan_tracker.h>

namespace xray {
namespace ai {
namespace planning {

operator_holder::~operator_holder	( )
{
	clear							( );
}

void operator_holder::clear			( )
{
	while ( !objects().empty() )
		remove( objects().back().id() );
}

void operator_holder::add			( operator_id_type const& operator_id, operator_ptr_type operator_impl )
{
	objects_type::iterator iter		= std::lower_bound( m_objects.begin(), m_objects.end(), operator_id );
	R_ASSERT						( ( iter == m_objects.end() ) || ( (*iter).id() != operator_id ), "cannot find operator with id %d", operator_id );

#ifdef DEBUG
	m_planner.validate				( operator_impl->preconditions() );
	m_planner.validate				( operator_impl->effects() );
#endif // #ifdef DEBUG

	m_planner.make_inactual			( );
	m_objects.insert				( iter, operator_pair_type( operator_id, operator_impl ) );
	operator_impl->on_after_addition( &m_planner );
}

void operator_holder::remove_impl	( operator_id_type const& operator_id, bool const notify_holder )
{
	objects_type::iterator iter		= std::lower_bound( m_objects.begin(), m_objects.end(), operator_id );
	R_ASSERT						( m_objects.end() != iter, "cannot find operator with id %d", operator_id );

	operator_ptr_type operator_ptr	= (*iter).get_operator();
	DELETE							( operator_ptr );
	m_objects.erase					( iter );
	if ( !notify_holder )
		return;

	m_planner.make_inactual			( );
}

operator_holder::operator_ptr_type operator_holder::object( operator_id_type const& operator_id )
{
	objects_type::iterator iter		= std::lower_bound( m_objects.begin(), m_objects.end(), operator_id );
	R_ASSERT						( m_objects.end() != iter, "propositional_planner[%s] cannot find operator with id[%d]", m_planner.id(), operator_id );
	return							(*iter).get_operator();
}

} // namespace planning
} // namespace ai
} // namespace xray
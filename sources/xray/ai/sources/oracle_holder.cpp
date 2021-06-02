////////////////////////////////////////////////////////////////////////////
//	Created		: 18.07.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include <xray/ai/search/oracle_holder.h>
#include <xray/ai/search/propositional_planner.h>
#include <xray/ai/search/oracle.h>

namespace xray {
namespace ai {
namespace planning {

oracle_holder::~oracle_holder	( )
{
	while ( !objects().empty() )
		remove_impl				( objects().rbegin()->first, false );
}

void oracle_holder::clear		( )
{
	while ( !objects().empty() )
		remove					( objects().rbegin()->first );
}

void oracle_holder::add			( oracle_id_type const& oracle_id, oracle_ptr_type oracle )
{
	R_ASSERT					( m_objects.find( oracle_id ) == m_objects.end(), "cannot find oracle with id %d", oracle_id );
	m_objects.insert			( std::make_pair( oracle_id, oracle ) );
}

void oracle_holder::remove_impl	( oracle_id_type const& oracle_id, bool const notify_holder )
{
	objects_type::iterator iter	= m_objects.find( oracle_id );
	R_ASSERT					( iter != m_objects.end(), "cannot find oracle with id %d", oracle_id );
	DELETE						( (*iter).second );
	m_objects.erase				( iter );

	if ( !notify_holder )
		return;

	m_planner.make_inactual		( );
}

oracle_holder::oracle_ptr_type oracle_holder::object	( oracle_id_type const& oracle_id )
{
	objects_type::const_iterator iter	= objects().find( oracle_id );
	R_ASSERT							( objects().end() != iter, "cannot find oracle with id %d", oracle_id );
	return								(*iter).second;
}

} // namespace planning
} // namespace ai
} // namespace xray


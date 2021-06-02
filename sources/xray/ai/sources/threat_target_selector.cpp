////////////////////////////////////////////////////////////////////////////
//	Created		: 24.01.2011
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "threat_target_selector.h"
#include "percept_memory_object.h"
#include "percept_memory_object_types.h"
#include "working_memory.h"
#include "ai_world.h"

namespace xray {
namespace ai {
namespace selectors {

threat_target_selector::threat_target_selector	( ai_world& world, working_memory const& memory, blackboard& board, pcstr name ) :
	target_selector_base						( world, memory, board, name )
{
}

static float get_recalculated_confidence		( ai_world& world, percept_memory_object* object )
{
	// after every 1000 milliseconds confidence of memory object must be considered as decreased by 0.06
	return object->confidence - ( ( world.get_current_time_in_ms() - object->update_time ) * 0.06f / 1000.f );
}

struct find_highest_confidence_predicate : private boost::noncopyable
{
	inline find_highest_confidence_predicate	( ai_world& selector_world ) :
		object_with_highest_confidence			( 0 ),
		world									( selector_world )
	{
	}

	inline void operator()				( percept_memory_object* memory_object )
	{
		object_with_highest_confidence	= !object_with_highest_confidence ? memory_object :
										( get_recalculated_confidence( world, object_with_highest_confidence ) <
										  get_recalculated_confidence( world, memory_object ) ?
										memory_object : object_with_highest_confidence );
	}

	percept_memory_object*				object_with_highest_confidence;
	ai_world&							world;
};

void threat_target_selector::tick		( )
{
	find_highest_confidence_predicate	find_predicate( m_world );
	m_working_memory.for_each			( percept_memory_object_type_threat, find_predicate );
	
	if ( percept_memory_object* memory_object = find_predicate.object_with_highest_confidence )
	{
		m_blackboard.set_current_threat	( c_ptr( memory_object->object ) );
	}
}

planning::object_type threat_target_selector::get_target	( u32 const target_index )
{
	XRAY_UNREFERENCED_PARAMETER					( target_index );
	return										std::make_pair( pcvoid( 0 ), "" );
}

void threat_target_selector::fill_targets_list	( )
{
}

void threat_target_selector::clear_targets		( )
{
}

pcstr threat_target_selector::get_target_caption( u32 const target_index ) const
{
	XRAY_UNREFERENCED_PARAMETER					( target_index );
	return										"";
}

void threat_target_selector::dump_state			( npc_statistics& stats ) const
{
	XRAY_UNREFERENCED_PARAMETER					( stats );
}

} // namespace selectors
} // namespace ai
} // namespace xray

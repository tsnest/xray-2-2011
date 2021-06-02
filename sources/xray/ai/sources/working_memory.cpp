////////////////////////////////////////////////////////////////////////////
//	Created		: 29.12.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "working_memory.h"
#include <xray/ai/npc_statistics.h>
#include "ai_world.h"

namespace xray {
namespace ai {

working_memory::working_memory	( size_t const objects_count, ai_world& world ) :
	// these 2 members must be initialized exactly in this order!
	m_subscription				( boost::bind( &working_memory::forget_all_about_object, return_this(), _1 ) ),
	m_world						( world ),
	m_memory					( MALLOC( objects_count * sizeof( percept_memory_object ), "working_memory" ) ),
	m_allocator					( m_memory, objects_count * sizeof( percept_memory_object ) )
{
	m_world.destruction_manager().subscribe( m_subscription );
}

working_memory::~working_memory	( )
{
	//LOG_INFO					( "destroying working memory 0x%8x", this );
}

void working_memory::clear_resources	( )
{
	forget_all					( );
	m_world.destruction_manager().unsubscribe( m_subscription );
	FREE						( m_memory );
}

percept_memory_object* working_memory::create_memory_object	( percept_memory_object_types knowledge_type )
{
	if ( m_allocator.total_size() == m_allocator.allocated_size() )
		displace_memory_object();
	
	percept_memory_object* new_memory_object	= XRAY_NEW_IMPL( m_allocator, percept_memory_object )( knowledge_type );
	m_percept_objects[knowledge_type].push_back( new_memory_object );
	return										new_memory_object;
}

void working_memory::delete_memory_object	( percept_memory_object* object_to_be_deleted )
{
	m_percept_objects[object_to_be_deleted->type].erase( object_to_be_deleted );
	XRAY_DELETE_IMPL						( m_allocator, object_to_be_deleted );
} 

void working_memory::displace_memory_object	( )
{
	delete_memory_object( get_oldest_object() );
}

percept_memory_object* working_memory::get_newest_object_by_type	(
		percept_memory_object_types knowledge_type
	) const
{
	knowledge const& knowledge_by_type	= m_percept_objects[knowledge_type];
	return								knowledge_by_type.back();
}

percept_memory_object* working_memory::get_oldest_object( ) const
{
	if ( m_percept_objects.empty() )
		return								0;
	
	percept_memory_object* result			= 0;
	for ( percept_objects::const_iterator iter = m_percept_objects.begin(); iter != m_percept_objects.end(); ++iter )
	{
		knowledge const& knowledge_by_type	= *iter;
		if ( knowledge_by_type.empty() )
			continue;
		
		result								= !result ? knowledge_by_type.front() :
											( result->update_time < knowledge_by_type.front()->update_time ?
											  result : knowledge_by_type.front() );
	}
	return									result;
}

 void working_memory::forget_all	( )
 {
	for ( percept_objects::iterator iter = m_percept_objects.begin(); iter != m_percept_objects.end(); ++iter )
		while ( percept_memory_object* memory_object = iter->pop_front() )
			delete_memory_object	( memory_object );
 }

 struct find_percept_object_by_game_object : private boost::noncopyable
 {
	inline find_percept_object_by_game_object	( game_object const& object, working_memory& memory ) :
		object_to_be_forgotten					( object ),
		memory_to_be_cleaned					( memory )
	{
	}

	inline void operator()						( percept_memory_object* memory_object )
	{
		if ( c_ptr( memory_object->object ) == &object_to_be_forgotten )	
			memory_to_be_cleaned.delete_memory_object( memory_object );
	}

	game_object const&							object_to_be_forgotten;
	working_memory&								memory_to_be_cleaned;
 };
 
 void working_memory::forget_all_about_object	( game_object const& object )
 {
	find_percept_object_by_game_object			find_all_facts_predicate( object, *this );
	
	for ( percept_objects::iterator iter = m_percept_objects.begin(); iter != m_percept_objects.end(); ++iter )
	{
		knowledge& knowledge_by_type			= *iter;
		knowledge_by_type.for_each				( find_all_facts_predicate );
	}
}

bool working_memory::has_facts_about_danger		( ) const
{
	return !m_percept_objects[percept_memory_object_type_threat].empty() &&
		   !m_percept_objects[percept_memory_object_type_enemy].empty();
}

struct dump_memory_facts_predicate : private boost::noncopyable
 {
	inline dump_memory_facts_predicate	( npc_statistics& stats, pcstr type_caption ) :
		npc_stats						( stats ),
		caption							( type_caption )
	{
	}

	inline void operator()				( percept_memory_object* memory_object )
	{
		typedef npc_statistics::sensor_info_type::content_type content_type;
		content_type					new_item_content( caption );
		new_item_content.append			( memory_object->object->get_name() );
		npc_stats.working_memory_state.content.push_back( new_item_content );
	}

	npc_statistics&						npc_stats;
	pcstr								caption;
 };

void working_memory::dump_state			( npc_statistics& stats ) const
{
	stats.working_memory_state.caption	= "working memory state:";

	for ( u32 i = 0; i < percept_memory_object_types_count; ++i )
	{
		knowledge const& facts			= m_percept_objects[i];
		if ( !facts.empty() )
		{
			dump_memory_facts_predicate	state_dumper_predicate( stats, memory_object_types_captions[i].second );
			facts.for_each				( state_dumper_predicate );
		}
	}
	if ( m_percept_objects.empty() )
		stats.working_memory_state.content.push_back( "none" );
}

} // namespace ai
} // namespace xray
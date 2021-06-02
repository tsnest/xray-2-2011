////////////////////////////////////////////////////////////////////////////
//	Created		: 23.11.2010
//	Author		: Tetyana Meleshchenko
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef WORKING_MEMORY_H_INCLUDED
#define WORKING_MEMORY_H_INCLUDED

#include <boost/array.hpp>
#include "percept_memory_object.h"
#include "percept_memory_object_types.h"
#include "game_object_subscriber.h"
#include <xray/memory_single_size_buffer_allocator.h>

namespace xray {
namespace ai {

struct npc_statistics;
class ai_world;

class working_memory : private boost::noncopyable
{
public:
	typedef intrusive_list< percept_memory_object,
							percept_memory_object*,
							&percept_memory_object::next > knowledge;
		
	typedef boost::array< knowledge, percept_memory_object_types_count > percept_objects;

public:						
							working_memory					( size_t const objects_count, ai_world& world );
							~working_memory					( );

	percept_memory_object*	create_memory_object			( percept_memory_object_types knowledge_type );
	void					delete_memory_object			( percept_memory_object* object_to_be_deleted );
	percept_memory_object*	get_newest_object_by_type		( percept_memory_object_types knowledge_type ) const;
	void					forget_all						( );
	void					forget_all_about_object			( game_object const& object );
	bool					has_facts_about_danger			( ) const;
	void					dump_state						( npc_statistics& stats ) const;
	void					clear_resources					( );

	template < class Predicate >
	inline	void			for_each						(
								percept_memory_object_types knowledge_type,
								Predicate& pred
							) const;

private:
	typedef memory::single_size_buffer_allocator< sizeof( percept_memory_object ),
												  threading::single_threading_policy > 
												  working_memory_allocator;

private:
	void						displace_memory_object		( );
	percept_memory_object*		get_oldest_object			( ) const;
	inline working_memory*		return_this					( ) { return this; }

private:
	percept_objects				m_percept_objects;
	game_object_subscriber		m_subscription;
	
	// these 2 members must be declared exactly in this order! (till ALIGNED_MALLOC will be implemented)
	pvoid						m_memory;
	working_memory_allocator	m_allocator;
	ai_world&					m_world;
}; // class working_memory

} // namespace ai
} // namespace xray

#include "working_memory_inline.h"

#endif // #ifndef WORKING_MEMORY_H_INCLUDED
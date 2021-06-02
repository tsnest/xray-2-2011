////////////////////////////////////////////////////////////////////////////
//	Created		: 09.06.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef XRAY_RESOURCES_MEMORY_H_INCLUDED
#define XRAY_RESOURCES_MEMORY_H_INCLUDED

#include <xray/intrusive_double_linked_list.h>
#include <xray/intrusive_list.h>
#include <xray/resources_query_result.h>
#include <xray/timing_timer.h>

namespace xray {
namespace resources {

class memory_type
{
public:
					memory_type			(pcstr name);

	pcstr			name				() const { return m_name; }

public:
	memory_type *						m_next;

private:
	memory_type	(memory_type const &) {}
	void operator = (memory_type const &) {}

	typedef intrusive_list< 
		query_result, 
		query_result *, 
		& query_result::m_next_out_of_memory, 
		threading::mutex 
	> out_of_memory_queue;

	typedef	intrusive_double_linked_list<	
		resource_base, 
		resource_base *, 
		& resource_base::m_prev_in_memory_type, 
		& resource_base::m_next_in_memory_type, 
		threading::single_threading_policy,
		size_policy	
	>	resources_list;

	pcstr								m_name;
	bool								in_list;

	resources_list						resources;
	u64									sort_actuality_tick;

	out_of_memory_queue					queue;
	enum listen_enum					{ listen_none,
										  listen_freed, // first query has requested some resources to free/decrease quality and waiting notification to try reallocation
										  listen_all }; // there was nothing to free/decrease quality. we now just wait for any deallocations
	
	listen_enum							listen_type;
	timing::timer						listen_all_timer;

	friend class						game_resources_manager;
	friend class						quality_increase_functionality;
	friend class						releasing_functionality;
	friend class						sorting_functionality;
	friend class						resource_freeing_functionality;
}; // memory_type

typedef intrusive_list< 
	memory_type, 
	memory_type *, 
	& memory_type::m_next, 
	threading::mutex 
> memory_type_list;

extern memory_type XRAY_CORE_API 		nocache_memory;
extern memory_type XRAY_CORE_API		managed_memory;
extern memory_type XRAY_CORE_API		unmanaged_memory;

} // namespace resources
} // namespace xray

#endif // #ifndef XRAY_RESOURCES_MEMORY_H_INCLUDED
////////////////////////////////////////////////////////////////////////////
//	Created		: 10.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_sorting.h"

namespace xray {
namespace resources {

static float const satisfaction_equality_tolerance	=	0.05f;

sorting_functionality::sorting_functionality (game_resources_manager_data & data)
: m_data(data), m_sort_actuality_tick(1)
{
}

struct sorting_predicate
{
	bool		operator () (resource_base const * left, 
							 resource_base const * right)
	{
		float const left_satisfaction	=	left->current_satisfaction();
		float const right_satisfaction	=	right->current_satisfaction();
		if ( math::abs(left_satisfaction - right_satisfaction) < satisfaction_equality_tolerance )
			return			left->reconstruction_size() < right->reconstruction_size();

		return		left_satisfaction > right_satisfaction;
	}
};

void   sorting_functionality::log_memory_type_resources (memory_type * memory_type)
{
	fixed_string8192	log_string;
	for ( resource_base *	it_resource	=	memory_type->resources.front();
							it_resource;
							it_resource	=	memory_type->resources.get_next_of_object(it_resource) )
	{
		log_string						+=	resources::log_string(it_resource);
		log_string.appendf					("(%f)", it_resource->current_satisfaction());

	}
	LOGI_INFO								("grm", "memory queue: %s", log_string.c_str());
}

void   sorting_functionality::sort_resources_if_needed (memory_type * memory_type)
{
	if ( memory_type->sort_actuality_tick == m_sort_actuality_tick )
		return;
	
	u32 const resources_count			=	memory_type->resources.size();
	typedef	buffer_vector<resource_base *>	resources_array_type;
	resources_array_type					resources_array	(ALLOCA(resources_count * sizeof(resource_base *)),
															 resources_count);

	for ( resource_base *	it_resource	=	memory_type->resources.front();
							it_resource;
							it_resource	=	memory_type->resources.get_next_of_object(it_resource) )
	{
		resources_array.push_back			(it_resource);
		it_resource->update_reconstruction_info	(m_sort_actuality_tick);
	}

	std::sort								(resources_array.begin(), resources_array.end(), sorting_predicate());
	memory_type->resources.clear		();
	for ( resources_array_type::iterator	it		=	resources_array.begin(),
											it_end	=	resources_array.end();
											it		!=	it_end;
											++it )
	{
		memory_type->resources.push_back	(* it);
	}

	//log_memory_type_resources				(memory_type);

	++m_sort_actuality_tick;
}

} // namespace resources
} // namespace xray

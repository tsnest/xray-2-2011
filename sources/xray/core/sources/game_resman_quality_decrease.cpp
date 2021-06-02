////////////////////////////////////////////////////////////////////////////
//	Created		: 28.07.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resources_manager.h"
#include "game_resman_free.h"
#include "game_resman_quality_decrease.h"
#include "game_resman_quality_increase.h"
#include "game_resman_test_resource.h"
#include "game_resman_test_utils.h"

namespace xray {
namespace resources {

static const float	min_satisfaction_slack	=	0.1f;

quality_decreasing_functionality::quality_decreasing_functionality (resource_freeing_functionality & freeing_functionality, 
																	float lowest_satisfaction_level) 
:	m_freeing_functionality		(freeing_functionality), 
	m_lowest_satisfaction_level	(lowest_satisfaction_level) 
{
}

decrease_is_possible_bool   quality_decreasing_functionality::collect_quality_resources (resource_base *			resource, 
																						 quality_resources_list *	out_quality_resources)
{
	if ( resource->in_grm_observed_list() )
		return									decrease_is_possible_true; // already freed by other branch

	resource_link_list & parents			=	resource->get_parents();
	if ( parents.empty() )
		return									(resource->reference_count() == 1) ? 
												decrease_is_possible_true : decrease_is_possible_false;

	resource_link_list::mutex_raii	raii		(parents);

	for ( resource_link *	it_link			=	resource_link_list_front_no_dying(parents); 
							it_link; 
							it_link			=	resource_link_list_next_no_dying(it_link) )
	{
		if ( it_link->is_quality_link() )
		{
			u32 const worst_quality_level	=	it_link->resource->quality_levels_count() - 1;
			if ( it_link->quality_value == worst_quality_level )
				return							decrease_is_possible_false; // worst quality already, cannot decrease
			
			if ( it_link->resource->is_increasing_quality() )
				return							decrease_is_possible_false;

			float const satisfaction		=	it_link->resource->satisfaction(it_link->quality_value + 1);

			if ( satisfaction - min_satisfaction_slack <= m_lowest_satisfaction_level )
				return							decrease_is_possible_false;
		}
		else if ( !collect_quality_resources	(it_link->resource, out_quality_resources) )
			return								decrease_is_possible_false;
	}

	if ( resource->is_quality_resource() )
		out_quality_resources->push_back		(resource);

	return										decrease_is_possible_true;
}

void   quality_decreasing_functionality::decrease_for_parents (resource_base * top_resource)
{
	resource_link_list &	parents			=	top_resource->get_parents();
	
	typedef	buffer_vector<resource_link *>		parents_array_type;
	resource_link_list::mutex_raii	raii		(parents);
	
	for ( resource_link * it_link = resource_link_list_front_no_dying(parents); it_link; )
	{
		u32 const previous_parents_count	=	parents.size();
		resource_link * const it_next		=	resource_link_list_next_no_dying(it_link);
		if ( !it_link->is_quality_link() )
		{
			it_link							=	it_next;			
			continue;
		}

		u32 const worst_quality_level		=	it_link->resource->quality_levels_count() - 1;
		CURE_ASSERT_CMP							(it_link->quality_value, !=, worst_quality_level, break);
		
		u32 const new_best_quality			=	it_link->quality_value + 1;
		LOGI_DEBUG								("grm", "%s decrease quality %d->%d (satisfaction will be: %f)", 
												 log_string(it_link->resource).c_str(),
												 it_link->resource->current_quality_level(), 
												 new_best_quality, 
												 it_link->resource->satisfaction(new_best_quality));

		resource_base * const resource_to_decrease_quality		=	it_link->resource;
		resource_to_decrease_quality->decrease_quality		(new_best_quality);
		quality_increase_functionality	quality_increase			(m_freeing_functionality.data());
		quality_increase.update_current_satisfaction_for_resource	(resource_to_decrease_quality);

		u32 const current_parents_count		=	parents.size();
		CURE_ASSERT_CMP							(current_parents_count, <, previous_parents_count, break);
		it_link								=	it_next;
	}
}

bool   quality_decreasing_functionality::try_decrease (resource_base * resource)
{
	if ( resource->in_grm_observed_list() )
		return									true; // already freed by other branch

	if ( !resource->is_on_quality_branch() )
		return									false;

	resources_to_free_collection::resources_to_free_list	quality_resources;
	if ( collect_quality_resources(resource, & quality_resources) == decrease_is_possible_false )
		return									false;

	while ( !quality_resources.empty() )
	{
		resource_base * const quality_resource	=	quality_resources.pop_front();
		decrease_for_parents					(quality_resource);
	}
	
	return										m_freeing_functionality.try_collect_to_free_resource(resource);
}

} // namespace resources
} // namespace xray

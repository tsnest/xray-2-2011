////////////////////////////////////////////////////////////////////////////
//	Created		: 16.07.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman.h"
#include "game_resman_free.h"
#include "game_resman_test_resource.h"
#include "game_resman_quality_increase.h"
#include "game_resman_release.h"
#include "game_resman_quality_decrease.h"

namespace xray {
namespace resources {

struct action_logger
{
	action_logger	(pcstr action_message, test_resource * grm_test_resource) 
		: result(false), grm_test_resource(grm_test_resource), action_message(action_message)
	{ 
// 		if ( grm_test_resource ) 
// 			LOGI_INFO	("grm", "started %s for %s", action_message, grm_test_resource->name()); 
	}
	~action_logger	()	
	{ 
		if ( grm_test_resource ) 
			LOGI_INFO	("grm", "%s for %s : %s", action_message, grm_test_resource->name(), result ? "scheduled" : "failed"); 
	}
	
	bool				result;
	test_resource *		grm_test_resource;
	pcstr				action_message;
};

bool   resource_freeing_functionality::parents_can_be_freed (resource_base * resource, bool * can_try_free, bool * can_try_decrease_quality)
{
	resource_link_list & parents		=	resource->get_parents();
	resource_link_list::mutex_raii	raii	(parents);

	* can_try_free						=	true;
	* can_try_decrease_quality			=	true;

	for ( resource_link *	it_parent	=	resource_link_list_front_no_dying(parents); 
							it_parent;
							it_parent	=	resource_link_list_next_no_dying(it_parent) )
	{
		bool	parent_can_try_free				=	false; 
		bool	parent_can_try_decrease_quality	=	false;
		
		if ( it_parent->resource->is_pinned_by_grm() )
			can_be_freed	(it_parent->resource, & parent_can_try_free, & parent_can_try_decrease_quality);

		if ( !parent_can_try_free )
		{
			* can_try_free				=	false;
			if ( !it_parent->is_quality_link() )
			{
				* can_try_decrease_quality	=	false;
				return						false;
			}
		}
	}

	return									true;
}

bool   resource_freeing_functionality::can_be_freed (resource_base * resource, bool * can_try_free, bool * can_try_decrease_quality)
{
	bool const is_increasing_quality	=	resource->is_resource_with_quality() && resource->is_increasing_quality();

	if ( resource->has_user_references() || is_increasing_quality )
	{
		* can_try_free					=	false;
		* can_try_decrease_quality		=	false;
		return								false;
	}

	return									parents_can_be_freed(resource, can_try_free, can_try_decrease_quality);
}

bool   resource_freeing_functionality::try_collect_parents_to_free(resource_base * resource)
{
	resource_link_list & parents			=	resource->get_parents();
	if ( parents.empty() )
		return									true;

	// uncomment when need debug of grm test
	//if ( test_resource * grm_test_resource = dynamic_cast<test_resource *>(resource) )
	//	LOGI_DEBUG								("grm", "trying to free parents of : %s", grm_test_resource->name());

	resource_link_list::mutex_raii	raii		(parents);
	resource_link * it_parent				=	resource_link_list_front_no_dying(parents);
	while ( it_parent )
	{
		if ( !try_collect_to_free_resource(it_parent->resource) )
			return								false;
		it_parent							=	resource_link_list_next_no_dying(it_parent);
	}

	return										true;
}

void   resource_freeing_functionality::collect_to_free (resource_base * resource)
{
	R_ASSERT_CMP	( (long)resource->reference_count(), ==, (long)(1 + resource->get_parents_count()));

	memory_usage_type const & memory_usage	=	resource->memory_usage();
	ASSERT										(memory_usage.type != & nocache_memory);

	m_collection.resources.push_back			(resource);
	resource->set_in_grm_observed_list			();
	if ( m_collection.collected_memory.same_type_with(memory_usage) )
		m_collection.collected_memory.size	+=	memory_usage.size;
}

bool   resource_freeing_functionality::try_collect_to_free_resource (resource_base * resource)
{
	if ( resource->in_grm_observed_list() )
		return								true; // already freed by other branch

	action_logger	test_logger				("freeing", dynamic_cast<test_resource *>(resource));
	
	if ( resource->has_user_references() )
		return								false;

	if ( !resource->is_pinned_by_grm() )
		return								true;
	
	if ( !try_collect_parents_to_free(resource) )
		return								false;

	u32 const count_that_allows_unregister	=	1 + resource->get_parents_count();
	if ( !resource->try_unregister_from_fat_or_from_name_registry(count_that_allows_unregister) )
		return								false;
	
	collect_to_free							(resource);

	test_logger.result					=	true;
	return									true;
}

void   resource_freeing_functionality::release_sub_fat_from_parents (vfs_sub_fat_resource * const sub_fat)
{
	resource_link_list & parents		=	sub_fat->get_parents();
	resource_link_list::mutex_raii	raii	(parents);
	
	resource_link * it_parent			=	resource_link_list_front_no_dying(parents);
	while ( it_parent )
	{
		if ( !try_collect_to_free_resource(it_parent->resource) )
		{
			resource_base * const parent	=	it_parent->resource;
			LOGI_WARNING					("resources", 
											 "LEAK: %s or one of its parents is held by user"
											 "when its sub-fat being unmounted, leak?",
											 parent->log_string().c_str());

			// force unregister from fat-node
			bool const unregistered		=	parent->try_unregister_from_fat_or_from_name_registry(u32(-1));
			R_ASSERT_U						(unregistered);

			parent->clean_sub_fat_and_fat_it	();
		}
		it_parent						=	resource_link_list_next_no_dying(it_parent);
	}
}

// freeing sub_fat is specialized because we want to allow unmount even with leaks
// when some resources still hold references to sub_fat, we unlink them and continue with warning
void   resource_freeing_functionality::release_sub_fat (vfs_sub_fat_resource * const sub_fat)
{
	R_ASSERT								(!sub_fat->in_grm_observed_list(), "wtf how come?");

	R_ASSERT								(!sub_fat->has_user_references(), 
											 "sub-fat '%s' should not be held by user link! Call Lain",
											 sub_fat->mount_ptr->get_virtual_path());

	u32 const max_tries					=	10;
	u32 tries							=	0;
	u32 count_that_allows_unregister	=	0;
	do 
	{
		release_sub_fat_from_parents		(sub_fat);
		
		count_that_allows_unregister	=	1 + sub_fat->get_parents_count();
		++tries;

	} while ( tries < max_tries && !((resource_base *)sub_fat)->try_unregister_from_fat_or_from_name_registry(count_that_allows_unregister) );

	R_ASSERT								(tries < max_tries, "cannot unregister from fat even after %d tries! Something really wrong, call Lain!", max_tries);
	
	collect_to_free							(sub_fat);

	free_collected							();
}

static
resource_base *   last_collected_of_required_type (resources_to_free_collection & collection)
{
	if ( !collection.query )
		return								NULL;

	resource_base * out_last_of_type	=	NULL;
	resource_base * it_current			=	collection.resources.front();
	while ( it_current )
	{
		if ( it_current->memory_usage().same_type_with(collection.required_memory) )
			out_last_of_type			=	it_current;

		it_current						=	collection.resources.get_next_of_object(it_current);
	}

	return									out_last_of_type;
}

void   resource_freeing_functionality::free_collected ()
{
	if ( m_collection.query )
		m_collection.query->set_ready_to_retry_action_that_caused_out_of_memory	(false);

	resource_base * const last_of_type	=	last_collected_of_required_type(m_collection);

	releasing_functionality	releasing		(m_data);

	while ( !m_collection.resources.empty() ) 
	{
		resource_base * const resource	=	m_collection.resources.pop_front();
		
		if ( m_collection.query && resource->memory_usage().same_type_with(m_collection.required_memory) )
			m_collection.query->observe_resource_destruction	(resource);

		if ( resource == last_of_type ) 
		{
			R_ASSERT						 (m_collection.query);
			m_collection.query->set_ready_to_retry_action_that_caused_out_of_memory	(true);
		}

		releasing.release_resource			(resource);
	}
}

bool   resource_freeing_functionality::try_collect_to_free ()
{
	float const lowest_satisfaction_level	=	m_collection.query->target_satisfaction();
	quality_decreasing_functionality	quality_decreasing	(* this, lowest_satisfaction_level);

	for ( resource_base *	it_resource	=	m_collection.info->resources.front();
							it_resource;
							it_resource	=	m_collection.info->resources.get_next_of_object(it_resource) )
	{
		bool can_try_free				=	false;
		bool can_try_decrease_quality	=	false;

		if ( !can_be_freed(it_resource, & can_try_free, & can_try_decrease_quality) )
			continue;

		bool const freed_or_decreased	=	(can_try_decrease_quality && quality_decreasing.try_decrease(it_resource))
												||
											(can_try_free && try_collect_to_free_resource(it_resource));

		XRAY_UNREFERENCED_PARAMETER			(freed_or_decreased);

		if ( m_collection.collected_memory.size >= m_collection.required_memory.size )
 			break;
	}

	return									m_collection.collected_memory.size != 0;
}

} // namespace resources
} // namespace xray
////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_release.h"
#include "game_resman.h"
#include "game_resman_quality_increase.h"

namespace xray {
namespace resources {

void   releasing_functionality::release_resource (resource_base * const resource)
{
	if ( !resource->is_pinned_by_grm() )
		return;

	managed_resource_ptr	managed_resource	=	resource->cast_managed_resource();
	unmanaged_resource_ptr	unmanaged_resource	=	resource->cast_unmanaged_resource();

	memory_type * memory_type			=	(class memory_type *)resource->memory_usage().type;
	memory_type->resources.erase			(resource);

	if ( resource->has_flags(resource_flag_in_grm_increase_quality_tree) )
	{
		quality_increase_functionality	functionality	(m_data);
		functionality.erase_from_increase_quality_tree	(resource);
	}

	// we ask resource to release its sub-fat / fat_it for unmount to work correctly
	resource->clean_sub_fat_and_fat_it		();

	resource->cast_base_of_intrusive_base()->on_release_decrement_reference_count(); 
	// oook, now its fully released by grm
	// destructor of ptr will call object if no parents are alive
}

template <class RemoveIfPredicate>
void   releasing_functionality::release_resources_of_memory_type (memory_type * info, RemoveIfPredicate predicate)
{
	for ( resource_base *	it_resource	=	info->resources.front();
							it_resource;	)
	{
		resource_base * it_next			=	info->resources.get_next_of_object(it_resource);
		if ( predicate(it_resource) )
		{
			bool const is_increasing_quality	=	it_resource->is_resource_with_quality() && 
													it_resource->is_increasing_quality();

			LOGI_DEBUG						("grm", "releasing %s", it_resource->log_string().c_str());
			if ( !is_increasing_quality )
				release_resource			(it_resource);
		}

		it_resource						=	it_next;
	}
}

struct release_by_class_id_predicate
{
	release_by_class_id_predicate (class_id_enum class_id) : class_id(class_id) {}

	bool operator () (resource_base * resource) const
	{
		return		resource->get_class_id() == class_id;
	}

	class_id_enum	class_id;
};

struct release_unconditionaly
{
	bool operator () (resource_base * ) const { return true; }
};

bool   releasing_functionality::release_resources_by_class_id (class_id_enum class_id)
{
	bool out_all_released					=	true;
	release_by_class_id_predicate	predicate(class_id);
	for ( memory_type * it_memory		=	m_data.memory_types.front();
						it_memory;
						it_memory		=	m_data.memory_types.get_next_of_object(it_memory) )
	{
		release_resources_of_memory_type	(it_memory, predicate);
		if ( !it_memory->resources.empty() )
			out_all_released				=	false;
	}

	return										out_all_released;
}

bool   releasing_functionality::release_all_resources ()
{
	bool out_all_released				=	true;
	release_unconditionaly	predicate;
	for ( memory_type * it_memory		=	m_data.memory_types.front();
						it_memory;
						it_memory		=	m_data.memory_types.get_next_of_object(it_memory) )
	{
		release_resources_of_memory_type		(it_memory, predicate);
		if ( !it_memory->resources.empty() )
			out_all_released				=	false;
	}

	return										out_all_released;
}

} // namespace resources
} // namespace xray
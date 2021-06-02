////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resman_test_resource_with_quality.h"
#include "game_resman_test_resource_with_quality_cook.h"

namespace xray {
namespace resources {

test_resource_with_quality_cook::test_resource_with_quality_cook () 
: translate_query_cook						   (test_resource_class, 
												cook_base::reuse_true, 
												use_any_thread_id) 
{
}

void   test_resource_with_quality_cook::translate_query (query_result_for_cook & parent)
{
	ASSERT										(parent.is_autoselect_quality_query());

	test_resource_with_quality * resource	=	MT_NEW(test_resource_with_quality)(parent.get_requested_path());

	u32 const target_quality_level			=	parent.target_quality_level();
	ASSERT										(target_quality_level != u32(-1));
	parent.set_unmanaged_resource				(resource, nocache_memory, sizeof(resource));
	resource->increase_quality					(target_quality_level, parent.target_satisfaction(), & parent);
	// increase_quality will call finish_query for parent
}

void   test_resource_with_quality_cook::delete_resource	(resource_base * resource)
{
	MT_DELETE									(resource);
}

u32	  test_resource_with_quality_cook::calculate_quality_levels_count (query_result_for_cook const *) 
{
	return										test_resource_with_quality::max_quality_levels_count; 
}

float   test_resource_with_quality_cook::satisfaction_with (u32 quality_level, math::float4x4 const & user_matrix, u32 users_count)
{
	XRAY_UNREFERENCED_PARAMETERS				( &user_matrix, users_count );

	if ( quality_level == u32(-1) )
		return									0;
	u32 const max_levels_count				=	test_resource_with_quality::max_quality_levels_count;
	return										(float)(max_levels_count - quality_level) / max_levels_count;
}

} // namespace resources
} // namespace xray
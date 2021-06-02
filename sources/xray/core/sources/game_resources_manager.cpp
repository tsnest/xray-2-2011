////////////////////////////////////////////////////////////////////////////
//	Created		: 27.05.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "game_resources_manager.h"
#include <xray/resources_callbacks.h>
#include "fs_file_system.h"
#include "resources_macros.h"

namespace xray {
namespace resources {

bool   resource_base_compare::operator () (resource_base const & left, resource_base const & right) const
{
	if ( left.get_reference_count() < right.get_reference_count() )
		return							true;

	if ( left.memory_usage().size < right.memory_usage().size )
		return							true;

	return								false;
}

bool   game_resources_manager::memory_type_and_pool_compare::operator () (memory_type_and_pool const & left, memory_type_and_pool const & right) const
{
		 if ( (size_t)left.type < (size_t)right.type )	return	true;
	else if ( (size_t)left.type > (size_t)right.type )	return	false;

		 if ( left.pool < right.pool )					return	true;
	else if ( left.pool > right.pool )					return	false;

	return														false;
}

void   game_resources_manager::query_finished_callback (resource_base * resource)
{
	memory_usage const & memory_usage	=	resource->memory_usage();
	if ( memory_usage.type == & memory_type_non_cacheable_resource )
		return;

	resource->cast_base_of_intrusive_base()->pin_reference_count_for_game_resource_manager	();
	LOGI_INFO								("game_resman", "taken control of: %s", 
											 resource->cast_managed_resource() ? 
											 resource->cast_managed_resource()->log_string().c_str() : 
											 resource->cast_unmanaged_resource()->log_string().c_str() );

	memory_type_and_pool type_and_pool		(memory_usage.type, memory_usage.pool);
	resources_info							info;
	trees_type::iterator it				=	m_trees.find(type_and_pool);
	if ( it == m_trees.end() )
	{
		resources_info						info;
		info.tree						=	RES_NEW(resource_tree_type);
		m_trees.insert						(std::make_pair(type_and_pool, info));
	}
	else
		info							=	it->second;

	info.tree->insert						(* resource);
}

bool   game_resources_manager::try_free (resource_base * resource)
{
	if ( managed_resource * managed_resource = resource->cast_managed_resource() )
	{
		if ( !resource->cast_base_of_intrusive_base()->try_unregister_from_fat_or_from_name_registry(managed_resource, 1) )
			return						false;
	}
	else if ( unmanaged_resource * unmanaged_resource = resource->cast_unmanaged_resource() )
	{
		if ( !resource->cast_base_of_intrusive_base()->try_unregister_from_fat_or_from_name_registry(unmanaged_resource, 1) )
			return						false;
	}

	R_ASSERT	(resource->cast_base_of_intrusive_base()->reference_count() == 1);

	memory_usage const & memory_usage	=	resource->memory_usage();
	ASSERT									(memory_usage.type != & memory_type_non_cacheable_resource);
	memory_type_and_pool					type_and_pool(memory_usage.type, memory_usage.pool);

	trees_type::iterator tree_it		=	m_trees.find(type_and_pool);
	CURE_ASSERT								(tree_it != m_trees.end(), return false);
	resources_info info					=	tree_it->second;

	resource_tree_type::iterator resource_it	=	info.tree->find(* resource);
	info.tree->erase						(resource_it);

	resource->cast_base_of_intrusive_base()->unpin_reference_count_for_game_resource_manager	();

	LOGI_INFO							("game_resman", "lost control of: %s", 
										 resource->cast_managed_resource() ? 
										 resource->cast_managed_resource()->log_string().c_str() : 
										 resource->cast_unmanaged_resource()->log_string().c_str() );

	R_ASSERT							(!resource->cast_base_of_intrusive_base()->reference_count());
	
	if ( managed_resource * managed_resource = resource->cast_managed_resource() )
		managed_resource->destroy		(managed_resource);
	else if ( unmanaged_resource * unmanaged_resource = resource->cast_unmanaged_resource() )
		unmanaged_resource->destroy		(unmanaged_resource);

	return								true;
}

out_of_memory_treatment_enum   game_resources_manager::out_of_memory_callback (memory_usage const & memory_needed)
{
	XRAY_UNREFERENCED_PARAMETER			(memory_needed);



// 	u32 freed_currently				=	0;
// 	resource_base * it				=	list.front();
// 	while ( it )
// 	{
// 		resource_base * const next	=	list.get_next_of_object(it);
// 
// 		if ( it->cast_base_of_intrusive_base()->reference_count() == 1 )
// 		{
// 			memory_usage amount	=	it->memory_usage_self();
// 			if ( try_free(it) )
// 				freed_currently		+=	amount.usage_of_type(type);
// 
// 			if ( freed_currently >= memory_needed )
// 				return					free_memory_result_freed_all_needed;
// 		}
// 		
// 		it							=	next;
// 	}
// 
// 	if ( !freed_currently )
// 		return							free_memory_result_freed_nothing;

	return								out_of_memory_treatment_failed;
}

void   game_resources_manager::on_resource_leaked_callback (resource_base * resource)
{
	R_ASSERT					(resource->cast_base_of_intrusive_base()->reference_count() == 1);
	try_free					(resource);
}

static uninitialized_reference<game_resources_manager>		s_game_resources_manager;

void   initialize_game_resources_manager	()
{
	XRAY_CONSTRUCT_REFERENCE			(s_game_resources_manager, game_resources_manager);

	set_query_finished_callback			(boost::bind(& game_resources_manager::query_finished_callback, 
													   s_game_resources_manager.c_ptr(), _1));

	set_out_of_memory_callback			(boost::bind(& game_resources_manager::out_of_memory_callback, 
													   s_game_resources_manager.c_ptr(), _1));

	fs::set_on_resource_leaked_callback	(boost::bind(& game_resources_manager::on_resource_leaked_callback, 
													   s_game_resources_manager.c_ptr(), _1));
}

void   finilize_game_resources_manager		()
{
	XRAY_DESTROY_REFERENCE				(s_game_resources_manager);
}

} // namespace resources
} // namespace xray

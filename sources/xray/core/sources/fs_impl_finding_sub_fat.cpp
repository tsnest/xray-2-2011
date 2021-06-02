////////////////////////////////////////////////////////////////////////////
//	Created		: 03.12.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "fs_impl.h"
#include "fs_impl_finding.h"
#include "resources_helper.h"

namespace xray {
namespace fs {

using namespace resources;

void   file_system_impl::mount_sub_fat_resource (sub_fat_resource_ptr sub_fat)
{
	R_ASSERT									(sub_fat);
	if ( sub_fat->is_mounted )
		return;

	mount_sub_fat_resource_impl					(sub_fat.c_ptr());

	find_by_db_path	find_predicate				(sub_fat->archive_physical_path.c_str(), NULL);
	sub_fat_resource_ptr parent_db			=	m_root_dbs.find_if(find_predicate);
	R_ASSERT									(parent_db);
	sub_fat->parent_db						=	parent_db;
	sub_fat->is_mounted						=	true;
	LOGI_INFO									("fs", "mounted sub-fat '%s'", sub_fat->virtual_path.c_str());
}

void   file_system_impl::on_sub_fat_loaded (resources::queries_result &	results,
										    pcstr						path_to_find,
											enum_flags<find_enum>		find_flags,
											find_results_struct *		find_results)
{
	R_ASSERT									(results.is_successful());
	sub_fat_resource_ptr sub_fat			=	static_cast_resource_ptr<sub_fat_resource_ptr>
												(results[0].get_unmanaged_resource());
	ASSERT										(sub_fat);

	find_results->sub_fat					=	sub_fat;
	mount_sub_fat_resource						(sub_fat);	

	find_async									(find_results, path_to_find, find_flags);
}

void   file_system_impl::find_async_query_sub_fat (find_env & env)
{
	R_ASSERT									(env.node->is_sub_fat());

	path_string									db_fat_full_path;
	env.node->get_full_path						(db_fat_full_path);
	resources::query_resource					(db_fat_full_path.c_str(), 
												 resources::sub_fat_class, 
												 boost::bind( &file_system_impl::on_sub_fat_loaded, this, _1,
															   env.path_to_find, 
															   env.find_flags,
															   env.find_results),
												 & memory::g_resources_helper_allocator);
}

} // namespace fs
} // namespace xray 
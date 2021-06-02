////////////////////////////////////////////////////////////////////////////
//	Created		: 16.09.2010
//	Author		: Nikolay Partas
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "material_cook.h"
#include "material.h"

namespace xray {
namespace render {

material_cook::material_cook( )
	: resources::translate_query_cook(  resources::material_class, 
										#ifdef MASTER_GOLD
											reuse_true, 
										#else // #ifdef MASTER_GOLD
											reuse_false, 
										#endif // #ifdef MASTER_GOLD
										use_current_thread_id ) 
{
}

void material_cook::translate_query( resources::query_result_for_cook & parent )
{
	if ( parent.user_data() )
	{
		configs::binary_config_ptr			cfg;
		parent.user_data()->try_get			(cfg);
		on_material_binary_config_loaded	( &parent, cfg.c_ptr() );
	}
	else
	{
		fs_new::virtual_path_string			path;
		path.assignf						("resources/material_instances/%s.material", parent.get_requested_path());
		
		resources::query_vfs_iterator		(
			path.c_str(), 
			boost::bind( & material_cook::on_fs_iterator_ready, this, _1, & parent), 
			g_allocator, 
			resources::recursive_false,
			& parent
		);
	}
}

void material_cook::on_fs_iterator_ready	(vfs::vfs_locked_iterator const & it, resources::query_result_for_cook * parent)
{
	fs_new::virtual_path_string				path;
	if ( it )
		path.assignf						("resources/material_instances/%s.material", parent->get_requested_path());
	else
		path.assignf						("resources/materials/%s.material", parent->get_requested_path());

	query_resource							( 
		path.c_str(), 
		resources::binary_config_class, 
		boost::bind( &material_cook::on_material_config_loaded, this, _1), 
		g_allocator,
		NULL, 
		parent
	);
}

void material_cook::on_material_config_loaded(resources::queries_result& result)
{
	resources::query_result_for_cook* const parent = result.get_parent_query();
	
	if(!result.is_successful())
	{
		parent->finish_query(result_error, assert_on_fail_false);
		return;
	}

	configs::binary_config* cfg			= static_cast_checked<configs::binary_config*>(result[0].get_unmanaged_resource().c_ptr());
	on_material_binary_config_loaded	( parent, cfg );
}

void material_cook::on_material_binary_config_loaded( resources::query_result_for_cook* parent, configs::binary_config* cfg)
{
	material* created_resource			= NEW(material)( cfg );
	created_resource->m_material_name	= parent->get_requested_path();
	
	parent->set_unmanaged_resource		( created_resource, resources::nocache_memory, sizeof(material) );
	parent->finish_query				( result_success );

}

void material_cook::delete_resource ( resources::resource_base* resource )
{
	material* mtl = static_cast_checked<material*>(resource);
	DELETE(mtl);
}

} // namespace render
} // namespace xray

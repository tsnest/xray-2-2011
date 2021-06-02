////////////////////////////////////////////////////////////////////////////
//	Created		: 26.10.2010
//	Author		: Armen Abroyan
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "terrain_cook.h"
#include "terrain.h"
#include "model_manager.h"
#include <xray/render/engine/model_format.h>
#include "scene.h"
#include <xray/render/facade/terrain_base.h>
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>


namespace xray {
namespace render {

terrain_mesh_cook::terrain_mesh_cook( )
:super( resources::terrain_mesh_class, reuse_true, use_current_thread_id, 0, flag_create_allocates_destroy_deallocates )
{}

void terrain_mesh_cook::create_resource	(	resources::query_result_for_cook&	in_out_query, 
											const_buffer						raw_file_data, 
											mutable_buffer						in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER		(in_out_unmanaged_resource_buffer);

	fs_new::virtual_path_string	cell_name;
	pcstr req_path				= in_out_query.get_requested_path();
	xray::fs_new::file_name_with_no_extension_from_path	(&cell_name, req_path);

	terrain_render_model* tvisual	= static_cast_checked<terrain_render_model*>(model_factory::create_render_model( mt_terrain_cell ));

	memory::chunk_reader		F((pbyte)raw_file_data.c_ptr(), raw_file_data.size(), memory::chunk_reader::chunk_type_sequential );

	terrain_cook_user_data pterrain_prjstr_pair;
	if( !in_out_query.user_data()->try_get( pterrain_prjstr_pair))
		ASSERT(0);

	typedef resources::resource_ptr< scene, resources::unmanaged_resource >	scene_ptr;

	render::terrain* terrain = static_cast_checked<render::terrain*>(static_cast_resource_ptr<scene_ptr>(pterrain_prjstr_pair.scene)->terrain());

	tvisual->load				( F, *terrain );

	tvisual->m_name				= cell_name.c_str();

	in_out_query.set_unmanaged_resource	(tvisual, resources::nocache_memory, sizeof(terrain_render_model));
	in_out_query.finish_query			(result_success);
}

void terrain_mesh_cook::destroy_resource (resources::unmanaged_resource* const res)
{
	terrain_render_model* mesh			= static_cast_checked<terrain_render_model*>(res);
	model_factory::destroy_render_model	( mesh );
}

void terrain_mesh_cook::create_resource_if_no_file(resources::query_result_for_cook &	in_out_query, 
												   mutable_buffer						in_out_unmanaged_resource_buffer)
{
	XRAY_UNREFERENCED_PARAMETER		(in_out_unmanaged_resource_buffer);

	pcstr requested_path			= in_out_query.get_requested_path();
	pcstr cell_name					= fs_new::file_name_from_path( fs_new::virtual_path_string(requested_path) );
	fs_new::virtual_path_string req_path;

	resources::user_data_variant* v	= in_out_query.user_data();
	if ( !v )
	{
		in_out_query.finish_query	(result_error);
		return;
	}

	terrain_cook_user_data	user_data;
	if ( !v->try_get(user_data) )
	{
		in_out_query.finish_query	(result_error);
		return;
	}

	req_path						= "resources/projects/";
	req_path						+= user_data.current_project_resource_path;
	req_path						+= "/terrain/";
	req_path						+= cell_name;

	in_out_query.finish_query		( result_postponed );

	query_resource(
			req_path.c_str(),
			resources::raw_data_class,
			boost::bind(&terrain_mesh_cook::raw_data_loaded, this, _1),
			in_out_query.get_user_allocator(),
			NULL,
			&in_out_query
		);

}

void terrain_mesh_cook::raw_data_loaded( resources::queries_result & raw_file_results)
{
	R_ASSERT						(raw_file_results.is_successful());

	resources::pinned_ptr_const<u8>	ptr(raw_file_results[0].get_managed_resource());

	const_buffer	raw_file_data	(ptr.c_ptr(), ptr.size());
	
	create_resource					(* raw_file_results.get_parent_query(), raw_file_data, mutable_buffer::zero());
}


terrain_instance_cook::terrain_instance_cook( )
: super( resources::terrain_instance_class, reuse_false, use_current_thread_id ) 
{}

void terrain_instance_cook::translate_query( resources::query_result_for_cook&	parent )
{
	if(parent.creation_data_from_user())
	{
		resources::query_create_resource(
			parent.get_requested_path(),
			parent.creation_data_from_user(),
			resources::terrain_mesh_class,
			boost::bind	(&terrain_instance_cook::on_subresources_loaded, this, _1, &parent ),
			xray::render::g_allocator,
			parent.user_data(),
			& parent
			);
	}else
	{
		resources::query_resource(
			parent.get_requested_path(),
			resources::terrain_mesh_class,
			boost::bind	(&terrain_instance_cook::on_subresources_loaded, this, _1, &parent ),
			xray::render::g_allocator,
			parent.user_data(),
			& parent
			);
	}
}

void terrain_instance_cook::on_subresources_loaded( resources::queries_result& data, 
													resources::query_result_for_cook* parent_query )
{
	R_ASSERT						(data.is_successful());

	render::terrain_render_model_instance* render_model_terrain_instance	= NEW(render::terrain_render_model_instance)();
	render_model_terrain_instance->m_terrain_model			= static_cast_resource_ptr<terrain_render_model_ptr>(data[0].get_unmanaged_resource());
	//?
	render_model_terrain_instance->set_transform			( render_model_terrain_instance->m_terrain_model->transform() );
	parent_query->set_unmanaged_resource	( render_model_terrain_instance, resources::nocache_memory, sizeof(render_model_terrain_instance));
	parent_query->finish_query				( result_success );
}

void terrain_instance_cook::delete_resource( resources::resource_base* resource )
{
	DELETE		( resource );
}

terrain_model_cook::terrain_model_cook( )
: super( resources::terrain_model_class, reuse_false, use_current_thread_id ) 
{}

void terrain_model_cook::translate_query( resources::query_result_for_cook&	parent )
{
	if(parent.creation_data_from_user())
	{
		resources::query_create_resource(
			parent.get_requested_path(),
			parent.creation_data_from_user(),
			resources::terrain_instance_class,
			boost::bind	(&terrain_model_cook::on_subresources_loaded, this, _1, &parent ),
			xray::render::g_allocator,
			parent.user_data(),
			& parent
			);
	}else
	{
		resources::query_resource(
			parent.get_requested_path(),
			resources::terrain_instance_class,
			boost::bind	(&terrain_model_cook::on_subresources_loaded, this, _1, &parent ),
			xray::render::g_allocator,
			parent.user_data(),
			& parent
			);
	}
}

void terrain_model_cook::on_subresources_loaded( resources::queries_result& data, 
													resources::query_result_for_cook* parent_query )
{
	R_ASSERT						(data.is_successful());

	render::terrain_model_instance* model	= NEW(render::terrain_model_instance)();
	terrain_render_model_instance* instance	= static_cast_checked<terrain_render_model_instance*>(data[0].get_unmanaged_resource().c_ptr());
	model->m_render_model					= instance;
	model->m_collision_geom					= instance->m_terrain_model->collision_geom();
	model->m_bt_collision_shape				= instance->m_terrain_model->bt_collision_shape();
	parent_query->set_unmanaged_resource	( model, resources::nocache_memory, sizeof(render::terrain_model_instance));
	parent_query->finish_query				( result_success );
}

void terrain_model_cook::delete_resource( resources::resource_base* resource )
{
	DELETE			( resource );
}


} // namespace render
} // namespace xray

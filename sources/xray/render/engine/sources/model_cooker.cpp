////////////////////////////////////////////////////////////////////////////
//	Created		: 05.08.2010
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "model_cooker.h"
#include "render_model.h"
#include "material.h"
#include "render_model_skeleton.h"
#include <xray/render/core/res_effect.h>
#include <xray/render/core/res_geometry.h>
#include <xray/render/core/custom_config.h>
#include <xray/render/core/untyped_buffer.h>

namespace xray {
namespace render {



static_model_instance_cook::static_model_instance_cook( )
: super( resources::static_model_instance_class, reuse_false, use_current_thread_id ) 
{}


void static_model_instance_cook::translate_query( resources::query_result_for_cook&	parent )
{
	fs_new::virtual_path_string			composite_path;
	composite_path.assignf("resources/composite_models/%s.composite_model", parent.get_requested_path() );

	fs_new::physical_path_info const &	composite_path_info	=	resources::get_physical_path_info( composite_path, resources::sources_mount );

	fs_new::virtual_path_string			collision_path;
	fs_new::virtual_path_string			render_path;

	resources::class_id_enum needed_class_id;
	if ( composite_path_info.is_folder() )
	{
		render_path.assignf		( "resources/composite_models/%s.composite_model/composite_render", parent.get_requested_path() );
		collision_path.assignf	( "resources/composite_models/%s.composite_model/collision", parent.get_requested_path() );
		needed_class_id			= resources::composite_render_model_instance_class;
	}else
	{
		render_path.assignf		( "resources/models/%s.model/render", parent.get_requested_path() );
		collision_path.assignf	( "resources/models/%s.model/collision", parent.get_requested_path() );
		needed_class_id			= resources::static_render_model_instance_class;
	}
	resources::request r[]={
		{render_path.c_str(), needed_class_id},
		{collision_path.c_str(), resources::collision_geometry_class},
	};
	resources::query_resources(
		r, 
		boost::bind(&static_model_instance_cook::on_subresources_loaded, this, _1, &parent ),
		g_allocator,
		NULL,
		&parent
		);
}

void static_model_instance_cook::on_subresources_loaded( resources::queries_result& data, resources::query_result_for_cook*	parent_query )
{
	if( data.is_successful() )
	{
		render::static_model_instance* created_resource	= NEW(render::static_model_instance)();
		
		created_resource->m_render_model		= static_cast_resource_ptr<render::render_model_instance_ptr>(data[0].get_unmanaged_resource());
		created_resource->m_collision_geom		= static_cast_resource_ptr<collision::geometry_ptr>(data[1].get_unmanaged_resource());
		parent_query->set_unmanaged_resource	( created_resource, resources::nocache_memory, sizeof(render::static_model_instance) );
		parent_query->finish_query				( result_success );
	}else
		parent_query->finish_query				( result_error );
	
}

void static_model_instance_cook::delete_resource( resources::resource_base* resource )
{
	DELETE( resource );
}

skeleton_model_instance_cook::skeleton_model_instance_cook( )
:super( resources::skeleton_model_instance_class, reuse_false, use_current_thread_id )
{}

void skeleton_model_instance_cook::translate_query( resources::query_result_for_cook& parent )
{
	skeleton_model_instance_cook_data* cook_data = NEW(skeleton_model_instance_cook_data)(&parent);

	fs_new::virtual_path_string	skeleton_config_path;
	fs_new::virtual_path_string	render_model_path;
	skeleton_config_path.assignf	( "resources/models/%s.skinned_model/skeleton", parent.get_requested_path() );
	render_model_path.assignf		( "resources/models/%s.skinned_model/render", parent.get_requested_path() );

	resources::query_resource(
		skeleton_config_path.c_str(),
		resources::binary_config_class,
		boost::bind( &skeleton_model_instance_cook::on_skeleton_config_loaded, this, _1, cook_data ),
		xray::render::g_allocator,
		0,
		& parent
	);
	
	resources::query_resource(
		render_model_path.c_str(),
		resources::skeleton_render_model_instance_class,
		boost::bind	(&skeleton_model_instance_cook::on_render_model_loaded, this, _1, cook_data ),
		xray::render::g_allocator,
		0,
		& parent
	);
}

void skeleton_model_instance_cook::on_skeleton_config_loaded( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data )
{
	resources::query_result_for_cook* parent_query = cook_data->parent_query;
	if (!result.is_successful())
	{
		parent_query->finish_query				( result_error );
		return;
	}
	
	configs::binary_config_ptr config = static_cast_resource_ptr<configs::binary_config_ptr>(result[0].get_unmanaged_resource());
	fs_new::virtual_path_string path = 			 pcstr(config->get_root()["skeleton"]);
	
	resources::query_resource(
		path.c_str(),
		resources::skeleton_class,
		boost::bind	(&skeleton_model_instance_cook::on_skeleton_loaded, this, _1, cook_data ),
		xray::render::g_allocator,
		0,
		cook_data->parent_query
	);
}

void skeleton_model_instance_cook::on_skeleton_loaded( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data )
{
	if (!result.is_successful())
	{
		resources::query_result_for_cook* parent_query = cook_data->parent_query;
		parent_query->finish_query				( result_error );
		return;
	}
	
	cook_data->skeleton = static_cast_resource_ptr<animation::skeleton_ptr>(result[0].get_unmanaged_resource());

	cook_data->skeleton_ready = true;

	if (cook_data->render_model_ready)
		on_all_subresources_ready(cook_data);	
}

void skeleton_model_instance_cook::on_render_model_loaded( resources::queries_result& result, skeleton_model_instance_cook_data* cook_data )
{
	if (!result.is_successful())
	{
		resources::query_result_for_cook* parent_query = cook_data->parent_query;
		parent_query->finish_query ( result_error );
		return;
	}

	cook_data->render_model = static_cast_resource_ptr<render::render_model_instance_ptr>(result[0].get_unmanaged_resource());
	
	cook_data->render_model_ready = true;

	if (cook_data->skeleton_ready)
		on_all_subresources_ready( cook_data );
}

void skeleton_model_instance_cook::on_all_subresources_ready( skeleton_model_instance_cook_data* cook_data )
{
	resources::query_result_for_cook* parent_query = cook_data->parent_query;
	
	render::skeleton_model_instance* created_resource= NEW(render::skeleton_model_instance)();
	
	created_resource->m_render_model = cook_data->render_model;
	created_resource->m_skeleton	 = cook_data->skeleton;
		
	skeleton_render_model_instance* skel_mesh = static_cast_checked<skeleton_render_model_instance*>(created_resource->m_render_model.c_ptr());
		
	skel_mesh->m_bones_matrices.resize	( skel_mesh->m_original->m_inverted_bones_matrices_in_bind_pose.size() );
		
	for (u32 i=0; i<skel_mesh->m_bones_matrices.size(); ++i)
		skel_mesh->m_bones_matrices[i] = math::float4x4().identity();
		
	parent_query->set_unmanaged_resource	( created_resource, resources::nocache_memory, sizeof(render::skeleton_model_instance) );
	parent_query->finish_query				( result_success );
	
	DELETE(cook_data);
}

void skeleton_model_instance_cook::delete_resource( resources::resource_base* resource )
{
	DELETE( resource );
}


} // namespace render
} // namespace xray

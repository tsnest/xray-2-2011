////////////////////////////////////////////////////////////////////////////
//	Created		: 12.09.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_shape_cook.h"
#include "bullet_include.h"
#include <xray/physics/collision_shapes.h>
#include <xray/render/engine/model_format.h>

namespace xray {
namespace physics {


collision_shape_cook::collision_shape_cook( bool static_object )
:super			( static_object ? resources::collision_bt_shape_class_static : resources::collision_bt_shape_class_dynamic, reuse_true, use_current_thread_id ),
m_static_object	( static_object )
{
	resources::register_cook(this);
}

void collision_shape_cook::translate_query( resources::query_result_for_cook&	parent )
{
	fs_new::virtual_path_string req_path_primitives		= parent.get_requested_path();
	req_path_primitives.append				("/primitives");

	resources::query_resource(
		req_path_primitives.c_str(),
		resources::binary_config_class,
		boost::bind	(&collision_shape_cook::on_primitives_collision_loaded, this, _1, &parent ),
		physics::g_ph_allocator,
		0,
		& parent
	);
}


void collision_shape_cook::on_primitives_collision_loaded( resources::queries_result& data, 
										  resources::query_result_for_cook* parent )
{
	if (!data.is_successful())
	{
		if(data[0].get_error_type()==resources::query_result_for_user::error_type_file_not_found)
		{
			fs_new::virtual_path_string	vertices_path = parent->get_requested_path();
			fs_new::virtual_path_string	indices_path  = parent->get_requested_path();
			
			vertices_path.append("/vertices");
			indices_path.append ("/indices");
			
			resources::request requests[] = {
				{ vertices_path.c_str(), resources::raw_data_class },
				{ indices_path.c_str(),  resources::raw_data_class }
			};
			
			resources::query_resources(
				requests,
				array_size(requests),
				boost::bind	(&collision_shape_cook::on_triangle_mesh_collision_loaded, this, _1, parent ),
				physics::g_ph_allocator,
				0,
				parent
			);
		
			return;
		}

		parent->finish_query( data[0].get_error_type() );
		return;
	}

	configs::binary_config_ptr cfg = static_cast_resource_ptr<configs::binary_config_ptr>(data[0].get_unmanaged_resource());

	configs::binary_config_value root	= cfg->get_root();
	bt_collision_shape*					result = NULL;

	bool single_shape = root["composite"].size()>1;
	if(single_shape)
	{
		float3 const p = root["composite"][0]["position"];
		single_shape = p.is_similar(float3(0,0,0));
	}

	if(!single_shape)
	{
		result = create_compound_shape( *g_ph_allocator, root["composite"] );
	}else
	{// single primitive shape
		configs::binary_config_value cfg = root["composite"][0];
		xray::collision::primitive_type type = (xray::collision::primitive_type)(int)cfg["type"];
		result = create_primitive_shape		( *g_ph_allocator, type, (float3)cfg["scale"] );
	}
	
	
	parent->set_unmanaged_resource	( result, resources::nocache_memory, sizeof(bt_collision_shape) );
	parent->finish_query			( result_success );
}

void collision_shape_cook::on_triangle_mesh_collision_loaded( resources::queries_result& data, 
										  resources::query_result_for_cook* parent )
{
	if (!data.is_successful())
	{
		parent->finish_query( data[0].is_successful() ?  
								data[1].get_error_type() : 
								data[0].get_error_type() 
							);
		return;
	}
	
	resources::pinned_ptr_const<u8> vertices_ptr(data[0].get_managed_resource( ));
	resources::pinned_ptr_const<u8> indices_ptr(data[1].get_managed_resource( ));
	
	memory::chunk_reader vertices_chunk_reader(vertices_ptr.c_ptr(), vertices_ptr.size(), memory::chunk_reader::chunk_type_sequential);
	memory::chunk_reader indices_chunk_reader(indices_ptr.c_ptr(), indices_ptr.size(), memory::chunk_reader::chunk_type_sequential);
	
	memory::reader vertices_reader = vertices_chunk_reader.open_reader(render::model_chunk_collision_v);
	memory::reader indices_reader  = indices_chunk_reader.open_reader(render::model_chunk_collision_i);
	
	bt_collision_shape* result = NULL;
	if(m_static_object)
	{
		result = create_static_triangle_mesh_shape( *g_ph_allocator,
													(float3*)vertices_reader.pointer(),
													(u32*)indices_reader.pointer(),
													vertices_reader.length() / sizeof(float3), 
													indices_reader.length() / sizeof(u32) );
	}else
	{
		result = create_dynamic_triangle_mesh_shape( *g_ph_allocator,
													(float3*)vertices_reader.pointer(),
													(u32*)indices_reader.pointer(),
													vertices_reader.length() / sizeof(float3), 
													indices_reader.length() / sizeof(u32) );
	}

	parent->set_unmanaged_resource	( result, 
									resources::nocache_memory, 
									sizeof(bt_collision_shape) );

	parent->finish_query			( result_success );
}

void collision_shape_cook::delete_resource( resources::resource_base* resource )
{
	bt_collision_shape* s = static_cast_checked<bt_collision_shape*>(resource);
	destroy_shape			( *g_ph_allocator, s );
}


} //namespace xray
} //namespace physics

////////////////////////////////////////////////////////////////////////////
//	Created		: 04.07.2011
//	Author		: Andrew Kolomiets
//	Copyright (C) GSC Game World - 2011
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "collision_cook.h"
#include <xray/render/engine/model_format.h>
#include <xray/collision/api.h>
#include <xray/collision/geometry.h>
#include <xray/collision/object.h>
#include "triangle_mesh_geometry.h"
#include "composite_geometry.h"
#include "composite_geometry_instance.h"
#include "triangle_mesh_geometry_instance.h"
#include <xray/resources_fs.h>

namespace xray{
namespace collision {

void initialize( )
{
	static collision_cook	collision_cooker;
	register_cook			( &collision_cooker );
}


collision_cook::collision_cook( )
: super( resources::collision_geometry_class, reuse_true, use_resource_manager_thread_id ) 
{
}

void collision_cook::translate_query( resources::query_result_for_cook&	parent )
{
	fs_new::virtual_path_string req_path_primitives		= parent.get_requested_path();
	req_path_primitives.append("/primitives");

	resources::query_resource(
		req_path_primitives.c_str(),
		resources::binary_config_class,
		boost::bind	(&collision_cook::on_primitives_collision_config_loaded, this, _1, &parent ),
		resources::unmanaged_allocator(), 
		0, 
		& parent, 
		assert_on_fail_false
	);
}

void collision_cook::query_triangle_mesh ( resources::query_result_for_cook * parent_query )
{
	fs_new::virtual_path_string	vertices_path = parent_query->get_requested_path();
	fs_new::virtual_path_string	indices_path  = parent_query->get_requested_path();
	
	vertices_path.append("/vertices");
	indices_path.append ("/indices");
	
	resources::request requests[] = {
		{ vertices_path.c_str(), resources::raw_data_class },
		{ indices_path.c_str(),  resources::raw_data_class }
	};

	resources::query_resources(
		requests,
		array_size(requests),
		boost::bind	(&collision_cook::on_triangle_mesh_collision_loaded, this, _1, parent_query ),
		resources::unmanaged_allocator(),
		NULL, 
		parent_query
	);
}

void collision_cook::on_primitives_collision_config_loaded( resources::queries_result& data, 
													 resources::query_result_for_cook* parent_query )
{
	if (!data.is_successful())
	{
		query_triangle_mesh			(parent_query);
		return;
	}
	configs::binary_config_ptr cfg = static_cast_resource_ptr<configs::binary_config_ptr>(data[0].get_unmanaged_resource());

	// todo: use collision::* new_composite_geometry_from_config() function insted of this temporary code

	configs::binary_config_value root					= cfg->get_root()["composite"];
	configs::binary_config_value::const_iterator it		= root.begin();
	configs::binary_config_value::const_iterator it_e	= root.end();
	
	u32 count											= root.size();
	pvoid buffer										= ALLOCA( count *sizeof(collision::geometry_instance*) );
	buffer_vector<collision::geometry_instance*>		instances(buffer, count);

	for(; it!=it_e; ++it)
	{
		int type		= (*it)["type"];
		float3 position	= (*it)["position"];
		float3 rotation	= (*it)["rotation"];
		float3 scale	= (*it)["scale"];
		switch(type)
		{
		case primitive_sphere:
			{// sphere
				non_null<collision::geometry_instance>::ptr instance= collision::new_sphere_geometry_instance( resources::unmanaged_allocator(), 
																										create_rotation(rotation)*create_translation(position), 
																										scale.x ); 
				instances.push_back									( &*instance );
			}break;
		case collision::primitive_box:
			{// 
				non_null<collision::geometry_instance>::ptr instance= collision::new_box_geometry_instance( resources::unmanaged_allocator(), 
																										create_scale(scale)*create_rotation(rotation)*create_translation(position) ); 
				instances.push_back									( &*instance );
			}break;
		case collision::primitive_cylinder:
			{// 
				non_null<collision::geometry_instance>::ptr instance= collision::new_cylinder_geometry_instance( resources::unmanaged_allocator(), 
																										create_rotation(rotation)*create_translation(position), 
																										scale.x,
																										scale.y );
				instances.push_back									( &*instance );
			}break;
		}
	}

	collision::geometry_ptr resource		= &*collision::new_composite_geometry( resources::unmanaged_allocator(), instances );

//	collision::geometry_instance_ptr resource	= &*collision::new_composite_geometry_instance( resources::unmanaged_allocator(), float4x4().identity(), composite_geom );

	parent_query->set_unmanaged_resource	( resource.c_ptr(), resources::nocache_memory, sizeof(collision::composite_geometry) );
	parent_query->finish_query				( result_success );
}

void collision_cook::on_triangle_mesh_collision_loaded( resources::queries_result& data, 
										  resources::query_result_for_cook* parent_query )
{
	if (!data.is_successful())
	{
		parent_query->finish_query( result_error );
		return;
	}
	
	resources::pinned_ptr_const<u8> vertices_ptr(data[0].get_managed_resource( ));
	resources::pinned_ptr_const<u8> indices_ptr(data[1].get_managed_resource( ));
	
	memory::chunk_reader vertices_chunk_reader(vertices_ptr.c_ptr(), vertices_ptr.size(), memory::chunk_reader::chunk_type_sequential),
						 indices_chunk_reader(indices_ptr.c_ptr(), indices_ptr.size(), memory::chunk_reader::chunk_type_sequential);
	
	memory::reader vertices_reader = vertices_chunk_reader.open_reader(render::model_chunk_collision_v);
	memory::reader indices_reader  = indices_chunk_reader.open_reader(render::model_chunk_collision_i);
	
	collision::geometry_ptr resource =
		&*collision::new_triangle_mesh_geometry
		( 
			resources::unmanaged_allocator(), 
			(xray::math::float3 const* const)vertices_reader.pointer(),
			vertices_reader.length() / sizeof(xray::math::float3), 
			(u32 const*)indices_reader.pointer(),
			indices_reader.length() / sizeof(u32)
		);
	
	//collision::geometry_instance_ptr resource	= &*collision::new_triangle_mesh_geometry_instance 
	//	(
	//		resources::unmanaged_allocator(),
	//		float4x4().identity(),
	//		mesh
	//	);

	parent_query->set_unmanaged_resource	( resource.c_ptr(), resources::nocache_memory, sizeof(collision::triangle_mesh_geometry) );
	parent_query->finish_query				( result_success );
}

void collision_cook::delete_resource( resources::resource_base* resource )
{
	collision::geometry* geom			= static_cast_checked<collision::geometry*>(resource);
//	geom->destroy						( resources::unmanaged_allocator() );
	collision::delete_geometry			( resources::unmanaged_allocator(), geom );
}

}
}

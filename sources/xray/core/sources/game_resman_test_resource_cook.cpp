////////////////////////////////////////////////////////////////////////////
//	Created		: 13.09.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include <xray/testing_macro.h>
#include "game_resman_test_resource.h"
#include "game_resman_test_allocator.h"
#include "game_resman_test_resource_cook.h"

namespace xray {
namespace resources {

test_resource_cook::test_resource_cook (class_id_enum	class_id,
									    u32				allocator_index,
									    bool			create_allocates_destroy_deallocates,
			 						    u32				create_thread_id, 
			 						    u32				allocate_thread_id)
	: unmanaged_cook							(class_id, 
												cook_base::reuse_true, 
												create_thread_id,													//	create_thread
												create_allocates_destroy_deallocates ? 
													0 : allocate_thread_id,	//	allocate_thread
												create_allocates_destroy_deallocates ? 
													flag_create_allocates_destroy_deallocates : flag_destroy_in_any_thread), 

	m_create_allocates_destroy_deallocates		(create_allocates_destroy_deallocates), 
	m_in_create_resource						(false),
	m_allocator_index							(allocator_index)
{

}

mutable_buffer	 test_resource_cook::allocate_resource (query_result_for_cook &	in_query, 
														const_buffer			raw_file_data, 
														bool					file_exist)
{
	if ( !m_in_create_resource )
		TEST_ASSERT							(!m_create_allocates_destroy_deallocates);

	TEST_ASSERT								(raw_file_data.size() && file_exist);
	TEST_ASSERT_CMP							(raw_file_data.size(), >=, sizeof(test_resource));
	
	u32 const resource_size				=	raw_file_data.size();
	memory::base_allocator * allocator	=	s_test_allocators[m_allocator_index].allocator();
	mutable_buffer const out_buffer			(XRAY_ALLOC_IMPL(allocator, char, resource_size), resource_size);
	
	if ( !out_buffer.c_ptr() )
	{
		in_query.set_out_of_memory			(s_test_allocators[m_allocator_index].memory_type, resource_size);
		return								mutable_buffer::zero();
	}

	return									out_buffer;
}

void   test_resource_cook::destroy_resource (unmanaged_resource * resource)
{
	resource->~unmanaged_resource			();
	if ( m_create_allocates_destroy_deallocates )
	{
		pvoid final_buffer				=	dynamic_cast<pvoid>(resource);
		XRAY_FREE_IMPL						(s_test_allocators[m_allocator_index].allocator(), final_buffer);
	}
}

void   test_resource_cook::deallocate_resource (pvoid buffer)
{
	TEST_ASSERT								(!m_create_allocates_destroy_deallocates);
	XRAY_FREE_IMPL							(s_test_allocators[m_allocator_index].allocator(), buffer);
}

void   test_resource_cook::create_resource	(query_result_for_cook &	in_out_query, 
										 	 const_buffer				raw_file_data,
										 	 mutable_buffer				in_out_unmanaged_resource_buffer)
{
	ASSERT										(!in_out_query.is_autoselect_quality_query());
	XRAY_UNREFERENCED_PARAMETER					(raw_file_data);
	test_resource * out_resource	=	NULL;
	if ( m_create_allocates_destroy_deallocates )
	{
		TEST_ASSERT								(!in_out_unmanaged_resource_buffer);
		m_in_create_resource				=	true;
		in_out_unmanaged_resource_buffer	=	allocate_resource(in_out_query, raw_file_data, true);
		m_in_create_resource				=	false;
		if ( !in_out_unmanaged_resource_buffer )
		{
			in_out_query.finish_query			(result_out_of_memory);
			return;
		}
	}
	
	out_resource							=	new (in_out_unmanaged_resource_buffer.c_ptr())	
												test_resource
												(m_allocator_index, in_out_query.get_requested_path(), 
												in_out_unmanaged_resource_buffer.size());

	in_out_query.set_unmanaged_resource			(out_resource, 
		memory_usage_type(s_test_allocators[m_allocator_index].memory_type, in_out_unmanaged_resource_buffer.size()));

	in_out_query.finish_query					(result_success);
}


} // namespace resources
} // namespace xray


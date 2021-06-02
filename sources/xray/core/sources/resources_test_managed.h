////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_MANAGED_H_INCLUDED
#define RESOURCES_TEST_MANAGED_H_INCLUDED

#include "resources_test_base_cook.h"
#include "resources_test_resource.h"

namespace xray {
namespace resources {

class test_managed_cook : public managed_cook, public test_cook_base
{
public:
	test_managed_cook ( reuse_enum	reuse_type, 
						bool		create_in_current_thread, 
						generate_if_no_file_test_enum generate_if_no_file_test) 
	: test_cook_base(generate_if_no_file_test), 
	managed_cook	(test_resource_class, reuse_type, create_in_current_thread ? use_current_thread_id : use_cook_thread_id) {}

	void	create_resource_if_no_file (query_result_for_cook &	in_out_query, 
			 							managed_resource_ptr	out_resource)
	{
		if ( generate_if_no_file_test() == generate_if_no_file_test_requery )
		{
			query_resource				("test/disk/generate_helper.txt", 
										 raw_data_class,  
										 boost::bind(&test_managed_cook::on_generate_helper_resource_loaded, this, _1),
										 & memory::g_mt_allocator,
										 NULL,
										 & in_out_query);

		
			in_out_query.finish_query	(result_postponed);
			return;
		}

		R_ASSERT						(generate_if_no_file_test() == generate_if_no_file_test_save_generated);

		mutable_buffer					generated_file_data(ALLOCA(generate_file_data_size), generate_file_data_size);
		fill_with_generated_file_data	(generated_file_data);

		result_enum const result	=	create_resource_impl(in_out_query, generated_file_data, out_resource);

		if ( !save_generated(in_out_query, generated_file_data) )
			return;

		in_out_query.finish_query		(result);
	}
	
	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() 
	{ 
		if ( generate_if_no_file_test() == generate_if_no_file_test_unset )
			return				NULL; 
		return							create_resource_if_no_file_delegate_type(this, & test_managed_cook::create_resource_if_no_file);
	}

	// raw_file_data can be NULL, when no file is found
	virtual	u32		calculate_resource_size		(const_buffer in_raw_file_data, bool file_exist)
	{
		R_ASSERT								(in_raw_file_data.size() || !file_exist);

		if ( !file_exist )
			return			sizeof(test_managed_resource) + generate_file_data_size;

		return				sizeof(test_managed_resource) + in_raw_file_data.size();
	}

	result_enum			create_resource_impl	(query_result_for_cook &	in_out_query, 
												 const_buffer				raw_file_data,
												 managed_resource_ptr		out_resource)
	{
		pinned_ptr_mutable<test_managed_resource> managed_typed_ptr	=	pin_for_write<test_managed_resource>(out_resource);

		TEST_ASSERT_CMP		(managed_typed_ptr.size(), ==, sizeof(test_managed_resource) + raw_file_data.size());
		mutable_buffer		dest_file_data((pbyte)managed_typed_ptr.c_ptr() + sizeof(test_managed_resource), 
											raw_file_data.size());
		memory::copy		(dest_file_data, raw_file_data);
		
		new (managed_typed_ptr.c_ptr()) test_managed_resource
										(in_out_query.get_requested_path(), 
										dest_file_data, 
										in_out_query.creation_data_from_user() ? creation_from_memory : creation_from_file);

		managed_typed_ptr->test_consistency	();
		return				result_success;
	}

	virtual void			create_resource	(query_result_for_cook &	in_out_query, 
											 const_buffer				raw_file_data,
											 managed_resource_ptr		out_resource)
	{
		result_enum	out_result		=	create_resource_impl(in_out_query, raw_file_data, out_resource);
		in_out_query.finish_query		(out_result);
	}

	virtual void   destroy_resource (resources::managed_resource * dying_resource) 
	{
		pinned_ptr_mutable<test_managed_resource> managed_typed_ptr	=	pin_for_write<test_managed_resource>(dying_resource);
		managed_typed_ptr.c_ptr()->~test_managed_resource				();
	}
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_MANAGED_H_INCLUDED
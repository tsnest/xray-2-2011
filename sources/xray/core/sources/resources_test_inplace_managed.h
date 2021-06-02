////////////////////////////////////////////////////////////////////////////
//	Created		: 22.04.2010
//	Author		: Sergey Chechin
//	Copyright (C) GSC Game World - 2010
////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCES_TEST_INPLACE_MANAGED_H_INCLUDED
#define RESOURCES_TEST_INPLACE_MANAGED_H_INCLUDED

#include "resources_test_base_cook.h"
#include "resources_test_resource.h"

namespace xray {
namespace resources {

class test_inplace_managed_cook : public inplace_managed_cook, public test_cook_base
{
public:
	enum { size_that_will_be_trunced = 32 };
public:
	test_inplace_managed_cook ( reuse_enum	reuse_type, 
								bool		create_in_current_thread, 
								generate_if_no_file_test_enum generate_if_no_file_test) 
	: test_cook_base(generate_if_no_file_test), 
	inplace_managed_cook	(test_resource_class, reuse_type, create_in_current_thread ? use_current_thread_id : use_cook_thread_id) {}

	void	create_resource_if_no_file (query_result_for_cook &	in_out_query, 
		 								managed_resource_ptr 	out_resource,
										u32 &					out_final_resource_size)
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

		// fill with generated file data
		pinned_ptr_mutable<test_managed_resource> managed_typed_ptr	=	pin_for_write<test_managed_resource>(out_resource);
		TEST_ASSERT_CMP					(managed_typed_ptr.size(), ==, sizeof(test_managed_resource) + generate_file_data_size + size_that_will_be_trunced);
		mutable_buffer					dest_file_data((pbyte)managed_typed_ptr.c_ptr() + sizeof(test_managed_resource), generate_file_data_size);
		memory::copy					(dest_file_data, generated_file_data);
		
		result_enum const result	=	create_resource_impl(in_out_query, out_resource, generate_file_data_size, out_final_resource_size);

		if ( !save_generated(in_out_query, generated_file_data) )
			return;

		in_out_query.finish_query		(result);
	}

	virtual create_resource_if_no_file_delegate_type	get_create_resource_if_no_file_delegate	() 
	{ 
		if ( generate_if_no_file_test() == generate_if_no_file_test_unset )
			return				NULL; 
		return							create_resource_if_no_file_delegate_type(this, & test_inplace_managed_cook::create_resource_if_no_file);
	}

	virtual	u32		calculate_resource_size		(u32 file_size, u32 & out_offset_to_file, bool file_exist) 
	{ 
		R_ASSERT								(file_size || !file_exist);

		out_offset_to_file		=	sizeof(test_managed_resource);
		if ( !file_exist )
			return					sizeof(test_managed_resource) + generate_file_data_size + size_that_will_be_trunced;

		return						sizeof(test_managed_resource) + file_size  + size_that_will_be_trunced;
	}


	result_enum			create_resource_impl	(query_result_for_cook &	in_out_query,
												 managed_resource_ptr		in_out_resource,
												 u32						raw_file_size,
												 u32 &						out_final_resource_size)
	{
		pinned_ptr_mutable<test_managed_resource> managed_typed_ptr	=	pin_for_write<test_managed_resource>(in_out_resource);

		TEST_ASSERT_CMP		(managed_typed_ptr.size(), ==, sizeof(test_managed_resource) + raw_file_size + size_that_will_be_trunced);
		mutable_buffer		file_data((pbyte)managed_typed_ptr.c_ptr() + sizeof(test_managed_resource), raw_file_size);
		
		new (managed_typed_ptr.c_ptr()) test_managed_resource
										(in_out_query.get_requested_path(), 
										 file_data, 
										 in_out_query.creation_data_from_user() ? creation_from_memory : creation_from_file);

		managed_typed_ptr->test_consistency	();
		out_final_resource_size	=	sizeof(test_managed_resource) + raw_file_size;
		return						result_success;
	}

	virtual void		create_resource		(query_result_for_cook &	in_out_query,
											 managed_resource_ptr		in_out_resource,
											 u32						raw_file_size,
											 u32 &						out_final_resource_size)
	{
		result_enum const out_result	=	create_resource_impl(in_out_query, in_out_resource, raw_file_size, out_final_resource_size);
		in_out_query.finish_query			(out_result);
	}

	virtual void   destroy_resource (resources::managed_resource * dying_resource) 
	{
		pinned_ptr_mutable<test_managed_resource> managed_typed_ptr	=	pin_for_write<test_managed_resource>(dying_resource);
		managed_typed_ptr.c_ptr()->~test_managed_resource();
	}
};

} // namespace resources
} // namespace xray

#endif // #ifndef RESOURCES_TEST_INPLACE_MANAGED_H_INCLUDED